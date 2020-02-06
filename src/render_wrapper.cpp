#include "render_wrapper.hpp"
#include "game_wrapper.hpp"
#include "renderer/assets.hpp"

namespace munchkin {

RenderWrapper::RenderWrapper(GameWrapper& w) :
    wrapper(&w), 
    framebuf(renderer::RenderTarget::CreateInfo{wrapper->game.window_w, wrapper->game.window_h}),
    projection(glm::ortho(0.0f, (float)wrapper->game.window_w, 0.0f, (float)wrapper->game.window_h)),
    game_renderer(*this), title_screen_renderer(w) {

    title_screen_renderer.set_render_target(&framebuf);

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

RenderWrapper::~RenderWrapper() {}

void RenderWrapper::on_resize(size_t w, size_t h) {
    wrapper->game.window_w = w;
    wrapper->game.window_h = h;
    framebuf.resize(w, h);
    projection =
        glm::ortho(0.0f, (float)wrapper->game.window_w, 0.0f, (float)wrapper->game.window_h);
}

void RenderWrapper::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float frame_time = (float)SDL_GetTicks() / 1000.0f;
    float delta_time = frame_time - last_frame_time;
    last_frame_time = frame_time;

    // Set viewport to full window
    glViewport(0, 0, framebuf.get_width(), framebuf.get_height());

    renderer::RenderTarget::bind(framebuf);

    framebuf.clear(0, 0, 0, 1, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (renderer_state) {
        case State::TitleScreen: {
            auto status = title_screen_renderer.frame(delta_time);
            if (status == systems::TitleScreenRenderer::Status::EnterGamePlaying) {
                renderer_state = State::GamePlaying;
                wrapper->do_tick = true;
            } else if (status == systems::TitleScreenRenderer::Status::QuitApp) {
                wrapper->done = true;
            }
            break;
        }
        case State::GamePlaying: game_renderer.render(); break;
    }

    // Blit framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuf.handle());
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, framebuf.get_width(), framebuf.get_height(), 0, 0, framebuf.get_width(),
                      framebuf.get_height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace munchkin