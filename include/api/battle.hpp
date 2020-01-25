#ifndef MUNCHKIN_BATTLE_HPP__
#define MUNCHKIN_BATTLE_HPP__

#include <vector>
#include <unordered_map>
#include "player.hpp"
#include "card.hpp"

namespace munchkin {

struct Battle {
	Battle(State& s, Player& source) : state(&s), source_player(&source) {}
	Battle() : state(nullptr), source_player(nullptr) {}

	State* state;
	Player* source_player;
	std::vector<Player*> helpers;
	// The offset for the total players' power. Can be negative.
	int player_power_offset;
	int get_total_player_power();

	// Map where Card is the played card and size_t is the (monster) power of that card. Needed because we need
	// to keep track of each card's individual power.
	std::unordered_map<CardPtr, int> played_cards;

	// API Functions
	// TODO: Change to CardPtr instead of ID
	void add_card(int id);
	void remove_card(int id);
	void modify_card(int id, int power);
	int get_card_power(CardPtr);
	std::vector<CardPtr> get_cards_played();

	// The offset for the total monsters' power. Can be negative.
	int monster_power_offset;
	int get_total_monster_power();
};

}

#endif