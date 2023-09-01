#include "loader.hpp"

#define _ANFORA_API_EXPORT
#include "anforaapi.hpp"
#include "mod.hpp"
#include "hook.hpp"

#include "faktum/faktum.hpp"


_ANFAPI void* _AnfGetHookTarget(HMODULE modModule, uintptr_t addr, unsigned int moduleID) {

	auto& mod = work.mods.at(modModule);

	uintptr_t realAddr = addr + work.baseAddresses[moduleID];

	if (mod.hooksNodes.contains(realAddr)) {

		return mod.hooksNodes.at(realAddr).getNextTarget();

	}

	FtGetTextDevice()->Logf(FTextDevice::Critical, L"Could not find target of hook %08X %s (%ls)\n", addr, ModulesNames[moduleID], mod.info->name);

	constexpr const wchar_t* MsgBoxText = L"Anfora caught a fatal error\nCheck cm2.log for further information";
	constexpr const wchar_t* MsgBoxTitle = L"Anfora Modloader";

	MessageBoxW(nullptr, MsgBoxText, MsgBoxTitle, MB_OK | MB_ICONERROR);

	std::exit(1);

}
