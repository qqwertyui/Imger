#ifndef PNG_HPP
#define PNG_HPP

#include <array>
#include <string>
#include <vector>

#include "Chunk.hpp"
#include "Image.hpp"
#include "Vector2.hpp"

class PNG : public Image {
public:
  PNG(std::string filename);
  PNG(std::vector<unsigned char> &input);
  ~PNG();

  unsigned char *get_pixels() const override;
  Vector2<int> get_size() const override;
  unsigned int get_bytes_per_pixel() const override;

  static bool is_valid(unsigned char *bytes);

private:
  static constexpr std::array<unsigned char, 8> SIGNATURE = {137, 80, 78, 71,
                                                             13,  10, 26, 10};
  static constexpr std::array<IHDR::Color_type, 5> SUPPORTED_TYPES = {
      IHDR::Color_type::RGB, IHDR::Color_type::RGBA, IHDR::Color_type::PALETTE,
      IHDR::Color_type::GREYSCALE, IHDR::Color_type::GREYSCALE_APLHA};
  void init(std::vector<unsigned char> &data);

  unsigned int get_color_type() const;
  bool is_supported_color_type(uint8_t type) const;
  bool are_critical_chunks_present() const;

  std::vector<Chunk *> get_chunks(const int type) const;
  bool is_chunk_present(const int type) const;
  unsigned int internal_get_bytes_per_pixel() const;

  void load_chunks(std::vector<unsigned char> &buffer);
  std::vector<unsigned char> decompress(std::vector<Chunk *> &data);
  std::vector<unsigned char> unfilter(std::vector<unsigned char> &data);
  std::vector<unsigned char> remap_palette(std::vector<unsigned char> &data);
  std::vector<unsigned char>
  grayscale_to_rgb(std::vector<unsigned char> &grayscale);

  void decode_line(unsigned char *destination, unsigned char *source,
                   int line_number) const;

  std::vector<Chunk *> chunk;
  std::vector<unsigned char> pixels;
  bool transparent = false;
};

#endif