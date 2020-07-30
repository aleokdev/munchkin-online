#ifndef MUNCHKIN_DEBUG_TERMINAL_HPP__
#define MUNCHKIN_DEBUG_TERMINAL_HPP__

#include <sol/sol.hpp>
#include <string>
#include <vector>

namespace munchkin {

class GameWrapper;

namespace systems {

class  [[system_codegen::wrapper_instance("debug_terminal")]] DebugTerminal {
public:
    DebugTerminal(GameWrapper&);

    void load_content() {}

    void render();
    void log(std::string);

private:
    void log_lua(std::string, sol::object);

    GameWrapper* wrapper;
    std::vector<std::string> terminal_log;
};

} // namespace systems
} // namespace munchkin

#endif