#include "api/battle.hpp"

namespace munchkin {

	int Battle::get_total_player_power()
	{
		int ret = source_player->get_power();

		for (auto& helper : helpers)
		{
			ret += helper->get_power();
		}

		ret += player_power_offset;

		return ret;
	}

	int Battle::get_total_monster_power()
	{
		int ret;
		for (auto& [card, power] : played_cards)
		{
			ret += power;
		}

		ret += monster_power_offset;

		return ret;
	}

}