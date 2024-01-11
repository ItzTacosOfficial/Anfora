#include "loader.hpp"

#include "detours.h"

#include <filesystem>


bool Loader::init() {

	FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Initializing Anfora...\n");


	FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Loading game library modules\n");

	for (size_t i = 0; const auto& name : ModuleNames) {

		HMODULE handle = GetModuleHandleW(name);

		if (!handle) {

			FtGetTextDevice()->Logf(FTextDevice::FMT_CRITICAL, L"Failed to get handle of module '%ls'\n", name);

			return false;

		}

		modules[i++] = handle;

	}


	std::filesystem::path modsDirectory("./mods/");

	if (!std::filesystem::exists(modsDirectory)) {

		FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"First time startup, nothing to load\n");

		if (!std::filesystem::create_directory(modsDirectory)) {

			FtGetTextDevice()->Log(FTextDevice::FMT_CRITICAL, L"Failed to create mods folder\n");

			return false;

		}

		FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Anfora initialized\n");

		return true;

	}


	FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Loading mods\n");

	for (const auto& entry : std::filesystem::directory_iterator(modsDirectory)) {

		if (!entry.is_regular_file()) {
			continue;
		}

		const auto& path = entry.path();


		FtGetTextDevice()->Logf(FTextDevice::FMT_INIT, L"Loading '%ls'\n", path.wstring().c_str());

		HMODULE module = LoadLibraryW(path.wstring().c_str());

		if (!module) {

			FtGetTextDevice()->Log(FTextDevice::FMT_ERROR, L"Failed to load DLL\n");

			continue;

		}


		Mod mod;

		if (!mod.load(module)) {

			freeLibrary(module);

			continue;

		}

		mods[module] = mod;

	}


	FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Initializing mods\n");

	for (const auto& [module, mod] : mods) {

		FtGetTextDevice()->Logf(FTextDevice::FMT_INIT, L"Initializing '%ls'\n", mod.info->name);


		if (!mod.init()) {

			FtGetTextDevice()->Logf(FTextDevice::FMT_ERROR, L"Failed to initialize mod\n");

			mod.exit();

			freeLibrary(module);

			failedMods.emplace_back(module);

		}

	}

	if (!failedMods.empty()) {

		size_t count = failedMods.size();

		FtGetTextDevice()->Logf(FTextDevice::FMT_WARNING, L"%i/%i Mods failed initialization\n", count, count + mods.size());

	}

	for (const auto& module : failedMods) {
		mods.erase(module);
	}


	if (!mods.empty()) {

		FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Searching hooks\n");

		for (const auto& [module, mod] : mods) {

			size_t count = appendHooks(mod);

			FtGetTextDevice()->Logf(FTextDevice::FMT_INIT, L"Found %d hooks in '%ls'\n", count, mod.info->name);

		}

	}


	FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Anfora initialized\n");

	return true;

}

bool Loader::install() {

	if (mods.empty() || nodes.empty()) {

		FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"No hooks to install\n");

		return true;

	}

	FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Installing hooks...\n");

	if (!attachHooks()) {
		return false;
	}


	FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Hooks installed\n");

	return true;

}

void Loader::exit() {

	FtGetTextDevice()->Log(FTextDevice::FMT_EXIT, L"Shutting down Anfora...\n");


	FtGetTextDevice()->Log(FTextDevice::FMT_EXIT, L"Detaching hooks\n");

	detachHooks();


	FtGetTextDevice()->Log(FTextDevice::FMT_EXIT, L"Exiting mods\n");

	for (auto& [module, mod] : mods) {

		FtGetTextDevice()->Logf(FTextDevice::FMT_EXIT, L"Exiting '%ls'\n", mod.info->name);

		mod.exit();

	}


	FtGetTextDevice()->Log(FTextDevice::FMT_EXIT, L"Unloading mods\n");

	for (auto& [module, mod] : mods) {

		FtGetTextDevice()->Logf(FTextDevice::FMT_EXIT, L"Unloading '%ls'\n", mod.info->name);

		freeLibrary(module);

	}

	mods.clear();


	FtGetTextDevice()->Log(FTextDevice::FMT_EXIT, L"Anfora shut down\n");

}


void* Loader::getHookTarget(const Anfora::ModInfo& info, uintptr_t address, unsigned int moduleID) {

	const auto& mod = mods[info.module];

	if (moduleID < ModuleCount) {

		address += reinterpret_cast<uintptr_t>(modules[moduleID]);

		if (nodes.contains(address)) {
			return nodes.at(address).next().target;
		}

	}

	const wchar_t* module = (moduleID < ModuleCount) ? ModuleNames[moduleID] : L"<unknown>";

	FtGetTextDevice()->Logf(FTextDevice::FMT_CRITICAL, L"Failed to get target of unexisting hook %ls!0x%08x\n", module, address);

	return nullptr;

}


size_t Loader::appendHooks(const Mod& mod) {

	struct EnumerateContext {

		const Mod& mod;
		Loader& loader;

		size_t count;

	};

	EnumerateContext context(mod, *this, 0);

	DetourEnumerateExports(mod.info->module, &context, [](PVOID pContext, ULONG nOrdinal, LPCSTR pszName, PVOID pCode) -> BOOL {

		auto* context = static_cast<EnumerateContext*>(pContext);


		uintptr_t address;
		unsigned int moduleID;

		int count = std::sscanf(pszName, "%*[^$]$$anfora_hook$%x$%u$$%*s", &address, &moduleID);

		if (count < 2) {
			return true;
		}


		if (moduleID >= ModuleCount) {

			FtGetTextDevice()->Logf(FTextDevice::FMT_WARNING, L"Hook <unknown>!%08X is invalid ('%ls')\n", address, context->mod.info->name);

			return true;

		}


		address += reinterpret_cast<uintptr_t>(context->loader.modules[moduleID]);

		Hook hook(reinterpret_cast<void*>(address), pCode, context->mod.info->module);

		context->loader.nodes[address].append(hook);

		context->count++;

		return true;

	});

	return context.count;

}

bool Loader::attachHooks() {

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	for (auto& [target, node] : nodes) {

		FtGetTextDevice()->Logf(FTextDevice::FMT_INIT, L"Attaching node 0x%08X\n", target);

		node.sort();

		for (auto& hook : node.hooks) {

			if (DetourAttach(&hook.target, hook.function) == NO_ERROR) {
				continue;
			}

			FtGetTextDevice()->Logf(FTextDevice::FMT_CRITICAL, L"Failed to attach hook (source '%ls')\n", target, mods[hook.source].info->name);

			DetourTransactionAbort();

			return false;

		}

	}

	if (DetourTransactionCommit() != NO_ERROR) {

		FtGetTextDevice()->Log(FTextDevice::FMT_CRITICAL, L"Failed to commit detour transaction\n");

		return false;

	}

	return true;

}

void Loader::detachHooks() {

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	for (auto& [target, node] : nodes) {

		FtGetTextDevice()->Logf(FTextDevice::FMT_INIT, L"Detaching from target 0x%08X\n", target);

		for (auto& hook : node.hooks) {
			DetourDetach(&hook.target, hook.function);
		}

	}

	DetourTransactionCommit();

}


void Loader::freeLibrary(HMODULE module) {

	if (FreeLibrary(module)) {
		return;
	}

	FtGetTextDevice()->Log(FTextDevice::FMT_ERROR, L"Failed to free library, attempting unmap\n");

	if (!UnmapViewOfFile(module)) {
		FtGetTextDevice()->Log(FTextDevice::FMT_ERROR, L"Failed to unmap library\n");
	}

}
