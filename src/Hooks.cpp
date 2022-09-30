#include "Hooks.h"
#include "Settings.h"

namespace SimpleOffenceSuppression
{
	RE::FIGHT_REACTION IgnoreFriendlyFire::func(RE::Actor* a_subject, RE::Actor* a_player)
	{
		using FLAGS = RE::TESObjectREFR::RecordFlags;

		if (!a_subject->IsHostileToActor(a_player)) {
			if ((a_subject->formFlags & FLAGS::kIgnoreFriendlyHits) == 0) {
				a_subject->formFlags |= FLAGS::kIgnoreFriendlyHits;
			}
			return Settings::GetSingleton()->newReaction;
		}
		return RE::FIGHT_REACTION::kNeutral;
	}

	RE::FIGHT_REACTION OnlyCombat::func(RE::Actor* a_subject, RE::Actor* a_player)
	{
		if (!a_subject->IsHostileToActor(a_player) && a_player->IsInCombat()) {
			return Settings::GetSingleton()->newReaction;
		}
		return RE::FIGHT_REACTION::kNeutral;
	}

	RE::FIGHT_REACTION Default::func(RE::Actor* a_subject, RE::Actor* a_player)
	{
		if (!a_subject->IsHostileToActor(a_player)) {
			return Settings::GetSingleton()->newReaction;
		}
		return RE::FIGHT_REACTION::kNeutral;
	}

	void InstallOnPostLoad()
	{
		const auto settings = Settings::GetSingleton();
		if (settings->ignoreFriendlyFire) {
			GetFactionFightReaction<IgnoreFriendlyFire>::Install();
		} else if (settings->onlyCombat) {
			GetFactionFightReaction<OnlyCombat>::Install();
		} else {
			GetFactionFightReaction<Default>::Install();
		}
	}

	void InstallOnDataLoad()
	{
		const auto settings = Settings::GetSingleton();
		if (settings->changeGameSettings) {
			constexpr auto set_gmst = [](const char* a_name, std::int32_t a_value) {
				if (auto gameSetting = RE::GameSettingCollection::GetSingleton()->GetSetting(a_name)) {
					gameSetting->data.i = a_value;
				}
			};
			set_gmst("iFriendHitNonCombatAllowed", settings->iFriendHitNonCombatAllowed);
			set_gmst("iFriendHitCombatAllowed", settings->iFriendHitCombatAllowed);
			set_gmst("iAllyHitNonCombatAllowed", settings->iAllyHitNonCombatAllowed);
			set_gmst("iAllyHitCombatAllowed", settings->iAllyHitCombatAllowed);
		}
		logger::info("OnlyCombat : {}", settings->onlyCombat);
		logger::info("SetAsAlly : {}", settings->setAsAlly);
		logger::info("IgnoreFriendlyFire : {}", settings->ignoreFriendlyFire);
	}
}
