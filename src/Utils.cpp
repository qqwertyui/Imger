#include <cmath>
#include <cstring>
#include <zlib.h>

#include "GenericException.hpp"
#include "Utils.hpp"

Utils::Data::Data(unsigned char *data, unsigned int data_size) {
  this->data = data;
  this->data_size = data_size;
}

Utils::Data::Data() {}

Utils::Chunk_manager::Chunk_manager() {
  this->elements = 0;
  this->total_bytes = 0;
}

Utils::Chunk_manager::~Chunk_manager() {
  for (Utils::Data_vector::iterator i = this->entry.begin();
       i != this->entry.end(); ++i) {
    delete[](*i)->data;
  }
  for (Utils::Data_vector::iterator i = this->entry.begin();
       i != this->entry.end(); ++i) {
    delete (*i);
  }
}

void Utils::Chunk_manager::add(unsigned char *data, unsigned int data_size) {
  unsigned char *tmp = new unsigned char[data_size];
  memcpy(tmp, data, data_size);

  this->entry.push_back(new Utils::Data(tmp, data_size));
  this->elements += 1;
  this->total_bytes += data_size;
}

unsigned char *Utils::Chunk_manager::to_bytearray() {
  unsigned char *bytes = new unsigned char[this->total_bytes];
  unsigned int counter = 0;
  for (Utils::Data_vector::iterator i = this->entry.begin();
       i != this->entry.end(); ++i) {
    memcpy(bytes + counter, (*i)->data, (*i)->data_size);
    counter += (*i)->data_size;
  }
  return bytes;
}

std::vector<unsigned char>
Utils::zlib_inflate(std::vector<unsigned char> &input) {
  constexpr int INCLUDE_ZLIB_HEADER_AND_TRAILER = 15;
  constexpr unsigned int CHUNKSZ = 0x1000;
  Utils::Chunk_manager container;
  z_stream infstream;

  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;
  infstream.avail_in = (uInt)input.size();
  infstream.next_in = input.data();

  int status = inflateInit2(&infstream, INCLUDE_ZLIB_HEADER_AND_TRAILER);
  if (status != Z_OK) {
    throw GenericException(infstream.msg, Code::DECOMPRESSION_ERROR);
  }

  unsigned char temp[CHUNKSZ] = {0};
  do {
    infstream.avail_out = CHUNKSZ;
    infstream.next_out = temp;

    status = inflate(&infstream, Z_NO_FLUSH);
    if (status != Z_OK && status != Z_STREAM_END) {
      inflateEnd(&infstream);
      throw GenericException(infstream.msg, Code::DECOMPRESSION_ERROR);
    }
    int have = CHUNKSZ - infstream.avail_out;
    container.add(temp, have);
  } while (status != Z_STREAM_END);

  inflateEnd(&infstream);
  std::vector<unsigned char> result(container.total_bytes);
  memcpy(result.data(), container.to_bytearray(), result.size());
  return result;
}

uint32_t Utils::crc32(const unsigned char *data, size_t datasz) {
  uint32_t _crc = 0xffffffff;
  for (size_t i = 0; i < datasz; i++) {
    uint8_t top_left = data[i];
    for (size_t j = 0; j < 8; j++) {
      uint8_t zero = (top_left ^ _crc) & 1;
      _crc >>= 1;
      if (zero)
        _crc = _crc ^ 0xedb88320;
      top_left >>= 1;
    }
  }
  return ~_crc;
}

uint32_t Utils::se32(uint32_t number) { return __builtin_bswap32(number); }

uint16_t Utils::se16(uint16_t number) { return __builtin_bswap16(number); }

int Utils::paeth(int left, int top, int top_left) {
  int p = left + top - top_left;
  int pa = abs(p - left);
  int pb = abs(p - top);
  int pc = abs(p - top_left);

  if ((pa <= pb) && (pa <= pc)) {
    return left;
  } else if (pb <= pc) {
    return top;
  } else {
    return top_left;
  }
}

std::vector<unsigned char> Utils::read_file(std::string &filename,
                                            std::ifstream::openmode flags) {
  std::ifstream file(filename, flags);
  if (file.good() == false) {
    throw GenericException("File error", Code::FILE_ERROR);
  }

  file.seekg(0, std::ios_base::end);
  unsigned int filesz = file.tellg();
  file.seekg(0, std::ios_base::beg);

  std::vector<unsigned char> result(filesz);
  file.read((char *)result.data(), result.capacity());
  file.close();
  return result;
}