#include "api/card.hpp"

#include "api/player.hpp"
#include "api/state.hpp"
#include <iostream>

namespace munchkin {

	Card::Card(State& st, CardDef& def) : state(st), def(&def), data(sol::state_view(def.metatable.lua_state()).create_table()) { 
		id = last_id++;
		// TODO: Replace with metatables (HOW?)
		for (auto& [key, val] : def.metatable)
		{
			data[key] = val;
		}
	}

	CardPtr::operator Card* () const
	{
		Card* retval = nullptr;
		for (auto& card : state.all_cards) {
			if (card.get_id() == card_id) {
				retval = &card;
				break;
			}
		}
		return retval;
	}

	Card* CardPtr::operator->() const
	{
		return *this;
	}

}