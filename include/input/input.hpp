#ifndef MUNCHKIN_INPUT_HPP__
#define MUNCHKIN_INPUT_HPP__
#include <SDL.h>
#include "util/pos_vec.hpp"

namespace munchkin {
namespace input {

enum class MouseButton {
	left = 1,
	middle = 2,
	right = 3
};
using Keycode = SDL_Keycode;

struct MouseState {
	int x = 0;
	int y = 0;
	int button_flagmap = 0;
};

void update();

bool is_key_pressed(Keycode);
bool is_mousebutton_pressed(MouseButton mouse_button);
// Returns true if the mousebutton given was released the previous frame and is now pressed.
bool has_mousebutton_been_clicked(MouseButton mouse_button);

MouseState get_current_mouse_state();
::math::Vec2D get_mouse_pos();
	
}
}



#endif