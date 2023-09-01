#include "anforaapi.hpp" // Anfora API to create hooks and define required data

#include "faktum/faktum.hpp" // Includes all Faktum headers at once
#include "cm2/cm2.hpp" // Includes all CM2 headers at once


/*
	anfora_mod(<Name>, <Description>);
	<Name>: Mod name (const wchar_t*)
	<Description>: Mod description (const wchar_t*)

	Strictly required to create a mod
	Defines and exports internal symbols, make sure to use it in a single .cpp file in your project
*/
anfora_mod(L"Example Mod", L"This is the mod description");

/*
	(bool) anfora_init { <Code> }

 	`anfora_init` is executed upon mod initialization, before hooks are attached
	Returning `true` tells to continue loading the mod normally
	Returning `false` causes Anfora to stop loading the mod (hooks will not be attached and the library will be unloaded)
	Global objects construction, mod config initialization and such should be done here
*/
anfora_init {

	FtGetTextDevice()->Log(FTextDevice::Init, L"Hello Anfora!\n");

	return true;

}

/*
	(void) anfora_exit { <Code> }

 	`anfora_exit` is executed upon mod destruction, after hooks are detached
	Mod destruction also happens `anfora_init` returns false
	Global objects destruction and such should be done here
*/
anfora_exit {

	FtGetTextDevice()->Log(FTextDevice::Exit, L"Bye Anfora!\n");

}

/*
	anfora_export

	Attribute used to export hooks, semantics follow `__declspec(dllexport)`
*/
/*
	anfora_hook(<Name>, <Address>, <Module>)
	<Name>: Acts as annotation but affects the symbol
	<Address>: Target function address
	<Module>: Target module (either `cm2` or `faktum`)

	Use over the standard function name to define a hook function
	A hook function will be called instead of the target function when it gets called
	To call the original function back use `anfora_target` or `anfora_raw_target` depending on your needs
*/
anfora_export BOOL __fastcall anfora_hook(CM2GameInit, 0x213D0, cm2)(CM2Game* self, void* edx) {

	FtGetTextDevice()->Log(FTextDevice::Raw, L"Initializing CM2Game\n");

	/*
		anfora_target(<Function>, <Address>, <Module>, <Arguments...>)
		<Function>: Address of original function
		<Address>: Target function address (same as current hook)
		<Module>: Target module (same as current hook)
		<Arguments...>: Call arguments

		Calls the original function that the hook with the params specified replace
	*/
	return anfora_target(&CM2Game::Init, 0x213D0, cm2, self);

}


anfora_export void __fastcall anfora_hook(CM2RendererInit, 0x397F0, cm2)(void* self, void* edx, FGame* game) {

	FtGetTextDevice()->Log(FTextDevice::Raw, L"Initializing CM2Renderer\n");

	/*
		anfora_raw_target(<Type>, <Address>, <Module>, <Arguments...>)
		<Type>: Raw original function type
		<Address>: Target function address (same as current hook)
		<Module>: Target module (same as current hook)
		<Arguments...>: Call arguments

		Same as `anfora_target` but with raw function type
		Useful when the original function is not in the reference
	*/
	anfora_raw_target(void(__thiscall*)(void* self, FGame* game), 0x397F0, cm2, self, game);

}
