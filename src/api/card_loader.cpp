#include "api/card_loader.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <iostream>

namespace munchkin {

std::vector<CardDef> load_cards(std::string_view path, sol::state& lua) {
    nlohmann::json j;
    std::ifstream f(path.data());
    f >> j;

    std::vector<CardDef> result;
    std::filesystem::path basepath(path);
    basepath = basepath.parent_path();

    for (auto& card_json : j) {
       std::string script_path = (basepath/std::filesystem::path((std::string)card_json["script"])).generic_string();
       DeckType def_category(DeckType::null);
       if (card_json["category"] == "dungeon")
           def_category = DeckType::dungeon;
       else if (card_json["category"] == "treasure")
           def_category = DeckType::treasure;
       else
           std::cerr << "Card has no category! Will default to null; This means that it won't be introduced to the game decks" << std::endl;
       CardDef def(lua, script_path, card_json["name"], card_json["description"], def_category);
       for (auto& play_stage : card_json["play_stages"])
           def.play_stages.emplace_back(play_stage);

       for (auto& [k, v] : card_json["properties"].items())
       {
           if (v.is_null()) continue;
           else if (v.is_boolean()) def.metatable[sol::create_if_nil]["properties"][k] = (bool)v;
           else if (v.is_number()) def.metatable[sol::create_if_nil]["properties"][k] = (int)v;
           else if (v.is_object()) throw std::runtime_error("load_cards doesn't allow JSON objects or arrays as card properties");
           else if (v.is_array()) throw std::runtime_error("load_cards doesn't allow JSON objects or arrays as card properties");
           else if (v.is_string()) def.metatable[sol::create_if_nil]["properties"][k] = (std::string)v;
       }
       result.emplace_back(def);
    }

    return result;
}

}