#include "anfora/api.hpp" // Anfora API

#include "faktum/faktum.hpp" // Includes all Faktum headers at once
#include "cm2/cm2.hpp" // Includes all CM2 headers at once


/*
	anfora_mod(<Name>, <Description>);
	Name:			Mod name (const wchar_t*)
	Description:	Mod description (const wchar_t*)

	Defines core mod information

	Must be used once in a single translation unit to make the mod loadable
*/
anfora_mod(L"Example Mod", L"This is the mod description");


/*
	(bool) anfora_init() { ... }

 	Optional mod initialization function that is executed before hooks installation

	Returning `false` causes Anfora to destruct the mod and unload its library

	Global objects construction, mod config management and such should be done here
*/
anfora_init() {

	FtGetTextDevice()->Log(FTextDevice::FMT_INIT, L"Hello Anfora!\n");

	return true;

}

/*
	(void) anfora_exit() { ... }

 	Optional mod destruction function that is executed after hooks are detached and on mod initialization failure

	Global objects destruction and such should be done here
*/
anfora_exit() {

	FtGetTextDevice()->Log(FTextDevice::FMT_EXIT, L"Bye Anfora!\n");

}


/*
	anfora_export

	Alias to `__declspec(novtable)`

	Useful for hook classes
*/
class anfora_novtable HookCM2Game : public CM2Game {

public:

	/*
		anfora_export

		Alias to `__declspec(dllexport)`
	*/
	/*
		... anfora_hook(<Name>, <Address>, <Module>, <Arguments...>) { ... }
		Name:		Name of the hook (to avoid conflicts)
		Address:	Target function address
		Module:		Target module (`cm2` or `faktum`)
		Arguments:	Function arguments

		Defines a function hook that will be called over the target function
	*/
	anfora_export BOOL __fastcall anfora_hook(Init, 0x213D0, cm2) {

		FtGetTextDevice()->Log(FTextDevice::FMT_ABOVE_WARNING, L"Initializing CM2Game\n");

		/*
			anfora_target(<Function>, <Address>, <Module>, <Arguments...>)
			Function:	Address of target function
			Address:	Target function address
			Module:		Target module
			Arguments:	Call arguments

			Calls the original function that the hook with the params specified replaced

			In case of overlapping hooks, the next function hook with the same target and module in the node will be executed
		*/
		return anfora_target(&CM2Game::Init, 0x213D0, cm2, this);

	}

};

/*
	... anfora_uhook(<Address>, <Module>, <Arguments...>) { ... }

	Same as `anfora_hook` but the function is unnamed (might cause conflicts)
*/
anfora_export void __fastcall anfora_uhook(0x397F0, cm2, void* self, void* edx, FGame* game) {

	FtGetTextDevice()->Log(FTextDevice::FMT_ABOVE_WARNING, L"Initializing CM2Renderer\n");

	/*
		anfora_raw_target(<Type>, <Address>, <Module>, <Arguments...>)

		Acts the same as `anfora_target` but with raw target function type is specified

		Useful for undefined functions
	*/
	anfora_raw_target(void(__thiscall*)(void* self, FGame* game), 0x397F0, cm2, self, game);

}
