#include "mod.hpp"


bool Mod::load(HMODULE module) {

	if (!module) {
		return false;
	}

	info = reinterpret_cast<Anfora::ModInfo*>(GetProcAddress(module, Anfora::Internal::InfoSymbol));

	if (!info) {

		FtGetTextDevice()->Logf(FTextDevice::FMT_ERROR, L"Invalid DLL (`anfora_mod` missing)\n");

		return false;

	}

	initFunction = reinterpret_cast<InitT>(GetProcAddress(module, Anfora::Internal::InitSymbol));
	exitFunction = reinterpret_cast<ExitT>(GetProcAddress(module, Anfora::Internal::ExitSymbol));

	info->module = module;

	return true;

}

bool Mod::init() const {

	if (initFunction) {
		return initFunction();
	}

	return true;

}

void Mod::exit() const {

	if (exitFunction) {
		exitFunction();
	}

}
