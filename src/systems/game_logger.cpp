#include "systems/game_logger.hpp"
#include "game_wrapper.hpp"
#include "game.hpp"

#include <imgui.h>

namespace munchkin {
namespace systems {

GameLogger::GameLogger(GameWrapper& g) : wrapper(&g) {
    /* clang-format off */
    wrapper->game.state.lua.new_usertype<GameLogger>("game_logger",
        "log", &GameLogger::log);
    /* clang-format on */

    wrapper->game.state.lua["logger"] = this;
}

void GameLogger::log(std::string&& str) { logs.emplace_back(std::forward<std::string>(str)); }

void GameLogger::render() {
    ImGui::SetNextWindowSize(ImVec2{400, window_height}, ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(ImVec2{0, 0});
    if (!ImGui::Begin("Game Logger", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        // Window is minimized
        ImGui::End();
        return;
    }

    ImGui::BeginChildFrame(ImGui::GetID("_Game Logger"), ImGui::GetContentRegionAvail(),
                           ImGuiWindowFlags_AlwaysAutoResize);

    for (auto& str : logs) ImGui::TextUnformatted(str.c_str());

    ImGui::EndChildFrame();
    ImGui::End();
}

} // namespace systems
} // namespace munchkin