#include "api/card.hpp"

#include "api/player.hpp"
#include "api/state.hpp"
#include <iostream>

namespace munchkin {

	Card::Card(State& st, CardDef& def, ConstructorKey) : state(&st), def(&def), data(sol::state_view(def.metatable.lua_state()).create_table()) {
		id = st.generate_id();

		// TODO: Replace with metatables (HOW?)
		for (auto& [key, val] : def.metatable)
		{
			data[key] = val;
		}

		data["id"] = id;
	}

	CardPtr::CardPtr(Card& card) : state(&card.get_state()), card_id(card.get_id()) {}

	CardPtr::CardPtr(State& _state, size_t cardID) : state(&_state), card_id(cardID) {}

	CardPtr::operator Card* () const
	{
		Card* retval = nullptr;
		for (auto& card : state->all_cards) {
			if (card.get_id() == card_id) {
				retval = std::addressof(card);
				break;
			}
		}
		return retval;
	}

	Card* CardPtr::operator->() const
	{
		return *this;
	}

	CardPtr Card::operator&()
	{
		return CardPtr(*this);
	}

}