#ifndef MUNCHKIN_CARDDEF_HPP_
#define MUNCHKIN_CARDDEF_HPP_

#include <string>
#include <sol/sol.hpp>

namespace munchkin
{
	struct CardDef
	{
		CardDef(sol::state& state, std::string const& script_path, std::string _name, std::string _description);

		std::string name;
		std::string description;
		sol::table metatable;
	};
}

#endif