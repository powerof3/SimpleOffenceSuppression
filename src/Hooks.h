#pragma once

namespace SimpleOffenceSuppression
{
	template <class T>
	struct GetFactionFightReaction
	{
		static RE::FIGHT_REACTION thunk(RE::Actor* a_subject, RE::Actor* a_player)
		{
			const auto fightReaction = func(a_subject, a_player);
			if (fightReaction == RE::FIGHT_REACTION::kNeutral) {
				if (a_subject && a_player) {
					return T::func(a_subject, a_player);
				}
			}
			return fightReaction;
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(37672, 38626), OFFSET(0x187, 0x182) };
			stl::write_thunk_call<GetFactionFightReaction<T>>(target.address());
		}
	};

    struct IgnoreFriendlyFire
	{
		static RE::FIGHT_REACTION func(RE::Actor* a_subject, RE::Actor* a_player);
    };

	struct OnlyCombat
	{
		static RE::FIGHT_REACTION func(RE::Actor* a_subject, RE::Actor* a_player);
    };

	struct Default
	{
		static RE::FIGHT_REACTION func(RE::Actor* a_subject, RE::Actor* a_player);
    };

	void InstallOnPostLoad();

	void InstallOnDataLoad();
}
