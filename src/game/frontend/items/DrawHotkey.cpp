#include "DrawHotkey.hpp"
#include "core/i18n/Language.hpp"

namespace YimMenu
{
	void DrawHotkey(CommandLink* link, std::string_view label)
	{
		ImGui::PushID(link);

		ImGui::Button(label.data());

		bool active = ImGui::IsItemActive();

		if (active)
		{
			HotkeySystem::SetBeingModifed(true);
			g_HotkeySystem.CreateHotkey(link->m_Chain);
		}

		ImGui::SameLine(250);
		ImGui::BeginGroup();

		if (link->m_Chain.empty())
		{
			if (active)
				ImGui::Text("%s", L("hotkey.press_any_button", "Press any button...").c_str());
			else
				ImGui::Text("%s", L("hotkey.no_hotkey", "No hotkey assigned").c_str());
		}
		else
		{
			ImGui::PushItemWidth(35);
			int i = 0;
			for (auto key : link->m_Chain)
			{
				char key_label[32];
				strcpy(key_label, g_HotkeySystem.GetHotkeyLabel(key).data());

				ImGui::PushID(i);
				ImGui::InputText("##keylabel", key_label, 32, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::IsItemClicked())
				{
					std::erase_if(link->m_Chain, [key](int j) {
						return j == key;
					});
					g_HotkeySystem.MarkStateDirty();
				}
				ImGui::PopID();

				i++;
				ImGui::SameLine();
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();
			if (ImGui::Button(L("btn.clear", "Clear").c_str()))
			{
				link->m_Chain.clear();
				g_HotkeySystem.MarkStateDirty();
			}
		}

		ImGui::EndGroup();

		ImGui::Spacing();

		ImGui::PopID();
	}
}