#ifndef SDLW_HPP
#define SDLW_HPP

#include <SDL2/SDL.h>
#include <string>

#include "Color.hpp"
#include "GenericException.hpp"
#include "Image.hpp"
#include "PNG.hpp"
#include "Vector2.hpp"

class SDLW {
public:
  SDLW(std::string window_title, unsigned int window_size_percent,
       Image *image);
  ~SDLW();

  void draw();
  void poll_events();
  bool is_running() const;
  void set_backgronud_color(Color::RGB color);
  void set_margin(unsigned int length);

private:
  static SDL_PixelFormatEnum
  bytes_per_pixel_to_sdl_format(unsigned int bytes_per_pixel);
  static Vector2<int> get_screen_size_percent(unsigned int percent);

  std::string get_formatted_window_title() const;
  void update_window_title() const;
  bool fits_bounds(int xdir, int ydir);
  void scroll_zoom(SDL_Event *e);
  double adjust_zoom(Vector2<int> &image, Vector2<int> &window);
  void update_position_on_resize(Vector2<int> &new_size);
  void set_active_image(Image *image);

  Vector2<int> window_size;
  Vector2<int> image_size;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Rect rect;

  Image *active_image = nullptr;
  std::string window_title;

  // initialized variables
  Color::RGB background_color = Color::DEFAULT;
  unsigned int margin_length = 15;
  double zoom = 1.f; // 100%
  bool running = true;
  bool clicked = false;

  // constants
  static constexpr double SCROLLING_STEP = 0.05; // +/- 5% per scroll tick
};

#endif