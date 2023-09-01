
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define _CRT_SECURE_NO_WARNINGS // _controlfp
#include <cfloat>

#include "faktum/faktum.hpp"
#include "cm2/cm2.hpp"


__declspec(dllimport) bool _AnfInit();
__declspec(dllimport) void _AnfExit();


static int Cm2Main() {

	FMemManagerWin32 memManager;
	FTextDeviceFile textDevice(L"cm2.log");
	FConfigINI config(L"cm2.ini");

	const wchar_t* args = GetCommandLineW();

	FtInit(args, &memManager, &textDevice, nullptr, &config);

	bool anfora = _AnfInit();

	if (!anfora) {

		constexpr const wchar_t* MsgBoxText = L"Anfora caught a fatal error on startup\nCheck cm2.log for further information\n\nDo you wish to play the vanilla game anyways?";
		constexpr const wchar_t* MsgBoxTitle = L"Anfora Modloader";

		int result = MessageBoxW(nullptr, MsgBoxText, MsgBoxTitle, MB_YESNO | MB_ICONERROR);

		if (result == IDNO) {

			FtExit();

			return 1;

		}

	}

	// I honestly have no clue why one would do that, but the original game does so
	_controlfp(0, 0x30000);

	auto* game = static_cast<CM2Game*>(CM2Game::operator new(500, nullptr, "cm2", FName::NAME(-1), 0));
	game->CM2Game::CM2Game();

	int result = game->Run();

	game->Release();

	_AnfExit();

	FtExit();

	return result;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	CreateMutexW(nullptr, true, L"CM2-2498A7E0-4306-4C1B-A0F2-A424BAEEE5D9");

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return 0;
	}

	return Cm2Main();

}
