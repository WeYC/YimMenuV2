#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/i18n/Language.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class FixAllVehicles : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			int count = 0;
			for (const auto& it : PersonalVehicles::GetPersonalVehicles())
			{
				const auto& personalVeh = it.second;
				if (personalVeh->Repair())
					count++;
			}

			if (count > 0)
				Notifications::Show(L("notification.fix_all_vehicles", "Fix All Vehicles").c_str(), (L("notification.vehicles_fixed", "Fixed ") + std::to_string(count) + L("notification.vehicles_fixed2", " vehicles.")).c_str(), NotificationType::Success);
			else
				Notifications::Show(L("notification.fix_all_vehicles", "Fix All Vehicles").c_str(), L("notification.no_vehicles_fix", "No vehicles to fix.").c_str());
		}
	};

	static FixAllVehicles _FixAllVehicles{"fixallvehicles", "Fix All Vehicles", "Fixes all of your destroyed personal vehicles."};
}