#ifndef MUNCHKIN_BATTLE_HPP__
#define MUNCHKIN_BATTLE_HPP__

#include <vector>
#include <unordered_map>
#include "player.hpp"

namespace munchkin {

struct Battle {
	Battle(Player& source) : source_player(&source) {}
	Battle() : source_player(nullptr) {}

	Player* source_player;
	std::vector<Player*> helpers;
	// The offset for the total players' power. Can be negative.
	int player_power_offset;
	int get_total_player_power();

	// Map where Card is the played card and size_t is the (monster) power of that card. Needed because we need
	// to keep track of each card's individual power.
	std::unordered_map<Card*, int> played_cards;
	// The offset for the total monsters' power. Can be negative.
	int monster_power_offset;
	int get_total_monster_power();
};

}

#endif