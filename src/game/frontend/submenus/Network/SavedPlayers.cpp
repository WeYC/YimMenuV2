#include "SavedPlayers.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/frontend/widgets/imgui_colors.h"
#include "core/frontend/Notifications.hpp"
#include "core/i18n/Language.hpp"
#include "game/backend/SavedPlayers.hpp"
#include "game/gta/Network.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Submenus
{
	static std::uint64_t g_SelectedRid = 0;
	static SavedPlayerData* g_SelectedPlayer = nullptr;
	static char g_SelectedPlayerName[24]{};
	static char g_NameToSearch[24]{};

	static ImColor GetStatusCircleColor(SavedPlayerData* data)
	{
		if (!data->m_FetchedData)
			return ImGui::Colors::Gray;
		else if (data->m_FetchedData->m_GameState == FetchedPlayerData::GameState::INVALID)
			return ImGui::Colors::Red;
		else if (data->m_FetchedData->m_GameState == FetchedPlayerData::GameState::PUBLIC)
			return ImGui::Colors::Green;
		else
			return ImGui::Colors::Yellow;
	}

	static bool ShouldRenderPlayer(std::string_view name, std::string_view search)
	{
		if (search.empty())
			return true;

		if (name.size() < search.size())
			return false;

		// TODO: this doesn't do what YimMenuV1 did (which is to lowercase both inputs and perform a substring search)
		for (int i = 0; i < search.size(); i++)
			if (tolower(name[i]) != tolower(search[i]))
				return false;

		return true;
	}

	static void RenderPlayerItem(std::uint64_t rid, SavedPlayerData* data)
	{
		ImGui::PushID(rid);

		constexpr float circle_size = 7.5f;
		auto cursor_pos = ImGui::GetCursorScreenPos();

		// render status circle
		ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(cursor_pos.x + 4.f + circle_size, cursor_pos.y + 4.f + circle_size), circle_size, GetStatusCircleColor(data));

		// we need some padding
		ImVec2 cursor = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(cursor.x + 25.f, cursor.y));

		if (ImGui::Selectable(data->m_Name.data(), rid == g_SelectedRid))
		{
			g_SelectedRid = rid;
			g_SelectedPlayer = data;
			strncpy(g_SelectedPlayerName, data->m_Name.data(), sizeof(g_SelectedPlayerName));
		}

		if (data->m_FetchedData && ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", FetchedPlayerData::GameStateToString(data->m_FetchedData->m_GameState).data());

		ImGui::PopID();
	}

	static void RenderPlayerList()
	{
		ImGui::SetNextItemWidth(200.f);
		ImGui::InputTextWithHint("##search", L("ui.search", "Search").c_str(), g_NameToSearch, sizeof(g_NameToSearch));

		if (ImGui::BeginListBox("###player-list", {180, -100 /* static_cast<float>(*Pointers.ScreenResY - 700 - 38 * 4) */}))
		{
			auto& players = SavedPlayers::GetSavedPlayers();

			if (players.size() == 0)
			{
				ImGui::TextDisabled(L("saved_players.no_players", "No saved players").c_str());
				ImGui::EndListBox();
				return;
			}

			// TODO: surely there must be a better way to do this

			for (auto& [rid, data] : players)
				if (data.m_FetchedData && data.m_FetchedData->m_GameState == FetchedPlayerData::GameState::PUBLIC && ShouldRenderPlayer(data.m_Name, g_NameToSearch))
					RenderPlayerItem(rid, &data);

			for (auto& [rid, data] : players)
				if (data.m_FetchedData && data.m_FetchedData->m_GameState != FetchedPlayerData::GameState::PUBLIC
				    && data.m_FetchedData->m_GameState != FetchedPlayerData::GameState::INVALID && ShouldRenderPlayer(data.m_Name, g_NameToSearch))
					RenderPlayerItem(rid, &data);

			for (auto& [rid, data] : players)
				if ((!data.m_FetchedData || (data.m_FetchedData->m_GameState == FetchedPlayerData::GameState::INVALID)) && ShouldRenderPlayer(data.m_Name, g_NameToSearch))
					RenderPlayerItem(rid, &data);


			ImGui::EndListBox();
		}
	}

	static void RenderPlayerEditor()
	{
		if (!g_SelectedPlayer || g_SelectedRid == 0)
			return;

		if (ImGui::BeginChild("##player-editor", {500, -100 /* static_cast<float>(*Pointers.ScreenResY - 688 - 38 * 4) */}, 0, ImGuiWindowFlags_NoBackground))
		{
			ImGui::SetNextItemWidth(180.f);
			if (ImGui::InputText(L("ui.name", "Name").c_str(), g_SelectedPlayerName, sizeof(g_SelectedPlayerName)))
			{
				g_SelectedPlayer->m_Name = g_SelectedPlayerName;
			}

			int old_rid = g_SelectedRid;
			ImGui::SetNextItemWidth(180.0f);
			if (ImGui::InputScalar(L("ui.rockstar_id", "Rockstar Id").c_str(), ImGuiDataType_U64, &g_SelectedRid))
			{
				SavedPlayers::UpdateRockstarId(old_rid, g_SelectedRid);
				g_SelectedPlayer = SavedPlayers::GetPlayerData(g_SelectedRid);
			}

			ImGui::Checkbox(L("saved_players.track_player", "Track Player").c_str(), &g_SelectedPlayer->m_TrackPlayer);

			if (g_SelectedPlayer->m_FetchedData)
			{
				auto& data = *g_SelectedPlayer->m_FetchedData;
				ImGui::Text("%s: %s", L("saved_players.session_type", "Session Type").c_str(), FetchedPlayerData::GameStateToString(data.m_GameState).data());
				ImGui::Text("%s: %s", L("saved_players.host_of_session", "Host of Session").c_str(), data.m_HostOfSession ? "Yes" : "No");
				ImGui::Text("%s: %s", L("saved_players.is_spectating", "Is Spectating").c_str(), data.m_Spectating ? "Yes" : "No");
				ImGui::Text("%s: %s", L("saved_players.is_job_lobby", "Is Job Lobby").c_str(), data.m_InTransition ? "Yes" : "No");
				ImGui::Text("%s: %s", L("saved_players.host_of_job_lobby", "Host of Job Lobby").c_str(), data.m_HostOfTransition ? "Yes" : "No");
				if (data.m_MissionType != FetchedPlayerData::MissionType::NONE)
				{
					ImGui::Text("%s: %s", L("saved_players.mission_type", "Mission Type").c_str(), FetchedPlayerData::MissionTypeToString(data.m_MissionType).data());
					if (data.m_MissionName)
						ImGui::Text("%s: %s", L("saved_players.mission_name", "Mission Name").c_str(), data.m_MissionName->data());
					else
						; // TODO: add fetch mission name
				}
			}
			else
			{
				ImGui::TextDisabled(L("saved_players.not_fetched", "Data not fetched yet").c_str());
			}

			if (ImGui::Button(L("btn.join", "Join").c_str()))
			{
				FiberPool::Push([] {
					Network::JoinRockstarId(g_SelectedRid);
				});
			}

			if (ImGui::Button(L("btn.save", "Save").c_str()))
			{
				SavedPlayers::Save();
			}
			ImGui::SameLine();
			if (ImGui::Button(L("btn.remove", "Remove").c_str()))
			{
				SavedPlayers::RemovePlayerData(g_SelectedRid);
				g_SelectedPlayer = nullptr;
				g_SelectedRid = 0;
			}
		}
		ImGui::EndChild();
	}

	static void RenderSavedPlayers()
	{
		RenderPlayerList();
		ImGui::SameLine();
		RenderPlayerEditor();
	}

	static void RenderAddNewPlayer()
	{
		static char name_buf[24]{};

		ImGui::SetNextItemWidth(180.0f);
		ImGui::InputText(L("ui.username", "Username").c_str(), name_buf, sizeof(name_buf));
		ImGui::SameLine();
		if (ImGui::Button(L("btn.add", "Add").c_str()))
			FiberPool::Push([] {
				auto rid = YimMenu::Network::ResolveRockstarId(name_buf);
				if (rid)
				{
					SavedPlayers::AddPlayerData(*rid, name_buf);
				}
				else
				{
					Notifications::Show(L("category.saved_players", "Saved Players").c_str(), L("saved_players.failed_rid", "Failed to get RID from username").c_str(), NotificationType::Error);
				}
			});
	}

	std::shared_ptr<Category> BuildSavedPlayersMenu()
	{
		auto menu = std::make_shared<Category>(L("category.saved_players", "Saved Players"));
		auto players = std::make_shared<Group>(L("group.players", "Players"));
		auto new_player = std::make_shared<Group>(L("group.new", "New"));
		auto tracking = std::make_shared<Group>(L("group.tracking", "Tracking"));
		auto notifications = std::make_shared<Group>(L("group.notifications", "Notifications"));

		players->AddItem(std::make_shared<ImGuiItem>([] {
			RenderSavedPlayers();
		}));

		new_player->AddItem(std::make_shared<ImGuiItem>([] {
			RenderAddNewPlayer();
		}));

		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifywhenjoinable"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifywhenunjoinable"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifywhenonline"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifywhenoffline"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifyonseschange"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifyonmischange"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifyonjoblobby"_J));

		auto update = std::make_shared<Group>("", 1);
		update->AddItem(std::make_shared<BoolCommandItem>("playerdbautoupdate"_J, L("saved_players.auto_update", "Auto Update").c_str()));
		update->AddItem(std::make_shared<CommandItem>("playerdbupdatenow"_J, L("saved_players.update_now", "Update Now").c_str()));

		tracking->AddItem(std::move(update));
		tracking->AddItem(std::make_shared<BoolCommandItem>("playerdbnotify"_J, L("saved_players.tracking_notifications", "Tracking Notifications").c_str()));
		tracking->AddItem(std::make_shared<ConditionalItem>("playerdbnotify"_J, std::move(notifications)));

		menu->AddItem(players);
		menu->AddItem(new_player);
		menu->AddItem(tracking);

		return menu;
	}
}