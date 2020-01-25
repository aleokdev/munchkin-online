#include "api/battle.hpp"

#include <iostream>

namespace munchkin {

	int Battle::get_total_player_power() {
		int ret = source_player->get_power();

		for (auto& helper : helpers)
		{
			ret += helper->get_power();
		}

		ret += player_power_offset;

		return ret;
	}

	int Battle::get_total_monster_power() {
		int ret = 0;
		for (auto& [card, power] : played_cards)
		{
			ret += power;
		}

		ret += monster_power_offset;

		return ret;
	}

	void Battle::add_card(int id) {
		CardPtr(*state, id)->move_to(Card::CardLocation::table_center);
		played_cards[CardPtr(*state, id)] = 0;
	}

	void Battle::remove_card(int id) {
		played_cards.erase(CardPtr(*state, id));
	}

	void Battle::modify_card(int id, int power) {
		played_cards[CardPtr(*state, id)] += power;
	}
	int Battle::get_card_power(CardPtr cardptr)
	{
		return played_cards[cardptr];
	}
	std::vector<CardPtr> Battle::get_cards_played()
	{
		std::vector<CardPtr> ret;
		for (auto& [card, power] : played_cards)
			ret.emplace_back(card);
		return ret;
	}
}