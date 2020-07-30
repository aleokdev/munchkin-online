#ifndef MUNCHKIN_GAME_LOGGER_HPP__
#define MUNCHKIN_GAME_LOGGER_HPP__

#include <string>
#include <vector>

namespace munchkin {

class GameWrapper;

namespace systems {

class [[system_codegen::wrapper_instance("logger")]] GameLogger {
public:
    GameLogger(GameWrapper&);
    
    void load_content() {}

    void log(std::string&&);

    void render();

private:
    GameWrapper* wrapper;
    std::vector<std::string> logs;

    inline static constexpr float window_height = 200;
};

} // namespace systems
} // namespace munchkin

#endif