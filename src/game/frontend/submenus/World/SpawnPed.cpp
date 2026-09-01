#include "SpawnPed.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/i18n/Language.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/data/PedModels.hpp"
#include "game/gta/Ped.hpp"
#include "game/gta/Natives.hpp"
#include "types/ped/PedCombatAttribute.hpp"
#include "game/gta/data/Weapons.hpp"
#include "game/gta/Scripts.hpp"
#include "game/backend/NativeHooks.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildSpawnPedMenu()
	{
		auto menu = std::make_shared<Category>(L("category.spawn_ped", "Spawn Ped"));

		static bool invincible;
		static bool spawnDead;
		static bool spawnAsBodyguard;
		static bool spawnAsCop;
		static bool spawnInMyVehicle;
		static bool giveAllWeapons;
		static bool spawnAsProstitute;
		static bool randomizeOutfit;
		static bool blipPed;
		static std::vector<Ped> spawnedPeds;

		menu->AddItem(std::make_unique<ImGuiItem>([] {
			static char search[64];
			ImGui::SetNextItemWidth(300.f);
			ImGui::InputTextWithHint("##ped_name", L("spawn_ped.name", "Name").c_str(), search, sizeof(search));

			const int visible = std::min(20, static_cast<int>(g_PedModels.size()));
			const float height = visible * ImGui::GetTextLineHeightWithSpacing();
			if (ImGui::BeginListBox("##peds", {300.f, height}))
			{
				std::string lower = search;
				std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
				for (int veh = 0; veh < g_PedModels.size(); veh++)
				{
					auto name = g_PedModels[veh];
					auto lowerName = std::string(name);
					std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

					bool matchesSearch = lowerName.find(lower) != std::string::npos;
					if (matchesSearch)
					{
						ImGui::PushID(name);
						if (ImGui::Selectable(name))
						{
							auto set_player = ImGui::GetIO().KeyCtrl;
							FiberPool::Push([name, set_player] {
								if (spawnInMyVehicle)
								{
									auto vehicle = Self::GetVehicle();
									if (vehicle)
									{
										if (!vehicle.IsSeatFree(-2) && 
										    !vehicle.IsSeatFree(-1))
										{
										Notifications::Show(
										    L("spawn_ped.title", "Spawn Ped").c_str(),
										    L("spawn_ped.seats_occupied", "Cannot spawn ped in vehicle, all seats are occupied, please free a seat first or disable 'Spawn In My Vehicle' option.").c_str(),
										    NotificationType::Warning);
											return;
										}
									}
								}

								auto hash = Joaat(name);
								auto handle = Ped::Create(hash, Self::GetPed().GetPosition(), Self::GetPed().GetHeading());

								if (!handle)
									return;

								handle.SetCombatAttribute(PedCombatAttribute::AlwaysFight, true);
								handle.SetCombatAttribute(PedCombatAttribute::DisableAllRandomsFlee, true);
								handle.SetCombatAttribute(PedCombatAttribute::DisableFleeFromCombat, true);
								handle.SetCombatAttribute(PedCombatAttribute::AlwaysFlee, false);
								handle.SetCombatAttribute(PedCombatAttribute::FleesFromInvincibleOpponents, false);
								handle.SetCombatAttribute(PedCombatAttribute::CanUseVehicles, true);
								handle.SetCombatAttribute(PedCombatAttribute::CanLeaveVehicle, true);

								if (invincible)
									handle.SetInvincible(true);

								if (spawnDead)
									handle.Kill();

								if (spawnAsBodyguard && !set_player)
								{
									handle.SetCombatAttribute(PedCombatAttribute::CanCharge, true);
									handle.SetCombatAttribute(PedCombatAttribute::CanCommandeerVehicles, true);
									handle.SetCombatAttribute(PedCombatAttribute::DisableInjuredOnGround, true);
									handle.SetCombatAttribute(PedCombatAttribute::AllowDogFighting, true);
									handle.SetCombatAttribute(PedCombatAttribute::PerfectAccuracy, true);
									handle.SetCombatAttribute(PedCombatAttribute::UseVehicleAttack, true);
									handle.SetCombatAttribute(PedCombatAttribute::CanDoDrivebys, true);
									handle.SetCombatAttribute(PedCombatAttribute::CanThrowSmokeGrenade, true);
									handle.SetCombatAttribute(PedCombatAttribute::CanSeeUnderwaterPeds, true);
									
									auto group = Self::GetPlayer().GetGroup();
									handle.AddToGroup(group);
									PED::SET_GROUP_SEPARATION_RANGE(group, 9999.9f);
									PED::SET_PED_CAN_TELEPORT_TO_GROUP_LEADER(handle.GetHandle(), group, true);
									PED::SET_GROUP_FORMATION_SPACING(group, 10.0f, 0.0f, 0.0f);
								}

								if (spawnAsCop)
								{
									handle.SetAsCop();
								}

								if (spawnInMyVehicle)
								{
									auto vehicle = Self::GetVehicle();
									if (vehicle) {
										if (vehicle.IsSeatFree(-1))
											handle.SetInVehicle(vehicle, -1);
										else
											handle.SetInVehicle(vehicle, -2);
									}
								}

								if (giveAllWeapons)
								{
									for (auto hash : g_WeaponHashes)
										handle.GiveWeapon(hash);
								}

								if (randomizeOutfit)
									handle.RandomizeOutfit();

								if (blipPed)
								{
									HUD::SET_BLIP_COLOUR(HUD::ADD_BLIP_FOR_ENTITY(handle.GetHandle()), 3);
								}

								if (spawnAsProstitute)
								{
									handle.StartScenario("WORLD_HUMAN_PROSTITUTE_HIGH_CLASS");
									handle.SetKeepTask(true);

									if (!*Pointers.IsSessionStarted)
									{
										int ped = handle.GetHandle();
										Scripts::StartScript("pb_prostitute"_J, eStackSizes::FRIEND, &ped, 1);
									}
								}

								if (set_player)
								{
									static auto hooked = []()
									{
										NativeHooks::AddHook("freemode"_J, NativeIndex::GET_ENTITY_MODEL, [](rage::scrNativeCallContext* ctx) {
											auto model = ENTITY::GET_ENTITY_MODEL(ctx->GetArg<int>(0));

											if (ctx->GetArg<int>(0) == Self::GetPed().GetHandle() && (model != "mp_m_freemode_01"_J && model != "mp_f_freemode_01"_J))
											{
												return ctx->SetReturnValue("mp_m_freemode_01"_J);
											}

											return ctx->SetReturnValue(model);
										});
										for (auto script : {"main"_J, "respawn_controller"_J, "pi_menu"_J})
										{
											NativeHooks::AddHook(script, NativeIndex::GET_ENTITY_MODEL, [](rage::scrNativeCallContext* ctx) {
												auto model = ENTITY::GET_ENTITY_MODEL(ctx->GetArg<int>(0));

												if (ctx->GetArg<int>(0) == Self::GetPed().GetHandle() && (model != "player_zero"_J && model != "player_one"_J && model != "player_two"_J))
												{
													return ctx->SetReturnValue("player_zero"_J);
												}

												return ctx->SetReturnValue(model);
											}); 
										}
										return true;
									}();
									Self::GetPlayer().SetPed(handle);
								}
								else
								{
									spawnedPeds.push_back(handle);
								}
							});
						}
						ImGui::PopID();
					}
				}

				ImGui::EndListBox();
			}

			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::BulletText(L("spawn_ped.ctrl_click_hint", "Ctrl+Click to set player model").c_str());
			ImGui::Checkbox(L("spawn_ped.invincible", "Invincible").c_str(), &invincible);
			ImGui::Checkbox(L("spawn_ped.spawn_dead", "Spawn Dead").c_str(), &spawnDead);
			ImGui::Checkbox(L("spawn_ped.spawn_as_bodyguard", "Spawn As Bodyguard").c_str(), &spawnAsBodyguard);
			ImGui::Checkbox(L("spawn_ped.spawn_as_cop", "Spawn As Cop").c_str(), &spawnAsCop);
			ImGui::Checkbox(L("spawn_ped.spawn_in_my_vehicle", "Spawn In My Vehicle").c_str(), &spawnInMyVehicle);
			ImGui::Checkbox(L("spawn_ped.give_all_weapons", "Give All Weapons").c_str(), &giveAllWeapons);
			ImGui::Checkbox(L("spawn_ped.spawn_as_prostitute", "Spawn As Prostitute").c_str(), &spawnAsProstitute);
			ImGui::Checkbox(L("spawn_ped.randomize_outfit", "Randomize Outfit").c_str(), &randomizeOutfit);
			ImGui::Checkbox(L("spawn_ped.blip_ped", "Blip Ped").c_str(), &blipPed);
			if (ImGui::Button(L("spawn_ped.remove_all", "Remove All").c_str()))
			{
				FiberPool::Push([] {
					for (auto& ped : spawnedPeds)
						if (ped)
							ped.Delete();

					spawnedPeds.clear();
				});
			}
			ImGui::EndGroup();
		}));

		return menu;
	}
}