#define __ANFORA_API_INTERNAL
#include "anfora/api.hpp"

#include "faktum/faktum.hpp"
#include "cm2/cm2.hpp"

#include <cfloat>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	CreateMutexW(nullptr, true, L"CM2-2498A7E0-4306-4C1B-A0F2-A424BAEEE5D9");

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return 0;
	}


	FMemManagerWin32 memManager;
	FTextDeviceFile textDevice(L"cm2.log");
	FConfigINI config(L"cm2.ini");


	const wchar_t* args = GetCommandLineW();

	FtInit(args, &memManager, &textDevice, nullptr, &config);


	bool vanilla;

	bool anfora = Anfora::Internal::initialize(&vanilla);

	if (!anfora && !vanilla) {

		FtExit();

		return 2;

	}


	_controlfp(_DN_SAVE, _MCW_PC);


	auto* game = new (nullptr, "cm2", FName::UNNAMED, 0) CM2Game();

	int result = game->Run();

	game->Release();


	if (anfora) {
		Anfora::Internal::exit();
	}

	FtExit();


	return result;

}
