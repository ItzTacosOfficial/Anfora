#include "loader.hpp"

#include "anforaapi.hpp"
#include "hook.hpp"
#include "mod.hpp"

#include <filesystem>


Work work;


__declspec(dllexport) bool _AnfInit();
__declspec(dllexport) void _AnfExit();

static void _AnfFreeLibrary(HMODULE module);


static constexpr const wchar_t* LogSeparator = L"================================================================================\n";


__declspec(dllexport) bool _AnfInit() {

	FtGetTextDevice()->Log(FTextDevice::Raw, LogSeparator);
	FtGetTextDevice()->Log(FTextDevice::Init, L"Initializing Anfora\n");


	// Get all hookable modules handles and base addresses (which in this case are the same, but kept separate for consistency)

	FtGetTextDevice()->Log(FTextDevice::Init, L"Loading modules information\n");

	for (int i = 0; i < ModulesCount; i++) {

		HMODULE handle = GetModuleHandleW(ModulesNames[i]);

		if (!handle) {

			FtGetTextDevice()->Logf(FTextDevice::Critical, L"Failed to get module handle of '%s'\n", ModulesNames[i]);

			return false;

		}

		work.modulesHandles[i] = handle;
		work.baseAddresses[i] = reinterpret_cast<uintptr_t>(handle);

	}


	// Create mods folder if missing

	std::filesystem::path modsDir("./mods/");

	if (!std::filesystem::exists(modsDir)) {

		FtGetTextDevice()->Log(FTextDevice::Init, L"First time startup, no mods to load\n");

		if (!std::filesystem::create_directory(modsDir)) {

			FtGetTextDevice()->Log(FTextDevice::Critical, L"Failed to create mods folder\n");

			return false;

		}

		FtGetTextDevice()->Log(FTextDevice::Init, L"Anfora initialized\n");
		FtGetTextDevice()->Log(FTextDevice::Raw, LogSeparator);

		return true;

	}


	// Load mods dlls, get core symbols (mod init, mod exit, mod module, mod info) and get all hooks
	// All loaded mods get added to the work.mods map with the module handle as key

	FtGetTextDevice()->Log(FTextDevice::Init, L"Begin mods loading\n");

	for (const auto& entry : std::filesystem::directory_iterator(modsDir)) {

		if (!entry.is_regular_file()) {
			continue;
		}

		const auto& path = entry.path();


		FtGetTextDevice()->Logf(FTextDevice::Init, L"Loading '%ls'\n", path.wstring().c_str());

		HMODULE module = LoadLibraryW(path.wstring().c_str());

		if (!module) {

			FtGetTextDevice()->Log(FTextDevice::Error, L"Failed to load mod DLL\n");

			continue;

		}


		Mod mod;

		if (!mod.load(module)) {

			_AnfFreeLibrary(module);

			for (const auto& pair : work.mods) {

				_AnfFreeLibrary(pair.first);

			}

			return false;

		}

		work.mods[module] = mod;

	}


	// Initialize mods by calling their init function (if exists), if one fails unload the mod

	FtGetTextDevice()->Log(FTextDevice::Init, L"Initializing mods\n");

	for (const auto& [module, mod] : work.mods) {

		FtGetTextDevice()->Logf(FTextDevice::Init, L"Initializing '%ls'\n", mod.info->name);

		if (!mod.initMod()) {

			FtGetTextDevice()->Logf(FTextDevice::Error, L"Failed to initialize '%ls'\n", mod.info->name);

			mod.exitMod();

			_AnfFreeLibrary(module);

			work.failedMods.push_back(module);

		}

	}

	if (!work.failedMods.empty()) {
		FtGetTextDevice()->Logf(FTextDevice::Warning, L"%i Mods failed initialization\n", work.failedMods.size());
	}

	for (const auto& module : work.failedMods) {
		work.mods.erase(module);
	}


	FtGetTextDevice()->Log(FTextDevice::Init, L"Attaching hooks\n");

	for (auto& [module, mod] : work.mods) {

		FtGetTextDevice()->Logf(FTextDevice::Init, L"Attaching '%ls'\n", mod.info->name);

		if (!mod.attachHooks()) {
			return false;
		}

	}


	FtGetTextDevice()->Log(FTextDevice::Init, L"Anfora initialized\n");
	FtGetTextDevice()->Log(FTextDevice::Raw, LogSeparator);

	return true;

}

__declspec(dllexport) void _AnfExit() {

	FtGetTextDevice()->Log(FTextDevice::Raw, LogSeparator);
	FtGetTextDevice()->Log(FTextDevice::Exit, L"Shutting down Anfora\n");


	FtGetTextDevice()->Log(FTextDevice::Exit, L"Detaching hooks\n");

	for (auto& [module, mod] : work.mods) {

		FtGetTextDevice()->Logf(FTextDevice::Exit, L"Detaching '%ls'\n", mod.info->name);

		mod.detachHooks();

	}


	FtGetTextDevice()->Log(FTextDevice::Exit, L"Exiting mods\n");

	for (auto& [module, mod] : work.mods) {

		FtGetTextDevice()->Logf(FTextDevice::Exit, L"Exiting '%ls'\n", mod.info->name);

		mod.exitMod();

	}


	FtGetTextDevice()->Log(FTextDevice::Exit, L"Unloading mods\n");

	for (auto& [module, mod] : work.mods) {

		FtGetTextDevice()->Logf(FTextDevice::Exit, L"Unloading '%ls'\n", mod.info->name);

		_AnfFreeLibrary(module);

	}

	work.mods.clear();


	FtGetTextDevice()->Log(FTextDevice::Exit, L"Anfora shut down\n");
	FtGetTextDevice()->Log(FTextDevice::Raw, LogSeparator);

}


static void _AnfFreeLibrary(HMODULE module) {

	if (!FreeLibrary(module)) {

		FtGetTextDevice()->Log(FTextDevice::Error, L"Failed to free library, attempting unmap\n");

		if (!UnmapViewOfFile(module)) {

			FtGetTextDevice()->Log(FTextDevice::Error, L"Failed to unmap library\n");

			// Nothing can be done at this point

		}

	}

}
