#include <cstring>
#include <fstream>

#include "GenericException.hpp"
#include "PNG.hpp"
#include "Utils.hpp"

PNG::PNG(std::string filename) {
  std::vector<unsigned char> data =
      Utils::read_file(filename, std::fstream::binary);
  this->init(data);
}

PNG::PNG(std::vector<unsigned char> &data) { this->init(data); }

void PNG::init(std::vector<unsigned char> &data) {
  this->load_chunks(data);
  IHDR *ihdr = (IHDR *)this->get_chunks(Chunk::Type::IHDR)[0]->content;
  if (this->is_supported_color_type(ihdr->type) == false) {
    throw GenericException("Unsupported color type",
                           Code::UNSUPPORTED_COLOR_TYPE);
  }
  if (ihdr->type == IHDR::Color_type::RGBA ||
      ihdr->type == IHDR::Color_type::GREYSCALE_APLHA) {
    this->transparent = true;
  }
  std::vector<Chunk *> idat_chunks = this->get_chunks(Chunk::Type::IDAT);
  std::vector<unsigned char> decompressed = this->decompress(idat_chunks);
  this->pixels = this->unfilter(decompressed);

  if (ihdr->type == IHDR::Color_type::PALETTE) {
    this->pixels = this->remap_palette(this->pixels);
  } else if (ihdr->type == IHDR::Color_type::GREYSCALE ||
             ihdr->type == IHDR::Color_type::GREYSCALE_APLHA) {
    this->pixels = this->grayscale_to_rgb(this->pixels);
  }
}

PNG::~PNG() {
  for (Chunk *c : this->chunk) {
    delete c;
  }
}

Vector2<int> PNG::get_size() const {
  IHDR *ihdr = (IHDR *)this->get_chunks(Chunk::Type::IHDR)[0]->content;
  return Vector2<int>(ihdr->width, ihdr->height);
}

unsigned char *PNG::get_pixels() const {
  return (unsigned char *)this->pixels.data();
}

unsigned int PNG::get_bytes_per_pixel() const {
  return (this->transparent == true) ? 4 : 3;
}

unsigned int PNG::get_color_type() const {
  return ((IHDR *)this->get_chunks(Chunk::Type::IHDR)[0]->content)->type;
}

unsigned int PNG::internal_get_bytes_per_pixel() const {
  IHDR *ihdr = (IHDR *)this->get_chunks(Chunk::Type::IHDR)[0]->content;
  unsigned int bytes_per_pallete_idx = (ihdr->depth / 8);
  unsigned int samples = 1; // GREYSCALE, PALETTE
  if (ihdr->type == IHDR::Color_type::GREYSCALE_APLHA) {
    samples = 2;
  } else if (ihdr->type == IHDR::Color_type::RGB) {
    samples = 3;
  } else if (ihdr->type == IHDR::Color_type::RGBA) {
    samples = 4;
  }
  return (bytes_per_pallete_idx * samples);
}

std::vector<Chunk *> PNG::get_chunks(const int type) const {
  std::vector<Chunk *> chunks;
  for (Chunk *c : this->chunk) {
    if (c->type == type) {
      chunks.push_back(c);
    }
  }
  return chunks;
}

bool PNG::are_critical_chunks_present() const {
  if (this->is_chunk_present(Chunk::Type::IHDR) == false) {
    return false;
  }
  if (this->is_chunk_present(Chunk::Type::PLTE) == false) {
    if (this->get_color_type() == IHDR::Color_type::PALETTE) {
      return false;
    }
  }
  if (this->is_chunk_present(Chunk::Type::IDAT) == false) {
    return false;
  }
  if (this->is_chunk_present(Chunk::Type::IEND) == false) {
    return false;
  }
  return true;
}

bool PNG::is_valid(unsigned char *bytes) {
  if (memcmp(bytes, PNG::SIGNATURE.data(), PNG::SIGNATURE.size()) == 0) {
    return true;
  }
  return false;
}

bool PNG::is_supported_color_type(uint8_t type) const {
  for (IHDR::Color_type t : PNG::SUPPORTED_TYPES) {
    if (t == type) {
      return true;
    }
  }
  return false;
}

bool PNG::is_chunk_present(const int type) const {
  return (this->get_chunks(type).size() > 0);
}

std::vector<unsigned char> PNG::decompress(std::vector<Chunk *> &data) {
  unsigned int raw_size = 0;
  for (Chunk *c : data) {
    raw_size += c->length;
  }
  std::vector<unsigned char> raw(raw_size);
  unsigned int offset = 0;

  // iterate through all IDAT's and merge datastreams
  for (Chunk *c : data) {
    IDAT *idat = (IDAT *)c->content;
    memcpy(raw.data() + offset, idat->data.data(), c->length);
    offset += c->length;
  }
  return Utils::zlib_inflate(raw);
}

std::vector<unsigned char>
PNG::remap_palette(std::vector<unsigned char> &indexes) {
  PLTE *plte = (PLTE *)this->get_chunks(Chunk::Type::PLTE)[0]->content;
  std::vector<unsigned char> result(indexes.size() * sizeof(Color::RGB));

  Color::RGB *colors = (Color::RGB *)plte->pixels.data();
  unsigned char *destination = result.data();
  for (unsigned int i = 0; i < indexes.size(); i++) {
    memcpy(destination, &colors[indexes[i]], sizeof(Color::RGB));
    destination += sizeof(Color::RGB);
  }
  return result;
}

std::vector<unsigned char>
PNG::grayscale_to_rgb(std::vector<unsigned char> &grayscale) {
  unsigned int buffer_size = grayscale.size() * sizeof(Color::RGB);
  if (this->transparent) {
    buffer_size += grayscale.size();
  }
  std::vector<unsigned char> rgb(buffer_size);
  unsigned char *ptr = rgb.data();

  for (unsigned int i = 0; i < grayscale.size(); i++) {
    memset(ptr, grayscale[i], sizeof(Color::RGB));
    if (this->transparent) {
      *(ptr + 3) = grayscale[i + 1];
      i += 1;
      ptr += 1;
    }
    ptr += 3;
  }
  return rgb;
}

std::vector<unsigned char> PNG::unfilter(std::vector<unsigned char> &data) {
  Vector2<int> image_size = this->get_size();

  // PNG picture contains image_size.y filtering bytes (one per line)
  std::vector<unsigned char> result(data.size() - image_size.y);
  for (int line = 0; line < image_size.y; line++) {
    this->decode_line(result.data(), data.data(), line);
  }
  return result;
}

void PNG::decode_line(unsigned char *destination, unsigned char *source,
                      int line_number) const {
  Vector2<int> image_size = this->get_size();

  int bytes_per_pixel = this->internal_get_bytes_per_pixel();
  int pixels_per_line = image_size.x * this->internal_get_bytes_per_pixel();
  int destination_offset = line_number * pixels_per_line;
  int source_offset = line_number * pixels_per_line + line_number;

  unsigned char *buffer = &destination[destination_offset];
  unsigned char *current_scanline = &source[source_offset] + 1;
  unsigned char *previous_scanline = nullptr;

  if (line_number > 0) {
    previous_scanline = &destination[destination_offset - pixels_per_line];
  }

  uint8_t filter = current_scanline[-1];
  if (filter == IDAT::Filter::NONE) {
    memcpy(buffer, current_scanline, pixels_per_line);
  } else if (filter == IDAT::Filter::SUB) {
    for (int i = 0; i < pixels_per_line; i++) {
      if (i >= bytes_per_pixel) {
        buffer[i] = current_scanline[i] + buffer[i - bytes_per_pixel];
      } else {
        buffer[i] = current_scanline[i];
      }
    }
  } else if (filter == IDAT::Filter::UP) {
    for (int i = 0; i < pixels_per_line; i++) {
      if (previous_scanline) {
        buffer[i] = current_scanline[i] + previous_scanline[i];
      } else {
        buffer[i] = current_scanline[i];
      }
    }
  } else if (filter == IDAT::Filter::AVERAGE) {
    float left = 0;
    float up = 0;
    for (int i = 0; i < pixels_per_line; i++) {
      if (i >= bytes_per_pixel) {
        left = buffer[i - bytes_per_pixel];
      }
      if (previous_scanline) {
        up = previous_scanline[i];
      }
      buffer[i] = current_scanline[i] + (int)((left + up) / 2.f);
    }
  } else if (filter == IDAT::Filter::PAETH) {
    unsigned int left = 0;
    unsigned int up = 0;
    unsigned int left_up = 0;

    for (int i = 0; i < pixels_per_line; i++) {
      if (i >= bytes_per_pixel) {
        left = buffer[i - bytes_per_pixel];
      }
      if (previous_scanline) {
        up = previous_scanline[i];
      }
      if ((previous_scanline != nullptr) && (i >= bytes_per_pixel)) {
        left_up = previous_scanline[i - bytes_per_pixel];
      }
      buffer[i] = current_scanline[i] + Utils::paeth(left, up, left_up);
    }
  }
}

void PNG::load_chunks(std::vector<unsigned char> &buffer) {
  unsigned char *ptr = buffer.data();
  ptr += PNG::SIGNATURE.size(); // skip signature

  while (ptr < &buffer.back()) {
    uint32_t length = Utils::se32(*(uint32_t *)ptr);
    uint32_t type = *(uint32_t *)(ptr + sizeof(Chunk::length));
    unsigned char *raw = (ptr + sizeof(Chunk::length) + sizeof(Chunk::type));
    uint32_t crc32 = Utils::se32(*(uint32_t *)(raw + length));

    this->chunk.push_back(new Chunk(length, type, raw, crc32));
    ptr += (sizeof(Chunk::length) + sizeof(Chunk::type) + sizeof(Chunk::crc) +
            length);
  }

  if (this->are_critical_chunks_present() == false) {
    throw GenericException("Critical chunks not found",
                           Code::CRITICAL_CHUNKS_NOT_FOUND);
  }
}