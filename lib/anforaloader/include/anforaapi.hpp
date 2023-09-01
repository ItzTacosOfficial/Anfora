#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>


#define _ANFORA_MODULEID_faktum		0
#define _ANFORA_MODULEID_cm2		1

#define _ANFORA_CATX(a, b) a##b
#define _ANFORA_CAT(a, b) _ANFORA_CATX(a, b)

#define _ANFORA_MODULEID($module) _ANFORA_CAT(_ANFORA_MODULEID_, $module)


namespace Detail {

	template<class T>
	struct FunctionPointerHelperT {
		static_assert((sizeof(T), true), "Anfora: anfora_target invalid symbol name");
	};

	template<class R, class T, class... Args>
	struct FunctionPointerHelperT<R(__thiscall T::*)(Args...)> {
		using Type = R(__thiscall*)(T*, Args...);
	};

	template<class R, class T, class... Args>
	struct FunctionPointerHelperT<R(__cdecl T::*)(Args...)> {
		using Type = R(__cdecl*)(T*, Args...);
	};

	template<class R, class... Args>
	struct FunctionPointerHelperT<R(*)(Args...)> {
		using Type = R(*)(Args...);
	};


	template<class T>
	using FunctionPointerHelper = typename FunctionPointerHelperT<T>::Type;

}


struct AnforaModInfo {
	const wchar_t* name;
	const wchar_t* description;
};


#ifdef _ANFORA_API_EXPORT
	#define _ANFAPI __declspec(dllexport)
#else
	#define _ANFAPI __declspec(dllimport)
#endif

_ANFAPI void* _AnfGetHookTarget(HMODULE modModule, uintptr_t addr, unsigned int moduleID);



#define anfora_export __declspec(dllexport)

#define _ANFORA_DECL extern "C" anfora_export


// Define mod initialization statement
#define anfora_init _ANFORA_DECL bool _AnModInit()

// Define mod destruction statement
#define anfora_exit _ANFORA_DECL void _AnModExit()

// Define mod essentials
#define anfora_mod($name, $description) \
_ANFORA_DECL HMODULE _AnModModule = nullptr; \
_ANFORA_DECL AnforaModInfo _AnModInfo = {$name, $description}

// Make hook function name
#define anfora_hook($name, $addr, $module) \
_ANFORA_CAT(_ANFORA_CAT($$anfora_hook$##$addr##$, _ANFORA_MODULEID($module)), $$)

// Get target of hook with raw type
#define anfora_raw_target($type, $addr, $module, ...) \
(reinterpret_cast<$type>(_AnfGetHookTarget(_AnModModule, $addr, _ANFORA_MODULEID($module)))(__VA_ARGS__))

// Get target of hook from original symbol
#define anfora_target($name, $addr, $module, ...) \
anfora_raw_target(Detail::FunctionPointerHelper<decltype($name)>, $addr, $module, ## __VA_ARGS__)


#ifndef ANFORA_API_NODOLLARSIGN

	#define $init		anfora_init
	#define $exit		anfora_exit
	#define $mod		anfora_mod
	#define $hook		anfora_hook
	#define $rawtarget	anfora_raw_target
	#define $target		anfora_target

#endif
