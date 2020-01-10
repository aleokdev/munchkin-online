#ifndef MUNCHKIN_RENDERER_UNIFORM_BUFFER_HPP_
#define MUNCHKIN_RENDERER_UNIFORM_BUFFER_HPP_

#include <glad/glad.h>

namespace munchkin::renderer {

class UniformBuffer {
public:
    UniformBuffer(size_t binding, size_t initial_size = 0, GLenum mode = GL_STATIC_DRAW);
    ~UniformBuffer();

    unsigned int handle();

    size_t get_binding() const;

    static void bind(UniformBuffer& buf);

    // Assumes the buffer has been bound
    void write_data(float const* data, size_t byte_size, size_t byte_offset = 0);

private:
    unsigned int ubo = 0;
    size_t binding = 0;
};

}

#endif