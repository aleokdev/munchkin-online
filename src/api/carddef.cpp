#include "api/carddef.hpp"


namespace munchkin
{

CardDef::CardDef(sol::state& state, std::string const& script_path)
{
	function_defs = state.script_file(script_path);
}

}
