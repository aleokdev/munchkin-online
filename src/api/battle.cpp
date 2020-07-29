#include "api/battle.hpp"

#include <iostream>

namespace munchkin {

int Battle::get_total_player_power() {
    int ret = source_player->get_power();

    for (auto& helper : helpers) { ret += helper->get_power(); }

    ret += player_power_offset;

    return ret;
}

int Battle::get_total_monster_power() {
    int ret = 0;
    for (auto& [card, power] : played_cards) { ret += power; }

    ret += monster_power_offset;

    return ret;
}

void Battle::add_card(CardPtr ptr) {
    ptr->move_to(Card::CardLocation::table_center);
    played_cards[ptr] = 0;
}

void Battle::remove_card(CardPtr ptr) { played_cards.erase(ptr); }

void Battle::modify_card(CardPtr ptr, int power) { played_cards[ptr] += power; }

int Battle::get_card_power(CardPtr ptr) { return played_cards[ptr]; }

std::vector<CardPtr> Battle::get_cards_played() {
    std::vector<CardPtr> ret;
    for (auto& [card, power] : played_cards) ret.emplace_back(card);
    return ret;
}

bool Battle::is_card_played(CardPtr card) {
    auto iter = played_cards.find(card);
    return iter != played_cards.end();
}
} // namespace munchkin