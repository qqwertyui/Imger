#include "SDLW.hpp"
#include "Log.hpp"
#include "Image.hpp"
#include "GenericException.hpp"

constexpr const char *WINDOW_TITLE = "Imger";
constexpr unsigned int WINDOW_PERCENT_SIZE = 50; // 50% of screen size 

int main(int argc, char **argv) {
	if(argc < 2) {
		Log::errorf("Usage: %s <image>", argv[0]);
		return Code::INVALID_ARGS;
	}
	Image *image = nullptr;
    try {
        image = Image::load(argv[1]);
    } catch(GenericException &e) {
        Log::errorf("Couldn't load image: %s\n", e.what());
        return e.get_code();
    }
    SDLW *display = nullptr;
    try {
        display = new SDLW(WINDOW_TITLE, WINDOW_PERCENT_SIZE, image);
    } catch(GenericException &e) {
        Log::errorf("Couldn't initialize SDL: %s\n", e.what());
        return e.get_code();
    }
	display->set_backgronud_color(Color::DEFAULT);
    display->set_margin(15);
    //display->set_active_image(image);

	while(display->is_running() == true) {
		display->poll_events();
		display->draw();
	}
    
    return Code::OK;
}