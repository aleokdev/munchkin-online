#include "api/card_loader.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

namespace munchkin {

std::vector<CardDef> load_cards(std::string_view path, sol::state& lua) {
    nlohmann::json j;
    std::ifstream f(path.data());
    f >> j;

    std::vector<CardDef> result;

    for (auto const& card : j) {
       std::string script_path = j["script"];
       CardDef def(lua, script_path, j["name"], j["description"]);
       def.metatable["properties"] = sol::as_table(j["properties"]);
       result.emplace_back(def);
    }

    return result;
}

}