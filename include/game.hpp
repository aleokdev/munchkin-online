#ifndef MUNCHKIN_GAME_HPP_
#define MUNCHKIN_GAME_HPP_

#include "api/gamerules.hpp"
#include "api/state.hpp"
#include "renderer/card_sprite.hpp"

#include "util/pos_vec.hpp"
#include <functional>

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

    using YieldReturnFunction =
        std::function<void(sol::coroutine const&, sol::protected_function_result const&)>;
    // Binds a yield result string (i.e. coroutine.yield("do_thing", ...)) with a callback, which
    // must accept the coroutine that called it and the yield parameters (as a
    // protected_function_result).
    void bind_yield_result(std::string const& key, YieldReturnFunction callback);

    State& get_state() { return state; }

    State state;
    GameRules gamerules;

    // windowing data
    size_t window_w, window_h;

    // Camera data
    class Camera {
    public:
        Camera(Game const& game, float x, float y) : game(&game), offset{x, y} {}
        // offsets are stored relative to center
        math::Vec2D offset;

        math::Vec2D pixel_world_to_screen(math::Vec2D pos) const {
            const auto window_w = (float)game->window_w;
            const auto window_h = (float)game->window_h;
            return pos - offset * math::Vec2D{window_w, -window_h} / 2.f + math::Vec2D{0, window_h};
        }

    private:
        const Game* const game;
    } camera;

    // Sprites
    std::vector<renderer::CardSprite> card_sprites;
    renderer::CardSprite* current_hovered_sprite = nullptr;

    size_t local_player_id = 0;

    std::unordered_map<std::string, YieldReturnFunction> yield_result_map;
};

} // namespace munchkin

#endif