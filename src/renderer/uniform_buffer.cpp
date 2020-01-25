#include "renderer/uniform_buffer.hpp"

#include <glad/glad.h>

namespace munchkin::renderer {

UniformBuffer::UniformBuffer(size_t binding, size_t initial_size, GLenum mode) : binding(binding) {
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    if (initial_size > 0) {
        glBufferData(GL_UNIFORM_BUFFER, initial_size, nullptr, mode);
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
}

UniformBuffer::~UniformBuffer() {
    if (ubo) {
        glDeleteBuffers(1, &ubo);
    }
}

unsigned int UniformBuffer::handle() { return ubo; }

size_t UniformBuffer::get_binding() const { return binding; }

void UniformBuffer::bind(UniformBuffer& buf) { glBindBuffer(GL_UNIFORM_BUFFER, buf.ubo); }

void UniformBuffer::write_data(float const* data, size_t byte_size, size_t byte_offset) {
    glBufferSubData(GL_UNIFORM_BUFFER, byte_offset, byte_size, data);
}

} // namespace munchkin::renderer