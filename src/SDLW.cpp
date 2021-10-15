#include "SDLW.hpp"
#include "GenericException.hpp"
#include "PNG.hpp"

#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>

SDLW::SDLW(std::string window_title, unsigned int window_size_percent,
           Image *image)
    : window_title(window_title) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw GenericException(SDL_GetError(), Code::SDL_INIT_ERROR);
  }

  this->window_size = SDLW::get_screen_size_percent(window_size_percent);
  this->window = SDL_CreateWindow(this->get_formatted_window_title().c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, this->window_size.x,
                                  this->window_size.y, SDL_WINDOW_RESIZABLE);
  if (this->window == nullptr) {
    throw GenericException(SDL_GetError(), Code::SDL_INIT_ERROR);
  }
  this->renderer =
      SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
  if (this->renderer == nullptr) {
    throw GenericException(SDL_GetError(), Code::SDL_INIT_ERROR);
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  this->set_active_image(image);
}

SDLW::~SDLW() { SDL_Quit(); }

SDL_PixelFormatEnum
SDLW::bytes_per_pixel_to_sdl_format(unsigned int bytes_per_pixel) {
  if (bytes_per_pixel == 4) {
    return SDL_PIXELFORMAT_RGBA32;
  } else if (bytes_per_pixel == 3) {
    return SDL_PIXELFORMAT_RGB24;
  } else {
    throw GenericException("Invalid pixelformat", Code::INVALID_PIXELFORMAT);
  }
}

Vector2<int> SDLW::get_screen_size_percent(unsigned int percent) {
  SDL_DisplayMode dm;
  if (SDL_GetCurrentDisplayMode(0, &dm) < 0) {
    throw GenericException("Can't get display info", Code::DISPLAY_INFO_ERROR);
  }
  double multiplier = percent / 100.f;
  return Vector2<int>(dm.w * multiplier, dm.h * multiplier);
}

void SDLW::set_active_image(Image *image) {
  this->active_image = image;
  this->image_size = this->active_image->get_size();

  this->zoom = SDLW::adjust_zoom(this->image_size, this->window_size);
  SDL_PixelFormatEnum pixel_format =
      SDLW::bytes_per_pixel_to_sdl_format(image->get_bytes_per_pixel());
  this->texture =
      SDL_CreateTexture(this->renderer, pixel_format, SDL_TEXTUREACCESS_STATIC,
                        image_size.x, image_size.y);
  if (this->texture == nullptr) {
    throw GenericException(SDL_GetError(), Code::SDL_INIT_ERROR);
  }

  if (pixel_format == SDL_PIXELFORMAT_RGBA32) {
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);
  } else {
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_NONE);
  }

  this->rect.w = this->image_size.x * this->zoom;
  this->rect.h = this->image_size.y * this->zoom;
  this->rect.x =
      (double)this->window_size.x / 2.f - this->image_size.x * this->zoom / 2.f;
  this->rect.y =
      (double)this->window_size.y / 2.f - this->image_size.y * this->zoom / 2.f;
  this->update_window_title();
}

void SDLW::draw() {
  Image *image = this->active_image;
  unsigned int pitch = image->get_bytes_per_pixel() * image->get_size().x;
  SDL_UpdateTexture(this->texture, nullptr, image->get_pixels(), pitch);

  SDL_SetRenderDrawColor(this->renderer, this->background_color.r,
                         this->background_color.g, this->background_color.b,
                         255);
  SDL_RenderClear(this->renderer);
  SDL_RenderCopy(this->renderer, this->texture, nullptr, &this->rect);

  SDL_SetRenderDrawColor(this->renderer, this->background_color.r,
                         this->background_color.g, this->background_color.b,
                         255);
  // Creating temporary variables to operate on int instead of uint
  Vector2<int> wsize(this->window_size.x, this->window_size.y);
  int mlength = (int)this->margin_length;

  SDL_Rect margin_left = {0, 0, mlength, wsize.y};
  SDL_Rect margin_bottom = {0, wsize.y - mlength, wsize.x, mlength};
  SDL_Rect margin_right = {wsize.x - mlength, 0, mlength, wsize.y};
  SDL_Rect margin_top = {0, 0, wsize.x, mlength};

  SDL_Rect margins[4] = {margin_left, margin_bottom, margin_right, margin_top};
  SDL_RenderFillRects(this->renderer, margins, 4);
  SDL_RenderPresent(this->renderer);
}

void SDLW::poll_events() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_KEYDOWN: {
      if (e.key.keysym.sym == SDLK_ESCAPE)
        this->running = false;
      break;
    }
    case SDL_MOUSEWHEEL: {
      this->scroll_zoom(&e);
      this->update_window_title();
      break;
    }
    case SDL_MOUSEMOTION: {
      if (clicked == true) {
        if (this->fits_bounds(e.motion.xrel, e.motion.yrel) == true) {
          this->rect.x += e.motion.xrel;
          this->rect.y += e.motion.yrel;
        }
      }
      break;
    }
    case SDL_MOUSEBUTTONDOWN: {
      this->clicked = true;
      break;
    }
    case SDL_MOUSEBUTTONUP: {
      this->clicked = false;
      break;
    }
    case SDL_WINDOWEVENT: {
      if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
        Vector2<int> new_size(e.window.data1, e.window.data2);
        this->update_position_on_resize(new_size);
      }
      break;
    }

    case SDL_QUIT: {
      this->running = false;
      break;
    }
    }
  }
}

void SDLW::scroll_zoom(SDL_Event *e) {
  if ((e->wheel.y > 0) && this->zoom < 80.f) {
    this->zoom += SCROLLING_STEP;
  } else if ((e->wheel.y < 0) && this->zoom > 0.1f) {
    this->zoom -= SCROLLING_STEP;
  }

  Vector2<int> diff(this->rect.w - this->image_size.x * this->zoom,
                    this->rect.h - this->image_size.y * this->zoom);

  this->rect.w = this->image_size.x * this->zoom;
  this->rect.h = this->image_size.y * this->zoom;

  this->rect.x += diff.x / 2.f;
  this->rect.y += diff.y / 2.f;
}

bool SDLW::is_running() const { return this->running; }

bool SDLW::fits_bounds(int xdir, int ydir) {
  if (this->rect.x < -(this->image_size.x * this->zoom) && xdir < 0) {
    return false;
  }
  if (this->rect.x > (this->window_size.x + this->image_size.x * this->zoom) &&
      xdir > 0) {
    return false;
  }
  if (this->rect.y < -(this->image_size.y * this->zoom) && ydir < 0) {
    return false;
  }
  if (this->rect.y > (this->window_size.y + this->image_size.y * this->zoom) &&
      ydir > 0) {
    return false;
  }
  return true;
}

double SDLW::adjust_zoom(Vector2<int> &image, Vector2<int> &window) {
  Vector2<int> window_fixed = window;
  window_fixed.x -= 2 * this->margin_length;
  window_fixed.y -= 2 * this->margin_length;

  if (window_fixed.x > image.x && window_fixed.y > image.y) {
    return 1.f;
  }
  double width_ratio = ((double)(window_fixed.x) / (double)(image.x));
  double height_ratio = ((double)(window_fixed.y) / (double)(image.y));

  return (width_ratio > height_ratio) ? height_ratio : width_ratio;
}

void SDLW::set_backgronud_color(Color::RGB color) {
  this->background_color = color;
}

void SDLW::set_margin(unsigned int length) { this->margin_length = length; }

std::string SDLW::get_formatted_window_title() const {
  std::ostringstream out;
  out << this->window_title << " | " << this->image_size.x << " x "
      << this->image_size.y << " | " << round(this->zoom * 100) << "%";
  return out.str();
}

void SDLW::update_window_title() const {
  SDL_SetWindowTitle(this->window, this->get_formatted_window_title().c_str());
}

void SDLW::update_position_on_resize(Vector2<int> &new_size) {
  Vector2<double> position_percent(
      (double)this->rect.x / (double)this->window_size.x,
      (double)this->rect.y / (double)this->window_size.y);

  this->rect.x = new_size.x * position_percent.x;
  this->rect.y = new_size.y * position_percent.y;
  this->window_size = new_size;
}