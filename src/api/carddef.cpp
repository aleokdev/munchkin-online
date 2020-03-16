#include "api/carddef.hpp"

namespace munchkin {

CardDef::CardDef(sol::state& state,
                 std::string const& script_path,
                 std::string _name,
                 std::string _description,
                 DeckType _category,
                 std::string _front_texture_path,
                 std::string _back_texture_path,
                 bool _is_monster) :
    name(_name),
    description(_description), category(_category), front_texture_path(_front_texture_path),
    back_texture_path(_back_texture_path), is_monster(_is_monster) {
    metatable = state.script_file(script_path);
}
} // namespace munchkin
