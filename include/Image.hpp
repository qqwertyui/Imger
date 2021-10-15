#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>

#include "Color.hpp"
#include "Utils.hpp"
#include "Vector2.hpp"

class Image {
public:
  virtual ~Image();

  virtual Vector2<int> get_size() const = 0;
  virtual unsigned int get_bytes_per_pixel() const = 0;
  virtual unsigned char *get_pixels() const = 0;

  virtual bool is_transparent() const;
  virtual Color::RGB get_background_color() const;

  static Image *load(std::string path);
  static Image *load(std::vector<unsigned char> &data);
  static Image *find_and_load(std::vector<unsigned char> &data);

private:
  static constexpr unsigned int MAX_SIGNATURE_SIZE = 16;
};

#endif