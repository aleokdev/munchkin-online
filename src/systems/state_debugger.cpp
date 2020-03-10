#include "systems/state_debugger.hpp"
#include "api/state.hpp"
#include "game.hpp"
#include "imgui.h"
#include <array>

void drawLuaStateInspectorTable(lua_State* state) {
    ImGui::Indent();
    lua_pushnil(state);
    while (lua_next(state, -2) != 0) { // key(-1) is replaced by the next key(-1) in table(-2)
        /* uses 'key' (at index -2) and 'value' (at index -1) */

        const char* keyName;
        const char* valueData;

        // Duplicate the values before converting them to string because tolstring can affect the
        // actual data in the stack
        {
            lua_pushvalue(state, -2);
            keyName = lua_tostring(state, -1);
            lua_pop(state, 1);
        }

        {
            lua_pushvalue(state, -1);
            valueData = lua_tostring(state, -1);

            std::string final_string;
            if (valueData == nullptr) {
                // lua_tolstring returns NULL if the value is not a number or a string, so let's get
                // the address instead.
                const void* address = lua_topointer(state, -1);
                std::ostringstream addressStr;
                addressStr << address;

                final_string.append("<");
                if (lua_iscfunction(state, -1))
                    final_string.append("C Function");
                else if (lua_isthread(state, -1))
                    final_string.append("Thread");
                else if (lua_isuserdata(state, -1))
                    final_string.append("Userdata");
                final_string.append(" @ 0x");
                final_string.append(addressStr.str());
                final_string.append(">");
                valueData = final_string.c_str();
            }

            lua_pop(state, 1); // Remove duplicate

            if (lua_istable(state, -1)) {
                if (ImGui::CollapsingHeader(keyName))
                    drawLuaStateInspectorTable(state);
                else {
                    /* removes 'value'; keeps 'key' for next iteration */
                    lua_pop(state, 1); // remove value(-1), now key on top at(-1)
                }
            } else {
                ImGui::Text("%s - %s", keyName, valueData);

                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(state, 1); // remove value(-1), now key on top at(-1)
            }
        }
    }
    lua_pop(state, 1); // remove starting table val
    ImGui::Unindent();
}

void DrawLuaStateInspector(lua_State* state, bool* p_open) {
    if (!ImGui::Begin("State Inspector", p_open, 0)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("GLOBALS")) {
        lua_pushglobaltable(state);
        drawLuaStateInspectorTable(state);
    }

    ImGui::End();
}

namespace munchkin {
namespace systems {

StateDebugger::StateDebugger(Game& g) : game(&g) {}

void StateDebugger::render() {
    ImGui::Begin("Debug");
    {
        if (ImGui::Button("Open Lua State Viewer"))
            show_stateviewer = true;
        if (show_stateviewer)
            DrawLuaStateInspector(game->state.lua, &show_stateviewer);

        if (ImGui::Button("Open ImGui Demo window"))
            show_demo = true;
        if (show_demo)
            ImGui::ShowDemoWindow(&show_demo);

        constexpr const char* events[] = {"tick", "clicked_dungeon_deck", "card_discarded",
                                          "card_played"};
        if (ImGui::BeginCombo("##event_to_push", events[current_selected_event])) {
            for (int i = 0; i < 4; i++) {
                bool is_selected = i == current_selected_event;
                if (ImGui::Selectable(events[i], is_selected))
                    current_selected_event = i;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Push Event"))
            game->state.event_queue.push({(FlowEvent::EventType)current_selected_event});

        ImGui::Separator();

        ImGui::TextUnformatted("Players");
        ImGui::SameLine();
        ImGui::TextDisabled("(%zu)", game->state.players.size());
        ImGui::Columns(5);
        ImGui::TextUnformatted("ID");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Name");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Level");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Hand Cards");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Hand Max Cards");
        ImGui::NextColumn();

        for (auto& player : game->state.players) {
            ImGui::Text("%zu", player.get_id());
            ImGui::NextColumn();
            ImGui::Text("%s", player.name.c_str());
            ImGui::NextColumn();
            ImGui::Text("%d", player.level);
            ImGui::NextColumn();
            ImGui::Text("%zu", player.hand.size());
            ImGui::SameLine();
            char buf[10];
            std::sprintf(buf, "Show##%zu", player.get_id());
            if (ImGui::Button(buf))
                player_cards_to_show = &player;
            ImGui::NextColumn();
            ImGui::Text("%zu", player.hand_max_cards);
            ImGui::NextColumn();
        }
        ImGui::Columns(1);

        ImGui::Separator();

        ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
        if (ImGui::TreeNode("Card Decks")) {
            ImGui::Text("Cards in dungeon deck: %zu", game->state.dungeon_deck.size());
            ImGui::Text("Cards in dungeon discard deck: %zu",
                        game->state.dungeon_discard_deck.size());
            ImGui::Text("Cards in treasure deck: %zu", game->state.treasure_deck.size());
            ImGui::Text("Cards in treasure discard deck: %zu",
                        game->state.treasure_discard_deck.size());
            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
        if (ImGui::TreeNode("State information")) {
            ImGui::Text("Current tick: %zu", game->state.tick);
            ImGui::Text("Turn number: %zu", game->state.turn_number);
            ImGui::Text("Current user playing: %zu", game->state.current_player_id);
            ImGui::Text("Number of active coroutines: %zu", game->state.active_coroutines.size());
            if (ImGui::TreeNode("Active coroutines")) {
                int i = 0;
                for (auto& coro : game->state.active_coroutines) {
                    const char* status_txt;
                    switch (coro.status()) {
                        case (sol::call_status::ok): status_txt = "ok"; break;
                        case (sol::call_status::file): status_txt = "file"; break;
                        case (sol::call_status::gc): status_txt = "gc"; break;
                        case (sol::call_status::handler): status_txt = "handler"; break;
                        case (sol::call_status::memory): status_txt = "memory"; break;
                        case (sol::call_status::runtime): status_txt = "runtime"; break;
                        case (sol::call_status::yielded): status_txt = "yielded"; break;
                        case (sol::call_status::syntax): status_txt = "syntax"; break;
                    }
                    ImGui::Text("[%i] Status: %s", i, status_txt);
                    i++;
                }
                ImGui::TreePop();
            }
            ImGui::Text("Should borrow facing up: %s",
                        game->state.should_borrow_facing_up ? "true" : "false");
            ImGui::Text("Game stage: %s", game->state.get_game_stage().c_str());

            ImGui::Text("Battle active: %s", game->state.current_battle ? "true" : "false");
            if (game->state.current_battle && ImGui::TreeNode("Current Battle")) {
                ImGui::Text("Number of cards played: %zu",
                            game->state.current_battle->played_cards.size());
                ImGui::Text("Total player power: %i",
                            game->state.current_battle->get_total_player_power());
                ImGui::Text("Total monster power: %i",
                            game->state.current_battle->get_total_monster_power());
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
        if (ImGui::TreeNode("Game information")) {
            ImGui::Text("Number of sprites: %zu", game->card_sprites.size());
            ImGui::Text("Local Player ID: %zu", game->local_player_id);
            ImGui::TreePop();
        }
    }
    ImGui::End();

    if (player_cards_to_show) {
        ImGui::Begin("Hand Cards");
        for (auto& card : player_cards_to_show->hand) {
            ImGui::TextUnformatted(card->get_def().name.c_str());
        }
        ImGui::End();
    }
}

} // namespace systems
} // namespace munchkin