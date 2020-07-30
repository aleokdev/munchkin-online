#ifndef MUNCHKIN_INPUT_BINDER_HPP__
#define MUNCHKIN_INPUT_BINDER_HPP__

#include "input/input.hpp"

namespace munchkin {

class GameWrapper;

namespace systems {

// Binds input from the user to Game and its state
class [[system_codegen::wrapper_instance("input_binder")]] InputBinder {
public:
    InputBinder(GameWrapper&);

    void load_content() {}

    void tick();

private:
    GameWrapper* const wrapper;

    float last_frame_time;
    input::MouseState last_mouse_state;
};

} // namespace systems
} // namespace munchkin

#endif