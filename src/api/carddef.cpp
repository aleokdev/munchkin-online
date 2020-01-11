#include "api/carddef.hpp"


namespace munchkin
{
	CardDef::CardDef(sol::state& state, std::string const& script_path, std::string _name, std::string _description, DeckType _category)
		: name(_name), description(_description), category(_category)
	{
		metatable = state.script_file(script_path);
	}
}
