#include "Chunk.hpp"
#include "GenericException.hpp"
#include "Vector2.hpp"

#include <cstring>
#include <vector>

Chunk::Chunk(int32_t length, int32_t type, unsigned char *data, int32_t crc32) {
  this->length = length;
  this->type = type;
  this->crc = crc32;

  if (type == Chunk::Type::IHDR) {
    this->content = (Chunk_content *)new IHDR(length, data);
  } else if (type == Chunk::Type::IDAT) {
    this->content = (Chunk_content *)new IDAT(length, data);
  } else if (type == Chunk::Type::PLTE) {
    this->content = (Chunk_content *)new PLTE(length, data);
  }
}

Chunk::~Chunk() { delete this->content; }

Chunk_content::~Chunk_content() {}

IHDR::IHDR(unsigned int chunk_length, unsigned char *data) {
  memcpy(&this->width, data, chunk_length);
  this->width = Utils::se32(this->width);
  this->height = Utils::se32(this->height);
}

PLTE::PLTE(unsigned int chunk_length, unsigned char *data) {
  if (chunk_length % 3 != 0) {
    throw GenericException("Invalid PLTE length", Code::INVALID_PLTE_LENGTH);
  }
  unsigned int number_of_pixels = chunk_length / 3;
  this->pixels.resize(number_of_pixels);
  Color::RGB *input = (Color::RGB *)data;

  for (unsigned int i = 0; i < this->pixels.size(); i++) {
    this->pixels[i] = input[i];
  }
}

IDAT::IDAT(unsigned int chunk_length, unsigned char *data) {
  this->data.resize(chunk_length);
  memcpy(this->data.data(), data, this->data.capacity());
}
