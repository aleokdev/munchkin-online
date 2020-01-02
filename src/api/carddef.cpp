#include "api/carddef.hpp"
#include "..\..\include\api\carddef.hpp"


namespace munchkin
{
	CardDef::CardDef(sol::state& state, std::string_view script_path, std::string _name, std::string _description)
		: name(_name), description(_description)
	{
		function_defs = state.script_file(script_path);
	}

	sol::object CardDef::execute_function(std::string_view name)
	{
		return function_defs[name]();
	}
}
