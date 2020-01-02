#ifndef MUNCHKIN_CARDDEF_HPP_
#define MUNCHKIN_CARDDEF_HPP_

#include <string>
#include <sol/sol.hpp>

namespace munchkin
{
	struct CardDef
	{
		CardDef(sol::state& state, std::string const& script_path);

		template<typename... Args>
		sol::object execute_function(std::string_view name, Args&&... args) {
			return function_defs[name](std::forward<Args>(args)...);
		}

		std::string name;
		std::string description;
		sol::table function_defs;
	};
}

#endif