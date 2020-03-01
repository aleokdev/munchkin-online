#ifndef MUNCHKIN_DEBUG_TERMINAL_HPP__
#define MUNCHKIN_DEBUG_TERMINAL_HPP__

#include <sol/sol.hpp>
#include <string>
#include <vector>

namespace munchkin {

class Game;

namespace systems {

class DebugTerminal {
public:
    DebugTerminal(Game&);

    void render();
    void log(std::string);

private:
    void log_lua(std::string, sol::object);

    Game* game;
    std::vector<std::string> terminal_log;
};

} // namespace systems
} // namespace munchkin

#endif