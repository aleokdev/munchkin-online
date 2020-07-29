#ifndef MUNCHKIN_RENDER_WRAPPER_HPP__
#define MUNCHKIN_RENDER_WRAPPER_HPP__

#include "systems/game_renderer.hpp"
#include "systems/title_screen_renderer.hpp"
#include "renderer/background_renderer.hpp"
#include "systems/jukebox_renderer.hpp"
#include "systems/game_gui_renderer.hpp"

#include <glm/mat4x4.hpp>

namespace munchkin {

class GameWrapper;

class RenderWrapper {
public:
    enum class State { TitleScreen, GamePlaying };

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
    systems::GameRenderer game_renderer;
    systems::TitleScreenRenderer title_screen_renderer;
    systems::GameGUIRenderer game_gui_renderer;
    systems::JukeboxRenderer jukebox_renderer;

private:
    State renderer_state = State::TitleScreen;

    float last_frame_time = 0;
};

} // namespace munchkin

#endif