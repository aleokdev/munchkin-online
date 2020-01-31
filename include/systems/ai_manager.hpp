#ifndef MUNCHKIN_AI_MANAGER_HPP__
#define MUNCHKIN_AI_MANAGER_HPP__

#include "api/player.hpp"
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>

#define AI_MAIN_FILE_FILENAME "main.lua"

namespace munchkin {
class State;

class AIManager {
public:
    AIManager(State&, std::vector<PlayerPtr> players_to_control, std::string ai_path);

    void tick();

private:
    State* state;
    std::unordered_map<PlayerPtr, sol::table> players_controlled;

    size_t last_player_playing_id = -1;
};

} // namespace munchkin

#endif