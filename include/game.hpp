#ifndef MUNCHKIN_GAME_HPP_
#define MUNCHKIN_GAME_HPP_

#include "api/gamerules.hpp"
#include "api/state.hpp"
#include "renderer/card_sprite.hpp"

#include "util/pos_vec.hpp"

namespace munchkin {

namespace renderer {
inline static constexpr float table_radius = 700;
}

class Game;
class Game {
public:
    Game(size_t player_count,
         size_t window_w,
         size_t window_h,
         std::string gamerules_path = DEFAULT_GAMERULES_PATH);
    Game& operator=(Game&&) = delete;
    Game(Game&&) = delete;

    void turn();

    void tick();

    void push_event(FlowEvent e);

    bool ended();

    State& get_state() { return state; }

    State state;
    GameRules gamerules;

    // windowing data
    size_t window_w, window_h;

    // Camera data
    struct Camera {
        Camera(float x, float y) : offset{x, y} {}
        // offsets are stored relative to center
        math::Vec2D offset;
    } camera;

    // Sprites
    std::vector<renderer::CardSprite> card_sprites;

    size_t local_player_id = 0;
};

} // namespace munchkin

#endif