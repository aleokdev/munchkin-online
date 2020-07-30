#ifndef MUNCHKIN_RENDER_WRAPPER_HPP__
#define MUNCHKIN_RENDER_WRAPPER_HPP__

#include "renderer/render_target.hpp"

#include <glm/mat4x4.hpp>

#include "render_systems_cg.hpp"

namespace munchkin {

class GameWrapper;

class RenderWrapper {
public:
    enum class State { TitleScreen, GamePlaying };

    // Defined in codegen'd systems_cg.hpp
    RenderWrapper(GameWrapper&);
    ~RenderWrapper();

    void render();
    void on_resize(size_t w, size_t h);

    State get_state() { return renderer_state; }
    void set_state(State s) { renderer_state = s; }

    // Rendering data
    GameWrapper* wrapper;

    renderer::RenderTarget framebuf;
    glm::mat4 projection;

    // Systems
    MUNCHKIN_RENDER_WRAPPER_SYSTEMS

private:
    State renderer_state = State::TitleScreen;

    float last_frame_time = 0;
};

} // namespace munchkin

#endif