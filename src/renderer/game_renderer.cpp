#include "renderer/game_renderer.hpp"
#include "api/game.hpp"

namespace munchkin {

GameRenderer::GameRenderer(Game& g, size_t window_w, size_t window_h) : game(&g) {
    renderer::RenderTarget::CreateInfo info;
    info.width = window_w;
    info.height = window_h;

    framebuf = std::move(renderer::RenderTarget(info));
}

void GameRenderer::render_frame() {
}

}