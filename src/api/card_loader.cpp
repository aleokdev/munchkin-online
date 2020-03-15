#include "api/card_loader.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace munchkin {

void parse_json_object(sol::table& t, nlohmann::json const& json) {
    for (auto& [k, v] : json.items()) {
        if (v.is_null())
            continue;
        else if (v.is_boolean())
            t[sol::create_if_nil][k] = (bool)v;
        else if (v.is_number())
            t[sol::create_if_nil][k] = (int)v;
        else if (v.is_object())
        {
            parse_json_object(t, v);
        }
        else if (v.is_array())
            throw std::runtime_error(
                "load_cards doesn't allow JSON arrays as card properties");
        else if (v.is_string())
            t[sol::create_if_nil][k] =
                (std::string)v;
    }
}

std::vector<CardDef> load_cards(fs::path path, sol::state& lua) {
    nlohmann::json j;
    std::ifstream f(path/"cards.json");
    f >> j;

    std::vector<CardDef> result;

    for (auto& card_json : j) {
        std::string script_path =
            (path / fs::path((std::string)card_json["script"])).generic_string();
        DeckType def_category(DeckType::null);
        if (!card_json.contains("category"))
            std::cerr << "Card has no category! Will default to null; This means that it won't be "
                         "introduced to the game decks and will have strange behaviour"
                      << std::endl;
        else if (card_json["category"] == "dungeon")
            def_category = DeckType::dungeon;
        else if (card_json["category"] == "treasure")
            def_category = DeckType::treasure;
        else
            std::cerr << "Card has unknown category! Will default to null; This means that it "
                         "won't be introduced to the game decks and will have strange behaviour"
                      << std::endl;

        // @todo: Do not hardcode default textures
        std::string front_texture_path =
            card_json.contains("front_texture")
                ? (path / fs::path((std::string)card_json["front_texture"]))
                      .generic_string()
                : (path / "textures" /
                   (def_category == DeckType::dungeon ? "dungeon-front.png" : "treasure-front.png"))
                      .generic_string();
        std::string back_texture_path =
            card_json.contains("back_texture")
                ? (path / fs::path((std::string)card_json["back_texture"]))
                      .generic_string()
                : (path / "textures" /
                   (def_category == DeckType::dungeon ? "dungeon-back.png" : "treasure-back.png"))
                      .generic_string();
        CardDef def(lua, script_path, card_json["name"], card_json["description"], def_category,
                    front_texture_path, back_texture_path);

        for (auto& play_stage : card_json["play_stages"]) def.play_stages.emplace_back(play_stage);

        sol::table t(def.metatable.lua_state(), sol::create);
        def.metatable["properties"] = t;
        if(card_json.contains("properties"))
            parse_json_object(t, card_json["properties"]);
        result.emplace_back(def);
    }

    return result;
}

} // namespace munchkin