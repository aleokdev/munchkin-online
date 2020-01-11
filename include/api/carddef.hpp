#ifndef MUNCHKIN_CARDDEF_HPP_
#define MUNCHKIN_CARDDEF_HPP_

#include <vector>
#include <string>
#include <sol/sol.hpp>

namespace munchkin
{
	enum class DeckType {
		null,
		dungeon,
		treasure
	};

	struct CardDef
	{
		CardDef(sol::state& state, std::string const& script_path, std::string _name, std::string _description, DeckType _category = DeckType::null);

		std::string name;
		std::string description;
		sol::table metatable;
		std::vector<std::string> play_stages;
		DeckType category;
	};
}

#endif