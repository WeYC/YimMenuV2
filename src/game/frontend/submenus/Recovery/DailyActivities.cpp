#include "DailyActivities.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildDailyActivitiesMenu()
	{
		auto dailyActivities = std::make_shared<Category>(L("category.daily_activities", "Daily Activities"));

		auto challengesTab       = std::make_shared<CollapsingHeaderItem>(L("group.challenges", "Challenges"));
		auto hiddenCacheTab      = std::make_shared<CollapsingHeaderItem>(L("group.hidden_caches", "Hidden Caches"));
		auto treasureChestTab    = std::make_shared<CollapsingHeaderItem>(L("group.treasure_chests", "Treasure Chests"));
		auto shipwreckedTab      = std::make_shared<CollapsingHeaderItem>(L("group.shipwreck", "Shipwreck"));
		auto buriedStashTab      = std::make_shared<CollapsingHeaderItem>(L("group.buried_stashes", "Buried Stashes"));
		auto skydiveTab          = std::make_shared<CollapsingHeaderItem>(L("group.junk_energy_skydives", "Junk Energy Skydives"));
		auto timeTrialsTab       = std::make_shared<CollapsingHeaderItem>(L("group.time_trials", "Time Trials"));
		auto exoticExportsTab    = std::make_shared<CollapsingHeaderItem>(L("group.exotic_exports", "Exotic Exports"));
		auto deadDropTab         = std::make_shared<CollapsingHeaderItem>(L("group.gs_cache", "G's Cache"));
		auto stashHouseTab       = std::make_shared<CollapsingHeaderItem>(L("group.stash_house", "Stash House"));
		auto streetDealerTab     = std::make_shared<CollapsingHeaderItem>(L("group.street_dealers", "Street Dealers"));
		auto lsTagsTab           = std::make_shared<CollapsingHeaderItem>(L("group.ls_tags", "LS Tags"));
		auto madrazoHitsTab      = std::make_shared<CollapsingHeaderItem>(L("group.madrazo_hits", "Madrazo Hits"));
		auto wildlifePhotography = std::make_shared<CollapsingHeaderItem>(L("group.shoot_animals_photography", "Shoot Animals Photography"));
		auto smokeOnTheWater     = std::make_shared<CollapsingHeaderItem>(L("group.smoke_on_the_water", "Smoke on the Water"));
		auto goldenClover        = std::make_shared<CollapsingHeaderItem>(L("group.golden_clover", "Golden Clover"));

		dailyActivities->AddItem(std::make_shared<CommandItem>("setallactivitiescompleted"_J));
		dailyActivities->AddItem(std::make_shared<CommandItem>("resetallactivities"_J));

		challengesTab->AddItem(std::make_shared<CommandItem>("completeallchallenges"_J));

		hiddenCacheTab->AddItem(std::make_shared<ListCommandItem>("hiddencacheindex"_J));
		hiddenCacheTab->AddItem(std::make_shared<CommandItem>("tptohiddencache"_J));
		hiddenCacheTab->AddItem(std::make_shared<CommandItem>("collecthiddencache"_J));

		treasureChestTab->AddItem(std::make_shared<ListCommandItem>("treasurechestindex"_J));
		treasureChestTab->AddItem(std::make_shared<CommandItem>("tptotreasurechest"_J));
		treasureChestTab->AddItem(std::make_shared<CommandItem>("collecttreasurechest"_J));
		treasureChestTab->AddItem(std::make_shared<BoolCommandItem>("enabletreasurechestinls"_J));

		shipwreckedTab->AddItem(std::make_shared<CommandItem>("tptoshipwrecked"_J));
		shipwreckedTab->AddItem(std::make_shared<CommandItem>("collectshipwrecked"_J));

		buriedStashTab->AddItem(std::make_shared<ListCommandItem>("buriedstashindex"_J));
		buriedStashTab->AddItem(std::make_shared<CommandItem>("tptoburiedstash"_J));
		buriedStashTab->AddItem(std::make_shared<CommandItem>("collectburiedstash"_J));
		buriedStashTab->AddItem(std::make_shared<BoolCommandItem>("enableburiedstashinls"_J));

		skydiveTab->AddItem(std::make_shared<ListCommandItem>("skydiveindex"_J));
		skydiveTab->AddItem(std::make_shared<CommandItem>("tptoskydive"_J));
		skydiveTab->AddItem(std::make_shared<CommandItem>("completeskydive"_J));

		// TO-DO: Add HSW Time Trial
		timeTrialsTab->AddItem(std::make_shared<ListCommandItem>("timetrialindex"_J));
		timeTrialsTab->AddItem(std::make_shared<CommandItem>("tptotimetrial"_J));
		timeTrialsTab->AddItem(std::make_shared<CommandItem>("beattimetrial"_J));

		exoticExportsTab->AddItem(std::make_shared<CommandItem>("tptoexoticexportsvehicle"_J));
		exoticExportsTab->AddItem(std::make_shared<CommandItem>("delivernextexoticexportsvehicle"_J));

		deadDropTab->AddItem(std::make_shared<CommandItem>("tptodeaddrop"_J));
		deadDropTab->AddItem(std::make_shared<CommandItem>("collectdeaddrop"_J));

		stashHouseTab->AddItem(std::make_shared<CommandItem>("tptostashhouse"_J));
		stashHouseTab->AddItem(std::make_shared<CommandItem>("enterstashhousesafecode"_J));

		streetDealerTab->AddItem(std::make_shared<ListCommandItem>("streetdealerindex"_J));
		streetDealerTab->AddItem(std::make_shared<CommandItem>("tptostreetdealer"_J));
		streetDealerTab->AddItem(std::make_shared<CommandItem>("openstreetdealermenu"_J));

		lsTagsTab->AddItem(std::make_shared<ListCommandItem>("lstagindex"_J));
		lsTagsTab->AddItem(std::make_shared<CommandItem>("tptolstag"_J));
		lsTagsTab->AddItem(std::make_shared<CommandItem>("spraylstag"_J));

		madrazoHitsTab->AddItem(std::make_shared<CommandItem>("tptomadrazohit"_J));
		madrazoHitsTab->AddItem(std::make_shared<CommandItem>("tptomadrazohittarget"_J));

		wildlifePhotography->AddItem(std::make_shared<ListCommandItem>("animalindex"_J));
		wildlifePhotography->AddItem(std::make_shared<CommandItem>("spawnanimal"_J));
		wildlifePhotography->AddItem(std::make_shared<CommandItem>("photographanimal"_J));

		smokeOnTheWater->AddItem(std::make_shared<ListCommandItem>("productindex"_J));
		smokeOnTheWater->AddItem(std::make_shared<CommandItem>("tptoproduct"_J));
		smokeOnTheWater->AddItem(std::make_shared<CommandItem>("collectproduct"_J));

		goldenClover->AddItem(std::make_shared<CommandItem>("tptogoldenclover"_J));
		goldenClover->AddItem(std::make_shared<CommandItem>("collectgoldenclover"_J));

		dailyActivities->AddItem(std::move(challengesTab));
		dailyActivities->AddItem(std::move(hiddenCacheTab));
		dailyActivities->AddItem(std::move(treasureChestTab));
		dailyActivities->AddItem(std::move(shipwreckedTab));
		dailyActivities->AddItem(std::move(buriedStashTab));
		dailyActivities->AddItem(std::move(skydiveTab));
		dailyActivities->AddItem(std::move(timeTrialsTab));
		dailyActivities->AddItem(std::move(exoticExportsTab));
		dailyActivities->AddItem(std::move(deadDropTab));
		dailyActivities->AddItem(std::move(stashHouseTab));
		dailyActivities->AddItem(std::move(streetDealerTab));
		dailyActivities->AddItem(std::move(lsTagsTab));
		dailyActivities->AddItem(std::move(madrazoHitsTab));
		dailyActivities->AddItem(std::move(wildlifePhotography));
		dailyActivities->AddItem(std::move(smokeOnTheWater));
		dailyActivities->AddItem(std::move(goldenClover));
		return dailyActivities;
	}
}