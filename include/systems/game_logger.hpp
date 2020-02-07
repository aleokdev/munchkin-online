#ifndef MUNCHKIN_GAME_LOGGER_HPP__
#define MUNCHKIN_GAME_LOGGER_HPP__

#include <string>
#include <vector>

namespace munchkin {

class Game;

namespace systems {

class GameLogger {
public:
    GameLogger(Game&);

    void log(std::string&&);

    void render();

private:
    Game* game;
    std::vector<std::string> logs;

    inline static constexpr float window_height = 200;
};

} // namespace systems
} // namespace munchkin

#endif