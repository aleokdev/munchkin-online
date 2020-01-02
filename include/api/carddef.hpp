#ifndef MUNCHKIN_CARDDEF_HPP_
#define MUNCHKIN_CARDDEF_HPP_

#include <string_view>
#include <sol/sol.hpp>

namespace munchkin
{
	struct CardDef
	{
		CardDef(sol::state& state, std::string_view script_path);

		sol::object execute_function(std::string_view name);

	private:
		std::string name;
		std::string description;
		sol::table function_defs;
	};
}

#endif