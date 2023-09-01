#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

#include "faktum/faktum.hpp"
#include "detours.h"



struct AnforaModInfo;

struct HooksNode;


class Mod {

public:

	using InitT = bool(*)();
	using ExitT = void(*)();


	bool load(HMODULE module);

	bool initMod() const; // Calls 'anfora_init' statement
	void exitMod() const; // Calls 'anfora_exit' statement

	bool attachHooks();
	void detachHooks();


	AnforaModInfo* info; // Mod info contained in the DLL
	HMODULE* modModule; // Mod module handle contained in the DLL (used to call _AnfGetHookTarget internally)
	InitT initFunction;
	ExitT exitFunction;
	std::unordered_map<uintptr_t, HooksNode> hooksNodes;

private:

	static constexpr const char* InitSymbol = "_AnModInit";
	static constexpr const char* ExitSymbol = "_AnModExit";
	static constexpr const char* InfoSymbol = "_AnModInfo";
	static constexpr const char* ModuleSymbol = "_AnModModule";


	static BOOL WINAPI enumerateExports(PVOID pContext, ULONG nOrdinal, LPCSTR pszName, PVOID pCode);
	static bool parseHookSymbol(const std::string& sym, uintptr_t& address, unsigned int& moduleID);

};
