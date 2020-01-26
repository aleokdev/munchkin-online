#include "api/card_loader.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace munchkin {

std::vector<CardDef> load_cards(std::string_view path, sol::state& lua) {
    nlohmann::json j;
    std::ifstream f(path.data());
    f >> j;

    std::vector<CardDef> result;
    std::filesystem::path basepath(path);
    basepath = basepath.parent_path();

    for (auto& card_json : j) {
        std::string script_path =
            (basepath / std::filesystem::path((std::string)card_json["script"])).generic_string();
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
                ? (basepath / std::filesystem::path((std::string)card_json["front_texture"]))
                      .generic_string()
                : (basepath / "textures" /
                   (def_category == DeckType::dungeon ? "dungeon-front.png" : "treasure-front.png"))
                      .generic_string();
        std::string back_texture_path =
            card_json.contains("back_texture")
                ? (basepath / std::filesystem::path((std::string)card_json["back_texture"]))
                      .generic_string()
                : (basepath / "textures" /
                   (def_category == DeckType::dungeon ? "dungeon-back.png" : "treasure-back.png"))
                      .generic_string();
        CardDef def(lua, script_path, card_json["name"], card_json["description"], def_category,
                    front_texture_path, back_texture_path);

        for (auto& play_stage : card_json["play_stages"]) def.play_stages.emplace_back(play_stage);

        for (auto& [k, v] : card_json["properties"].items()) {
            if (v.is_null())
                continue;
            else if (v.is_boolean())
                def.metatable[sol::create_if_nil]["properties"][k] = (bool)v;
            else if (v.is_number())
                def.metatable[sol::create_if_nil]["properties"][k] = (int)v;
            else if (v.is_object())
                throw std::runtime_error(
                    "load_cards doesn't allow JSON objects or arrays as card properties");
            else if (v.is_array())
                throw std::runtime_error(
                    "load_cards doesn't allow JSON objects or arrays as card properties");
            else if (v.is_string())
                def.metatable[sol::create_if_nil]["properties"][k] =
                    (std::string)v;
        }
        result.emplace_back(def);
    }

    return result;
}

} // namespace munchkin