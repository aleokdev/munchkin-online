#include "systems/debug_terminal.hpp"
#include "game.hpp"
#include "game_wrapper.hpp"
#include <imgui.h>
#include <lua.hpp>

#include <iostream>

namespace munchkin {
namespace systems {

DebugTerminal::DebugTerminal(GameWrapper& g) : wrapper(&g) {
    wrapper->game.state.lua.set_function("internal_print", &DebugTerminal::log_lua, this);
}

void DebugTerminal::log_lua(std::string func_name, sol::object obj) {
    // @todo: Make log_lua also log the print caller
    std::cout << "[" << func_name << "] " << obj.as<std::string>() << std::endl;
    terminal_log.emplace_back("["+func_name+"] "+obj.as<std::string>());
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