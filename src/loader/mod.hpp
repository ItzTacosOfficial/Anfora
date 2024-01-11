#pragma once

#define __ANFORA_API_INTERNAL
#include "anfora/api.hpp"


struct Mod {

	using InitT = bool(*)();
	using ExitT = void(*)();


	constexpr Mod() = default;


	bool load(HMODULE module);

	bool init() const;

	void exit() const;


	Anfora::ModInfo* info;

	InitT initFunction;
	ExitT exitFunction;

};
