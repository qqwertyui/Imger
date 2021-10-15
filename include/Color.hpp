#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

namespace Color {
class RGB {
public:
  RGB() = default;
  constexpr RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
  uint8_t r, g, b;
};

// Constants
inline constexpr RGB RED = RGB(255, 0, 0);
inline constexpr RGB GREEN = RGB(0, 255, 0);
inline constexpr RGB BLUE = RGB(0, 0, 255);
inline constexpr RGB BLACK = RGB(0, 0, 0);
inline constexpr RGB WHITE = RGB(255, 255, 255);
inline constexpr RGB DEFAULT = RGB(43, 43, 43);
}; // namespace Color

#endif