#include "systems/ai_manager.hpp"
#include "api/state.hpp"

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include <random>

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

namespace munchkin {

AIManager::AIManager(State& s, std::vector<PlayerPtr> _pl, std::string ai_path) : state(&s) {
    std::filesystem::path fspath(ai_path);
    fspath /= AI_MAIN_FILE_FILENAME;
    ai_thread = sol::thread::create(s.lua.lua_state());

    // Load AI names
    nlohmann::json json;
    std::ifstream(AI_NAMES_FILEPATH) >> json;

    for (std::string name : json) { ai_names.emplace_back(name); }

    std::random_device rnd;
    std::mt19937 gen(rnd());
    // Create player states and set their names
    for (auto& player : _pl) {
        players_controlled[player] = ai_thread.state().script_file(fspath.string());
        player->name = *select_randomly(ai_names.begin(), ai_names.end(), gen);
    }
}

size_t AIManager::get_total_players_controlled() { return players_controlled.size(); }

void AIManager::tick() {
    if (last_player_playing_id != state->current_player_id) {
        for (auto& [player, ai_table] : players_controlled) {
            if (player->get_id() == state->current_player_id) {
                sol::coroutine coro = ai_table["on_turn"];
                coro(player);
                state->active_coroutines.emplace_back(ai_table["on_turn"]);
            }
        }
        last_player_playing_id = state->current_player_id;
    }
}

} // namespace munchkin