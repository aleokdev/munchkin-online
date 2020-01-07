#include <renderer/render_target.hpp>

#include <glad/glad.h>
#include <cassert>
#include <utility>

namespace munchkin::renderer {

RenderTarget::RenderTarget(CreateInfo const& info) {
    assert(info.width > 0 && info.height > 0 && "RenderTarget::RenderTarget(): Size cannot be zero.");

    width = info.width;
    height = info.height;

    glGenTextures(1, &texture);
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);

    // Bind the stuff we need
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Fill texture with empty data
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create renderbuffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    assert(valid() && "RenderTarget::RenderTarget(): Failed to create render target");
}

RenderTarget::RenderTarget(RenderTarget&& rhs) {
    if (valid()) {
        destroy();
    }

    std::swap(texture, rhs.texture);
    std::swap(fbo, rhs.fbo);
    std::swap(rbo, rhs.rbo);

    std::swap(width, rhs.width);
    std::swap(height, rhs.height);
}

RenderTarget& RenderTarget::operator=(RenderTarget&& rhs) {
    if (valid()) {
        destroy();
    }

    std::swap(texture, rhs.texture);
    std::swap(fbo, rhs.fbo);
    std::swap(rbo, rhs.rbo);

    std::swap(width, rhs.width);
    std::swap(height, rhs.height); 

    return *this;
}

RenderTarget::~RenderTarget() {
    if (valid()) {
        destroy();
    }
}

void RenderTarget::bind(RenderTarget& target) {
    glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
}

bool RenderTarget::valid() const {
    return texture != 0 && fbo != 0 && rbo != 0 && width != 0 && height != 0;
}

void RenderTarget::clear(float r, float b, float g, float a, 
                         unsigned int flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) {
    bind(*this);
    glClearColor(r, g, b, a);
    glClear(flags);
}

size_t RenderTarget::get_width() const {
    return width;
}

size_t RenderTarget::get_height() const {
    return height;
}

size_t RenderTarget::handle() {
    return fbo;
}

size_t RenderTarget::texture_handle() {
    return texture;
}

void RenderTarget::destroy() {
    assert(valid() && "RenderTarget::destroy() called on invalid render target.");
    // deallocate gl resources
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    // reset size
    width = 0;
    height = 0;
}


}