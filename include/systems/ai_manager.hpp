#ifndef MUNCHKIN_AI_MANAGER_HPP__
#define MUNCHKIN_AI_MANAGER_HPP__

#include "api/player.hpp"
#include <optional>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#define AI_MAIN_FILE_FILENAME "main.lua"
#define AI_NAMES_FILEPATH "data/generic/ai_names.json"

namespace munchkin {
class State;

class AIManager {
public:
    AIManager(State&, std::vector<PlayerPtr> players_to_control, std::string ai_path);

    void tick();

    size_t get_total_players_controlled();

private:
    State* state;
    std::unordered_map<PlayerPtr, sol::table> players_controlled;
    sol::thread ai_thread;
    std::vector<std::string> ai_names;

    size_t last_player_playing_id = -1;
};

} // namespace munchkin

#endif