#ifndef MUNCHKIN_ONLINE_TEXTURE_HPP_
#define MUNCHKIN_ONLINE_TEXTURE_HPP_

namespace munchkin {
namespace renderer {

struct Texture {
    unsigned int handle;
    size_t w, h;
};

} // namespace renderer
} // namespace munchkin

#endif