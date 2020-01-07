#include "renderer/game_renderer.hpp"
#include "api/game.hpp"

#include <glad/glad.h>

namespace munchkin {

GameRenderer::GameRenderer(Game& g, size_t window_w, size_t window_h) : game(&g) {
    renderer::RenderTarget::CreateInfo info;
    info.width = window_w;
    info.height = window_h;

    framebuf = std::move(renderer::RenderTarget(info));
}

void GameRenderer::render_frame() {
    // Set viewport to full window
    glViewport(0, 0, framebuf.get_width(), framebuf.get_height());

    renderer::RenderTarget::bind(framebuf);
    
//    framebuf.clear(1, 0, 0, 1, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuf.handle());
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
}

void GameRenderer::blit(unsigned int target_framebuf) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuf.handle());
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, framebuf.get_width(), framebuf.get_height(), 
                      0, 0, framebuf.get_width(), framebuf.get_height(), 
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

}