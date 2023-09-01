#include "mod.hpp"

#include "anforaapi.hpp"
#include "loader.hpp"
#include "hook.hpp"

#include "detours.h"


bool Mod::load(HMODULE module) {

	if (!module) {
		return false;
	}


	info = reinterpret_cast<AnforaModInfo*>(GetProcAddress(module, InfoSymbol));
	modModule = reinterpret_cast<HMODULE*>(GetProcAddress(module, ModuleSymbol));

	if (!info || !modModule) {

		FtGetTextDevice()->Log(FTextDevice::Error, L"Invalid DLL (missing 'anfora_mod' statement)\n");

		return false;

	}

	*modModule = module;
	initFunction = reinterpret_cast<InitT>(GetProcAddress(module, InitSymbol));
	exitFunction = reinterpret_cast<ExitT>(GetProcAddress(module, ExitSymbol));

	DetourEnumerateExports(module, this, enumerateExports);

	return true;

}


bool Mod::initMod() const {

	if (initFunction) {
		return initFunction();
	}

	return true;

}

void Mod::exitMod() const {

	if (exitFunction) {
		exitFunction();
	}

}


bool Mod::attachHooks() {

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	for (auto& [target, node] : hooksNodes) {

		FtGetTextDevice()->Logf(FTextDevice::Init, L"Attaching hooks (target 0x%08X)\n", target);

		for (auto& hook : node.hooks) {

			if (DetourAttach(&hook.target, hook.detour) != NO_ERROR) {

				FtGetTextDevice()->Logf(FTextDevice::Critical, L"Failed to attach hook 0x%08X (source '%ls')\n", target, info->name);

				DetourTransactionAbort();

				return false;

			}

		}

	}

	if (DetourTransactionCommit() != NO_ERROR) {

		FtGetTextDevice()->Log(FTextDevice::Critical, L"Failed to commit detour transaction\n");

		return false;

	}

	return true;

}

void Mod::detachHooks() {

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	for (auto& [target, node] : hooksNodes) {

		for (auto& hook : node.hooks) {
			DetourDetach(&hook.target, hook.detour);
		}

	}

	DetourTransactionCommit();

}


BOOL WINAPI Mod::enumerateExports(PVOID pContext, ULONG nOrdinal, LPCSTR pszName, PVOID pCode) {

	Mod* self = static_cast<Mod*>(pContext);

	uintptr_t address;
	unsigned int moduleID;

	if (!parseHookSymbol(pszName, address, moduleID)) {
		return TRUE;
	}

	if (moduleID > ModulesCount - 1) {

		FtGetTextDevice()->Logf(FTextDevice::Warning, L"Hook 0x%08X (source '%ls') has invalid module ID %i", address, self->info->name, moduleID);

		return TRUE;

	}

	address += work.baseAddresses[moduleID];

	self->hooksNodes[address].appendHook({reinterpret_cast<void*>(address), pCode, 0});

	return TRUE;

}

bool Mod::parseHookSymbol(const std::string& sym, uintptr_t& address, unsigned int& moduleID) {

	// $$anfora_hook$<address>$<moduleID>$$

	constexpr const char Identifier[] = "$$anfora_hook$";
	constexpr char Separator = '$';
	constexpr const char End[] = "$$";

	auto stringToUInt = [](unsigned int& value, const std::string& str, bool hex) {

		size_t pos;

		value = std::stoul(str, &pos, hex ? 16 : 10);

		if (pos != str.length()) {
			return false;
		}

		return true;

	};

	size_t pos = sym.find(Identifier);

	if (pos == std::string::npos) {
		return false;
	}

	size_t addressPos = pos + std::size(Identifier) - 1;

	pos = sym.find(Separator, addressPos);

	if (pos == std::string::npos) {
		return false;
	}

	std::string addressStr = sym.substr(addressPos, pos - addressPos);

	pos = sym.find(End, addressPos);

	if (pos == std::string::npos) {
		return false;
	}

	size_t modulePos = pos - 1;

	std::string moduleStr = sym.substr(modulePos, pos - modulePos);

	if (!stringToUInt(address, addressStr, true)) {
		return false;
	}

	if (!stringToUInt(moduleID, moduleStr, false)) {
		return false;
	}

	return true;

}
