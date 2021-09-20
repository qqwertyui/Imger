#ifndef ICOSTRUCTS_HPP
#define ICOSTRUCTS_HPP

#include <cstdint>

struct ICOheader {
    uint16_t reserved;
    uint16_t type;
    uint16_t number_of_images;

    enum Type {
        ICON = 1,
        CURSOR = 2
    };
};

struct ICOdirectory {
    uint8_t width;
    uint8_t height;
    uint8_t color_count;
    uint8_t reserved;
    uint16_t color_planes;
    uint16_t bits_per_pixel;
    uint32_t bitmap_size;
    uint32_t offset;
};

#endif