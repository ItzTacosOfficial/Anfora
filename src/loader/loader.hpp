#pragma once

#define __ANFORA_API_INTERNAL
#include "anfora/api.hpp"

#include "mod.hpp"
#include "hook.hpp"

#include <unordered_map>
#include <array>


class Loader {

public:

	static constexpr const wchar_t* ModuleNames[] = {
		L"faktum.dll",
		L"cm2.dll"
	};

	static constexpr size_t ModuleCount = std::size(ModuleNames);


	bool init();

	bool install();

	void exit();


	void* getHookTarget(const Anfora::ModInfo& info, uintptr_t address, unsigned int moduleID);

private:

	size_t appendHooks(const Mod& mod);

	bool attachHooks();

	void detachHooks();


	static void freeLibrary(HMODULE module);


	HMODULE modules[2];

	std::unordered_map<HMODULE, Mod> mods;
	std::vector<HMODULE> failedMods;

	HookMap nodes;

};
