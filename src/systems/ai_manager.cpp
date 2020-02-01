#include "systems/ai_manager.hpp"
#include "api/state.hpp"

#include <filesystem>

namespace munchkin {

AIManager::AIManager(State& s, std::vector<PlayerPtr> _pl, std::string ai_path) : state(&s) {
    std::filesystem::path fspath(ai_path);
    fspath /= AI_MAIN_FILE_FILENAME;
    ai_thread = sol::thread::create(s.lua.lua_state());
    for (auto& player : _pl) {
        players_controlled[player] = ai_thread.state().script_file(fspath.string());
    }
}

size_t AIManager::get_total_players_controlled() { return players_controlled.size(); }

void AIManager::tick() {
    if (last_player_playing_id != state->current_player_id) {
        for (auto& [player, ai_table] : players_controlled) {
            if (player->id == state->current_player_id) {
                sol::coroutine coro = ai_table["on_turn"];
                coro(player);
                state->active_coroutines.emplace_back(ai_table["on_turn"]);
            }
        }
        last_player_playing_id = state->current_player_id;
    }
}

} // namespace munchkin