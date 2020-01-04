// carddata.cpp test -- Checks if card tables and metatables work
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "api/card.hpp"
#include "api/carddef.hpp"
#include "api/state.hpp"

#include <sol/sol.hpp>

#include <iostream>

using namespace munchkin;

TEST_CASE("card metatables") {
	// Player count shouldn't matter in this test
	State state(5);

	// Create a card definition from metatable.lua, that defines a single function, set_local_var.
	CardDef example_def(state.lua, "data/cardpacks/test/scripts/metatable.lua", "Metatable Test", "Tests metatables");

	Card card1(example_def);
	Card card2(example_def);

	REQUIRE(&card1.get_def() == &card2.get_def());
	REQUIRE((sol::object)example_def.metatable["set_local_var"] != sol::lua_nil);
	REQUIRE((sol::object)card1.get_data_variable("set_local_var") != sol::lua_nil);

	SUBCASE("using set_local_var in card1 sets a variable in card1")
	{
		// Should set card1.variable to true and return true
		bool result = card1.execute_function("set_local_var").as<bool>();

		CHECK(result);

		SUBCASE("using set_local_var in card1 does NOT set the metatable's variable")
		{
			CHECK((sol::object)example_def.metatable["variable"] == sol::lua_nil);
		}

		SUBCASE("using set_local_var in card1 does NOT set card2's variable")
		{
			CHECK(card2.get_data_variable("variable") == sol::lua_nil);
		}
	}
}

TEST_CASE("State::current_battle") {
	// Player count shouldn't matter in this test
	State state(5);

	CardDef def(state.lua, "data/cardpacks/test/scripts/battle.lua", "current_battle Test", "Tests the current battle variable");
	Card def_instance(def);

	CHECK(def_instance.execute_function("test_battle_1"));
	CHECK(def_instance.execute_function("test_battle_2"));
	CHECK(def_instance.execute_function("test_battle_3"));
}