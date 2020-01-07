#ifndef MUNCHKIN_RENDERER_RENDER_TARGET_HPP_
#define MUNCHKIN_RENDERER_RENDER_TARGET_HPP_

namespace munchkin {

namespace renderer {

class RenderTarget {
public:
    struct CreateInfo {
        size_t width, height;
    };

    RenderTarget() = default;
    // Note: This function changes the GL state. After this call, the framebuffer owned by this RenderTarget and it's 
    // corresponding texture and renderbuffer will be bound.
    RenderTarget(CreateInfo const& info);
    RenderTarget(RenderTarget const&) = delete;
    RenderTarget(RenderTarget&& rhs);

    RenderTarget& operator=(RenderTarget const& rhs) = delete;
    RenderTarget& operator=(RenderTarget&& rhs);

    ~RenderTarget();

    static void bind(RenderTarget& target);

    // Returns whether the render target is valid. A render target is considered valid when it has a valid texture,
    // fbo and rbo attached, and none of the dimensions is 0.
    bool valid() const;

    // Clears the stored framebuffer
    void clear(float r, float g, float b, float a, unsigned int flags /* = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT*/);

    size_t get_width() const;
    size_t get_height() const;

    // Returns the handle to the OpenGL framebuffer object
    size_t handle();
    
    // Returns the handle to the OpenGL texture object
    size_t texture_handle();

private:
    unsigned int texture = 0;
    unsigned int fbo = 0;
    unsigned int rbo = 0;

    size_t width = 0;
    size_t height = 0;

    // assumes valid() == true
    void destroy();
};

}

}

#endif