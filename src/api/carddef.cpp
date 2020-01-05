#include "api/carddef.hpp"


namespace munchkin
{
	CardDef::CardDef(sol::state& state, std::string const& script_path, std::string _name, std::string _description)
		: name(_name), description(_description)
	{
		metatable = state.script_file(script_path);
	}
}
