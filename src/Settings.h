#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	void Load()
	{
				
		constexpr auto path = L"Data/SKSE/Plugins/po3_SimpleOffenceSuppression.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		detail::get_value(ini, setAsAlly, "Settings", "SetAsAlly", ";Whether neutral NPCs should be considered as friends or allies when hitting them. This affects the number of hits they'll take before turning hostile (see gamesettings)");
		detail::get_value(ini, onlyCombat, "Settings", "OnlyCombat", ";NPCs will only ignore player hits in combat");
		detail::get_value(ini, ignoreFriendlyFire, "Settings", "IgnoreFriendlyFire", ";Neutral NPCs will ALWAYS ignore hits from the player");
		detail::get_value(ini, changeGameSettings, "Settings", "ModifyGameSettings", ";Modify included gamesettings using this config");

		detail::get_value(ini, iFriendHitNonCombatAllowed, "GameSettings", "iFriendHitNonCombatAllowed", nullptr);
		detail::get_value(ini, iFriendHitCombatAllowed, "GameSettings", "iFriendHitCombatAllowed", nullptr);
		detail::get_value(ini, iAllyHitNonCombatAllowed, "GameSettings", "iAllyHitNonCombatAllowed", nullptr);
		detail::get_value(ini, iAllyHitCombatAllowed, "GameSettings", "iAllyHitCombatAllowed", nullptr);

		if (setAsAlly) {
			newRank = RE::FIGHT_REACTION::kAlly;
		}

		ini.SaveFile(path);
	}

	bool setAsAlly{ false };
	bool onlyCombat{ true };
	bool ignoreFriendlyFire{ false };

	bool changeGameSettings{ false };
	
	std::int32_t iFriendHitNonCombatAllowed{ 0 };
	std::int32_t iFriendHitCombatAllowed{ 4 };
	std::int32_t iAllyHitNonCombatAllowed{ 3 };
	std::int32_t iAllyHitCombatAllowed{ 1000 };

	RE::FIGHT_REACTION newRank{ RE::FIGHT_REACTION::kFriend };

private:
	struct detail
	{
		static void get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
			a_ini.SetBoolValue(a_section, a_key, a_value, a_comment);
		};

		static void get_value(CSimpleIniA& a_ini, std::int32_t& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			try {				
				a_value = string::lexical_cast<std::int32_t>(a_ini.GetValue(a_section, a_key, std::to_string(a_value).c_str()));
				a_ini.SetValue(a_section, a_key, std::to_string(a_value).c_str(), a_comment);
			} catch (...) {
			}
		}
	};
};
