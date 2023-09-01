#pragma once

#include <unordered_map>
#include <cstdint>

#include "faktum/faktum.hpp"


static constexpr const wchar_t* ModulesNames[] = {
	L"faktum.dll",
	L"cm2.dll"
};

static constexpr size_t ModulesCount = std::size(ModulesNames);


class Mod;

struct Work {

	HMODULE modulesHandles[ModulesCount];
	uintptr_t baseAddresses[ModulesCount];

	std::unordered_map<HMODULE, Mod> mods;
	std::vector<HMODULE> failedMods;

};

extern Work work;
