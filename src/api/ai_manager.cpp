#include "api/ai_manager.hpp"

namespace munchkin {
namespace games {

AIManager::AIManager(State& s, std::vector<size_t> players_to_control) : state(&s) {
	for (size_t id : players_to_control) {
		ais.emplace_back(AIPlayer(s, id));
	}
}

void AIManager::tick() {
	for (auto& ai : ais) {
		ai.tick();
	}
}

}
}