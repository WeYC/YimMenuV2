#include "Toxic.hpp"
#include "core/i18n/Language.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildKickMenu()
	{
		auto menu = std::make_shared<Category>(L("group.kicks", "Kick"));

		auto kicks = std::make_shared<Group>(L("group.kicks", "Kicks"));
		kicks->AddItem(std::make_shared<PlayerCommandItem>("hkick"_J));
		kicks->AddItem(std::make_shared<PlayerCommandItem>("shkick"_J));
		kicks->AddItem(std::make_shared<PlayerCommandItem>("endkick"_J));
		kicks->AddItem(std::make_shared<PlayerCommandItem>("nfkick"_J));

		auto other = std::make_shared<Group>(L("group.other", "Other"));
		other->AddItem(std::make_shared<PlayerCommandItem>("blackscreen"_J));

		menu->AddItem(kicks);
		menu->AddItem(other);

		return menu;
	}
}