#include "systems/debug_terminal.hpp"
#include "game.hpp"
#include <imgui.h>
#include <lua.hpp>

#include <iostream>

namespace munchkin {
namespace systems {

DebugTerminal::DebugTerminal(Game& g) : game(&g) {
    g.state.lua.set_function("print", &DebugTerminal::log_lua, this);
}

void DebugTerminal::log_lua(sol::this_state state, sol::object obj) {
    // @todo: Make log_lua also log the print caller
    std::cout << obj.as<std::string>() << std::endl; 
    terminal_log.emplace_back(obj.as<std::string>());
}

void DebugTerminal::render() {
    ImGui::SetNextWindowSize(ImVec2{400, 200}, ImGuiCond_Once);
    ImGui::Begin("Debug Terminal");

    ImGui::BeginChildFrame(ImGui::GetID("debug_frame"), ImGui::GetContentRegionAvail());
    ImGui::SetScrollY(ImGui::GetScrollMaxY());
    for (auto& str : terminal_log) ImGui::TextUnformatted(str.c_str());
            ImGui::EndChildFrame();

            ImGui::End();
    }

} // namespace systems
} // namespace systems