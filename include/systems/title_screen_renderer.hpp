#ifndef MUNCHKIN_TITLE_SCREEN_RENDERER_HPP_
#define MUNCHKIN_TITLE_SCREEN_RENDERER_HPP_

#include "renderer/render_target.hpp"
#include "renderer/background_renderer.hpp"
#include "renderer/assets.hpp"

namespace munchkin::systems {

class TitleScreenRenderer {
public:
    enum class Status {
        // Regular title screen
        None,
        // Transitioning to GamePlaying state
        TransitionGamePlaying,
        // Signals that the title screen ends, enter the GamePlaying state
        EnterGamePlaying
    };

    TitleScreenRenderer();
    void set_render_target(renderer::RenderTarget* tg);

    Status frame(float delta_time);

private:
    renderer::RenderTarget* target;

    renderer::Background background;
    assets::Handle<renderer::Shader> sprite_shader;
    assets::Handle<renderer::Font> font;

    Status status;
};

} // namespace munchkin::renderer

#endif