#ifndef MUNCHKIN_CARDDEF_HPP_
#define MUNCHKIN_CARDDEF_HPP_

#include <vector>
#include <string>
#include <sol/sol.hpp>
#include <optional>

namespace munchkin
{
	enum class DeckType {
		null,
		dungeon,
		treasure
	};

	struct CardDef
	{
		CardDef(sol::state& state,
			std::string const& script_path,
			std::string name,
			std::string description,
			DeckType category,
			std::string front_texture_path,
			std::string back_texture_path
		);

		std::string name;
		std::string description;
		sol::table metatable;
		std::vector<std::string> play_stages;
		DeckType category;

		std::string front_texture_path;
		std::string back_texture_path;
	};
}

#endif