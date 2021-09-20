#ifndef BMP_HPP
#define BMP_HPP

#include <string>
#include <array>

#include "BMPstructs.hpp"
#include "Vector2.hpp"
#include "Image.hpp"

class BMP : public Image {
public:
    BMP(std::string filename);
    BMP(std::vector<unsigned char> &data);

    Vector2<int> get_size() const override;
    unsigned char* get_pixels() const override;
    unsigned int get_bytes_per_pixel() const override;

    static bool is_valid(unsigned char *bytes);

    class DIB {
    public:
        static bool is_valid(unsigned char *bytes);
        static constexpr std::array <unsigned char, 1> SIGNATURE = { 40 };
    };

private:
    static constexpr std::array <unsigned char, 2> SIGNATURE = { 'B', 'M' };
    void init(std::vector<unsigned char> &data);

    BMPHeader header;
    BitmapInfoHeader dib;
    std::vector<unsigned char> pixels;
};

#endif