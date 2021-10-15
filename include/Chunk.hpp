#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "Color.hpp"
#include "Utils.hpp"

class Chunk_content {
public:
  virtual ~Chunk_content();
};

class Chunk {
public:
  Chunk(int32_t length, int32_t type, unsigned char *data, int32_t crc32);
  ~Chunk();

  int32_t length;
  int32_t type;
  Chunk_content *content;
  int32_t crc;

  class Type {
  public:
    static constexpr int IHDR = 0x52444849;
    static constexpr int sRGB = 0x42475273;
    static constexpr int gAMA = 0x414d4167;
    static constexpr int pHYs = 0x73594870;
    static constexpr int IDAT = 0x54414449;
    static constexpr int IEND = 0x444e4549;
    static constexpr int bKGD = 0x44474b62;
    static constexpr int PLTE = 0x45544c50;
    static constexpr int tRNS = 0x534e5274;
    static constexpr std::array ALL = {IHDR, sRGB, gAMA, pHYs, IDAT,
                                       IEND, bKGD, PLTE, tRNS};
    static constexpr std::array CRITICAL = {IHDR, PLTE, IDAT, IEND};
  };
};

class IHDR : public Chunk_content {
public:
  IHDR(unsigned int chunk_length, unsigned char *data);

  uint32_t width;
  uint32_t height;
  uint8_t depth;
  uint8_t type;
  uint8_t compression_method;
  uint8_t filter_method;
  uint8_t interlace_method;

  enum Color_type {
    GREYSCALE = 0,
    RGB = 2,
    PALETTE = 3,
    GREYSCALE_APLHA = 4,
    RGBA = 6
  };
};

class IDAT : public Chunk_content {
public:
  IDAT(unsigned int chunk_length, unsigned char *data);

  std::vector<unsigned char> data;

  enum Filter { NONE = 0, SUB = 1, UP = 2, AVERAGE = 3, PAETH = 4 };
};

class PLTE : public Chunk_content {
public:
  PLTE(unsigned int chunk_length, unsigned char *data);
  std::vector<Color::RGB> pixels;
};

#endif