#ifndef MUNCHKIN_ONLINE_UTIL_HPP_
#define MUNCHKIN_ONLINE_UTIL_HPP_

#include <glm/glm.hpp>

namespace munchkin {

struct BoundingBox {
    glm::vec2 top_left;
    glm::vec2 bottom_right;
};

// Y coordinates are going top to bottom for this function
inline bool inside_bounding_box(BoundingBox const& box, glm::vec2 pos) {
    return pos.x > box.top_left.x && pos.x < box.bottom_right.x 
           && pos.y > box.top_left.y && pos.y < box.bottom_right.y;
}

}

#endif