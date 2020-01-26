#ifndef MUNCHKIN_RENDERER_UTIL_HPP_
#define MUNCHKIN_RENDERER_UTIL_HPP_

namespace munchkin {

namespace renderer {

unsigned int load_texture(const char* path);

unsigned int load_shader(const char* vtx_path, const char* frag_path);

} // namespace renderer

} // namespace munchkin

#endif