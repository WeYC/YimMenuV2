#pragma once
#include "core/util/Joaat.hpp"
#include "core/i18n/Language.hpp"

#include <nlohmann/json.hpp>


namespace YimMenu
{
	class Command
	{
	private:
		int m_NumArgs = 0; // TODO: currently unused

	protected:
		virtual void OnCall() = 0;
		void MarkDirty();
		std::string m_Name;
		std::string m_Label;
		std::string m_Description;
		joaat_t m_Hash;

	public:
		Command(std::string name, std::string label, std::string description, int num_args = 0);
		virtual ~Command() = default;
		void Call();

		// Lua-created commands override this so runtime commands don't leave entries behind in the config file.
		virtual bool ShouldSaveState() const
		{
			return true;
		}

		virtual void SaveState(nlohmann::json& value) {};
		virtual void LoadState(nlohmann::json& value) {};

		const std::string& GetName()
		{
			return m_Name;
		}

		std::string GetLabel()
		{
			auto tr = I18n::Get(m_Name + ".label");
			return tr.empty() ? m_Label : std::string(tr);
		}

		std::string GetDescription()
		{
			auto tr = I18n::Get(m_Name + ".description");
			return tr.empty() ? m_Description : std::string(tr);
		}

		joaat_t GetHash()
		{
			return m_Hash;
		}
	};
}