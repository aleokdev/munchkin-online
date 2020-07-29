#ifndef MUNCHKIN_INPUT_BINDER_HPP__
#define MUNCHKIN_INPUT_BINDER_HPP__

#include "input/input.hpp"

namespace munchkin {

class GameWrapper;

namespace systems {

// Binds input from the user to Game and its state
class InputBinder {
public:
    InputBinder(GameWrapper&);

    void tick();

private:
    GameWrapper* const wrapper;

    float last_frame_time;
    input::MouseState last_mouse_state;
};

} // namespace systems
} // namespace munchkin

#endif