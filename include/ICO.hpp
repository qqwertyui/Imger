#ifndef ICO_HPP
#define ICO_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "ICOstructs.hpp"
#include "PNG.hpp"

class ICO : public Image {
public:
  ICO(std::string filename);
  ICO(std::vector<unsigned char> &data);
  ~ICO();

  Vector2<int> get_size() const override;
  unsigned int get_bytes_per_pixel() const override;
  unsigned char *get_pixels() const override;

  static bool is_valid(unsigned char *bytes);

private:
  static constexpr std::array<unsigned char, 4> SIGNATURE = {0, 0, 1, 0};
  void init(std::vector<unsigned char> &data);

  ICOheader header;
  std::vector<ICOdirectory *> directories;
  Image *image;
};

#endif