#include "systems/ai_manager.hpp"
#include "api/state.hpp"

#include <filesystem>

namespace munchkin {

AIManager::AIManager(State& s, std::vector<PlayerPtr> _pl, std::string ai_path) : state(&s) {
    std::filesystem::path fspath(ai_path);
    fspath /= AI_MAIN_FILE_FILENAME;
    for (auto& player : _pl) { players_controlled[player] = s.lua.script_file(fspath.string()); }
}

void AIManager::tick() {
    if (last_player_playing_id != state->current_player_id) {
        for (auto& [player, ai_table] : players_controlled) {
            if (player->id == state->current_player_id) { /*
                 sol::coroutine coro = ;
                 coro(player);*/
                sol::coroutine coro = ai_table["on_turn"];
                for (int i = 0; i < 10;i++) { coro(); }
                //state->active_coroutines.emplace_back(ai_table["on_turn"]);
            }
        }
        last_player_playing_id = state->current_player_id;
    }
}

} // namespace munchkin