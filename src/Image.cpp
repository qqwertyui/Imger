#include "Image.hpp"
#include "BMP.hpp"
#include "PNG.hpp"
#include "ICO.hpp"
#include "GenericException.hpp"

Image::~Image() { }

Image* Image::load(std::string path) {
    std::vector<unsigned char> data = Utils::read_file(path, std::ifstream::binary);
    return Image::find_and_load(data);
}

Image* Image::load(std::vector<unsigned char> &data) {
    return Image::find_and_load(data);
}

Image* Image::find_and_load(std::vector<unsigned char> &data) {
    unsigned char signature[Image::MAX_SIGNATURE_SIZE];
    memcpy(signature, data.data(), Image::MAX_SIGNATURE_SIZE);

    if(BMP::is_valid(signature) == true) {
        return (new BMP(data));
    } else if(PNG::is_valid(signature) == true) {
        return (new PNG(data));
    } else if(ICO::is_valid(signature) == true) {
        return (new ICO(data));
    } else {
        throw GenericException("Unrecognized format", Code::UNRECOGNIZED_FORMAT);
    }
}

Color::RGB Image::get_background_color() const {
    return Color::DEFAULT;
}

bool Image::is_transparent() const {
    return false;
}