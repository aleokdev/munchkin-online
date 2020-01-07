#include "renderer/state_debugger.hpp"
#include "api/state.hpp"
#include "imgui.h"

namespace munchkin {

	StateDebugger::StateDebugger(State& s) : state(&s) {

	}

	void StateDebugger::render() {
		ImGui::Begin("Debug");
		{
			ImGui::Button("Open Lua State Viewer");

			if (ImGui::Button("Open ImGui Demo window"))
				show_demo = true;
			if (show_demo)
				ImGui::ShowDemoWindow(&show_demo);

			ImGui::InputTextWithHint("##ev", "Input event...", event_name, 64);
			ImGui::SameLine();
			if (ImGui::Button("Push Event"))
			{
				char cpy_str[64];
				strcpy_s(cpy_str, 64, event_name);
				state->event_queue.push({ std::string(cpy_str) });
			}

			ImGui::Separator();

			ImGui::TextUnformatted("Players");
			ImGui::SameLine();
			ImGui::TextDisabled("(%d)", state->players.size());
			ImGui::Columns(4);
			ImGui::TextUnformatted("ID");
			ImGui::NextColumn();
			ImGui::TextUnformatted("Level");
			ImGui::NextColumn();
			ImGui::TextUnformatted("Hand Cards");
			ImGui::NextColumn();
			ImGui::TextUnformatted("Hand Max Cards");
			ImGui::NextColumn();

			for (auto& player : state->players)
			{
				ImGui::Text("%d", player.id);
				ImGui::NextColumn();
				ImGui::Text("%d", player.level);
				ImGui::NextColumn();
				ImGui::Text("%d", player.hand.size());
				ImGui::NextColumn();
				ImGui::Text("%d", player.hand_max_cards);
				ImGui::NextColumn();
			}
			ImGui::Columns(1);

			ImGui::Separator();

			ImGui::TextUnformatted("Game information");
			ImGui::Indent();
			{
				ImGui::Text("Turn number: %d", state->turn_number);
				ImGui::Text("Current user playing: %d", state->current_player_id);
				ImGui::Text("Number of active coroutines: %d", state->active_coroutines.size());
				ImGui::Text("Should borrow facing up: %s", state->should_borrow_facing_up ? "true" : "false");
				ImGui::Text("Game stage: %s", state->game_stage.c_str());
			}
			ImGui::Unindent();
		}
		ImGui::End();
	}

}