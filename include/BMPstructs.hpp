#ifndef BMPSTRUCTS_HPP
#define BMPSTRUCTS_HPP

#include <cstdint>

struct BMPHeader {
  uint16_t signature;
  uint32_t file_size;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t bitmap_offset;
} __attribute__((packed));

struct BitmapInfoHeader {
  uint32_t header_size;
  int32_t width;
  int32_t height;
  uint16_t color_panes;
  uint16_t bpp;
  uint32_t compression_method;
  uint32_t image_size;
  int32_t horizontal_resolution;
  int32_t vertical_resolution;
  uint32_t palette_colors;
  uint32_t important_colors;
} __attribute__((packed));

typedef BitmapInfoHeader DeviceIndependedBitmap;

#endif