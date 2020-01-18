#ifndef MUNCHKIN_FONT_HPP_
#define MUNCHKIN_FONT_HPP_

#include <glm/glm.hpp>
#include <unordered_map>
#include <filesystem>

namespace munchkin {
namespace renderer {

class Font {
public:
    Font() = default;
    Font(std::filesystem::path path);
    ~Font();

    struct glyph_data {
        unsigned int texture;
        // size of the glyph
        glm::vec2 size;
        // offset from baseline to left/top of the glyph
        glm::vec2 bearing;
        // offset to advance to next glyph
        unsigned int advance;
        // the pixel size this character was rendered with in the font texture
        size_t pixel_size;
    };

    std::unordered_map<char, glyph_data> glyphs;

    void swap(Font& other);
};

}
}

#endif