#pragma once
#include <string>
#include <string_view>

namespace YimMenu::I18n
{
	struct TranslationEntry
	{
		std::string_view key;
		std::string_view value;
	};

	// Translation table
	// Command keys: "{command_name}.label", "{command_name}.description"
	// UI keys: "submenu.{name}", "category.{name}", "group.{name}", "ui.{name}"
	inline constexpr TranslationEntry g_Translations[] = {
		// Add translations here, e.g.:
		// {"godmode.label", "上帝模式"},
		// {"godmode.description", "阻挡所有伤害"},
		// {"submenu.self", "自身"},
		// {"submenu.vehicle", "载具"},
	};

	// Look up a translation by key
	// Returns empty string_view if not found
	inline std::string_view Get(std::string_view key)
	{
		for (const auto& entry : g_Translations)
		{
			if (entry.key == key)
				return entry.value;
		}
		return {};
	}

	// Look up a translation with fallback
	inline std::string GetOrDefault(std::string_view key, std::string_view fallback)
	{
		auto result = Get(key);
		return result.empty() ? std::string(fallback) : std::string(result);
	}
}

// Macro for inline usage with fallback
// Usage: L("submenu.self", "Self")
#define L(key, fallback) YimMenu::I18n::GetOrDefault(key, fallback)
