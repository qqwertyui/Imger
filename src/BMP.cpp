#include "BMP.hpp"
#include "GenericException.hpp"

#include <fstream>
#include <cstdint>
#include <cstring>
#include <cmath>

BMP::BMP(std::string filename) {
    std::vector<unsigned char> data = Utils::read_file(filename, std::ifstream::binary);
    this->init(data);
}

BMP::BMP(std::vector<unsigned char> &data) {
    this->init(data);
}

void BMP::init(std::vector<unsigned char> &data) {
    unsigned char *ptr = data.data();
    memcpy(&this->header, ptr, sizeof(BMPHeader));
    ptr += sizeof(BMPHeader);

    uint16_t dib_size = *(uint16_t*)ptr;
    if(dib_size != BMP::DIB::SIGNATURE[0]) {
        throw GenericException("Unsupported DIB format", Code::UNSUPPORTED_FORMAT);
    }

    memcpy(&this->dib, ptr, sizeof(BitmapInfoHeader));
    ptr += sizeof(BitmapInfoHeader);


    if(this->dib.bpp != 24 && this->dib.bpp != 32) {
        throw GenericException("Unsupported color depth", Code::UNSUPPORTED_COLOR_DEPTH);
    }
    
    if(this->dib.compression_method != 0) {
        throw GenericException("Unsupported compression method", Code::UNSUPPORTED_COMPRESSION_METHOD);
    }

    unsigned int bytes_per_row = this->dib.width*(this->dib.bpp/8);
    unsigned int bitmap_size = bytes_per_row*abs(this->dib.height);
    std::vector<unsigned char> tmp(bitmap_size);

    unsigned char *src = data.data()+this->header.bitmap_offset;
    unsigned char *dst = tmp.data();
    int offset = bytes_per_row;

    // check whether image should be displayed uspide down
    if(this->dib.height > 0) {
        offset = -bytes_per_row;
        dst = tmp.data() + bitmap_size - bytes_per_row;
    }
  
    unsigned int strip_bytes = 4 - (bytes_per_row % 4);
    if(bytes_per_row % 4 == 0) {
        strip_bytes = 0;
    }

    for(int i=0; i<abs(this->dib.height); i++) {
        memcpy(dst, src, bytes_per_row);
        src += bytes_per_row + strip_bytes;
        dst += offset;
    }

    this->pixels = std::move(tmp);
}

bool BMP::is_valid(unsigned char *bytes) {
    if(memcmp(bytes, BMP::SIGNATURE.data(), BMP::SIGNATURE.size()) != 0) {
        return false;
    }
    return true;
}

Vector2<int> BMP::get_size() const {
    return Vector2<int>(this->dib.width, abs(this->dib.height));
}

unsigned char* BMP::get_pixels() const {
    return (unsigned char*)this->pixels.data();
}

unsigned int BMP::get_bytes_per_pixel() const {
    return (this->dib.bpp/8);
}

bool BMP::DIB::is_valid(unsigned char *bytes) {
    if(memcmp(bytes, BMP::DIB::SIGNATURE.data(), BMP::DIB::SIGNATURE.size()) != 0) {
        return false;
    }
    return true;
}