#include "api/card.hpp"

#include "api/player.hpp"
#include "api/state.hpp"
#include <iostream>

namespace munchkin {

	Card::Card(CardDef& def) : def(&def), data(sol::state_view(def.metatable.lua_state()).create_table()) { 
		// TODO: Replace with metatables (HOW?)
		for (auto& [key, val] : def.metatable)
		{
			data[key] = val;
		}
	}

}