#ifndef MUNCHKIN_CARDDEF_HPP_
#define MUNCHKIN_CARDDEF_HPP_

#include <string_view>
#include <sol/sol.hpp>

namespace munchkin
{
	struct CardDef
	{
		CardDef(sol::state& state, std::string_view script_path);

	private:
		std::string name;
		std::string description;
		sol::table function_defs;
	};
}

#endif