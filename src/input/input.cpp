#include "input/input.hpp"

namespace munchkin {
namespace input {

MouseState mouse;
MouseState last_mouse;

void update(bool capture_mouse) {
	last_mouse = mouse;
	if(!capture_mouse)
    {
        mouse.button_flagmap = 0;
        mouse.x = mouse.y = -1;
    }
	else
	    mouse.button_flagmap = SDL_GetMouseState(&mouse.x, &mouse.y);
}

bool is_key_pressed(Keycode keycode) {
	const Uint8* state = SDL_GetKeyboardState(NULL);
	return state[SDL_GetScancodeFromKey(keycode)];
}

bool is_mousebutton_pressed(MouseButton mousebutton) {
	return mouse.button_flagmap & SDL_BUTTON((int)mousebutton);
}

bool has_mousebutton_been_clicked(MouseButton mousebutton)
{
	return !(last_mouse.button_flagmap & SDL_BUTTON((int)mousebutton)) && (mouse.button_flagmap & SDL_BUTTON((int)mousebutton));
}

MouseState get_current_mouse_state() {
	return mouse;
}

::math::Vec2D get_mouse_pos()
{
	return ::math::Vec2D{ (float)mouse.x, (float)mouse.y };
}

}
}


