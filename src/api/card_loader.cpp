#include "api/card_loader.hpp"

#include <nlohmann/json.hpp>

namespace munchkin {

std::vector<CardDef> load_cards(std::string_view path, sol::state& lua) {
    nlohmann::json j;
}

}