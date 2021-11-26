#include "Settings.h"

namespace SimpleOffenceSuppression
{
	namespace IgnoreFriendlyFire
	{
		struct GetFactionRank
		{
			static std::uint32_t thunk(RE::Actor* a_subject, RE::Actor* a_target)
			{
				using FLAGS = RE::TESObjectREFR::RecordFlags;

				const auto factionRank = func(a_subject, a_target);
				if (factionRank == 0) {
					if (a_subject && a_target && !a_subject->IsHostileToActor(a_target)) {
						if ((a_subject->formFlags & FLAGS::kIgnoreFriendlyHits) == 0) {
							a_subject->formFlags |= FLAGS::kIgnoreFriendlyHits;
						}
						return stl::to_underlying(Settings::GetSingleton()->newRank);
					}
				}
				return factionRank;
			}
			static inline REL::Relocation<decltype(&thunk)> func;
		};
	}

	namespace OnlyCombat
	{
		struct GetFactionRank
		{
			static std::uint32_t thunk(RE::Actor* a_subject, RE::Actor* a_target)
			{
				const auto factionRank = func(a_subject, a_target);
				if (factionRank == 0) {
					if (a_target && a_subject && !a_subject->IsHostileToActor(a_target) && a_target->IsInCombat()) {
						return stl::to_underlying(Settings::GetSingleton()->newRank);
					}
				}
				return factionRank;
			}
			static inline REL::Relocation<decltype(&thunk)> func;
		};
	}

	namespace
	{
		struct GetFactionRank
		{
			static std::uint32_t thunk(RE::Actor* a_subject, RE::Actor* a_target)
			{
				const auto factionRank = func(a_subject, a_target);
				if (factionRank == 0) {
					if (a_target && a_subject && !a_subject->IsHostileToActor(a_target)) {
						return stl::to_underlying(Settings::GetSingleton()->newRank);
					}
				}
				return factionRank;
			}
			static inline REL::Relocation<decltype(&thunk)> func;
		};
	}

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(38626) };

		const auto settings = Settings::GetSingleton();
		if (settings->ignoreFriendlyFire) {
			stl::write_thunk_call<IgnoreFriendlyFire::GetFactionRank>(target.address() + 0x182);
		} else if (settings->onlyCombat) {
			stl::write_thunk_call<OnlyCombat::GetFactionRank>(target.address() + 0x182);
		} else {
			stl::write_thunk_call<GetFactionRank>(target.address() + 0x182);
		}
	}
}

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
		const auto settings = Settings::GetSingleton();
		if (settings->changeGameSettings) {
			logger::info("Setting gamesettings...");

			auto gmstCollection = RE::GameSettingCollection::GetSingleton();
			if (gmstCollection) {
				const auto set_gmst = [&](const char* a_name, std::int32_t a_value) {
					auto gameSetting = gmstCollection->GetSetting(a_name);
					if (gameSetting) {
						gameSetting->data.i = a_value;
					}
				};

				set_gmst("iFriendHitNonCombatAllowed", settings->iFriendHitNonCombatAllowed);
				set_gmst("iFriendHitCombatAllowed", settings->iFriendHitCombatAllowed);
				set_gmst("iAllyHitNonCombatAllowed", settings->iAllyHitNonCombatAllowed);
				set_gmst("iAllyHitCombatAllowed", settings->iAllyHitCombatAllowed);
			}
		}
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Version::MAJOR);
	v.PluginName("Simple Offence Suppression");
	v.AuthorName("powerofthree");
	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();

	logger::info("loaded");

	SKSE::Init(a_skse);

	Settings::GetSingleton()->Load();
	SKSE::AllocTrampoline(14);

	SimpleOffenceSuppression::Install();

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener("SKSE", OnInit);

	return true;
}
