#pragma once

#include "faktum/faktum.hpp"


#ifdef __ANFORA_API_EXPORT
#define __ANFORA_API __declspec(dllexport)
#else
#define __ANFORA_API __declspec(dllimport)
#endif


#define __ANFORA_STR(x) __ANFORA_STRX(x)
#define __ANFORA_STRX(x) #x

#define __ANFORA_CAT(a, b) __ANFORA_CATX(a, b)
#define __ANFORA_CATX(a, b) a##b

#define __ANFORA_MODULEID_faktum	0
#define __ANFORA_MODULEID_cm2		1

#define __ANFORA_MODULEID($module) __ANFORA_CAT(__ANFORA_MODULEID_, $module)


#define __ANFORA_SYM_INIT __anforaModInit
#define __ANFORA_SYM_EXIT __anforaModExit
#define __ANFORA_SYM_INFO __anforaInfo


namespace Anfora {

	namespace Detail {

		template<class T>
		struct RawFunctionType;

		template<class R, class T, class... Args>
		struct RawFunctionType<R(__thiscall T::*)(Args...)> {
			using Type = R(__thiscall*)(T*, Args...);
		};

		template<class R, class T, class... Args>
		struct RawFunctionType<R(__cdecl T::*)(Args...)> {
			using Type = R(__cdecl*)(T*, Args...);
		};

		template<class R, class... Args>
		struct RawFunctionType<R(*)(Args...)> {
			using Type = R(*)(Args...);
		};

	}

	template<class T>
	using RawFunctionType = typename Detail::RawFunctionType<T>::Type;


	struct ModInfo {

		constexpr ModInfo(const wchar_t* name, const wchar_t* description) : name(name), description(description), module(nullptr) {}


		const wchar_t* name;
		const wchar_t* description;

		HMODULE module;

	};


	namespace Internal {

#ifdef __ANFORA_API_INTERNAL

		static constexpr const char* InitSymbol = __ANFORA_STR(__ANFORA_SYM_INIT);
		static constexpr const char* ExitSymbol = __ANFORA_STR(__ANFORA_SYM_EXIT);
		static constexpr const char* InfoSymbol = __ANFORA_STR(__ANFORA_SYM_INFO);

		__ANFORA_API bool initialize(bool* startVanilla);

		__ANFORA_API void exit();

#endif

		__ANFORA_API void* getHookTarget(const ModInfo& info, uintptr_t address, unsigned int module);

	}

}


#define __ANFORA_MOD_API __declspec(dllexport)
#define __ANFORA_MOD_DECL extern "C" __ANFORA_MOD_API


#define anfora_novtable __declspec(novtable)

#define anfora_export __ANFORA_MOD_API

#define anfora_init \
__ANFORA_MOD_DECL bool __ANFORA_SYM_INIT /* ( ... ) { ... } */

#define anfora_exit \
__ANFORA_MOD_DECL void __ANFORA_SYM_EXIT /* () { ... } */

#define anfora_mod($name, $description) \
__ANFORA_MOD_DECL Anfora::ModInfo __ANFORA_SYM_INFO($name, $description) /* ; */

#define anfora_hook($name, $address, $module, ...) \
__ANFORA_CAT(__ANFORA_CAT($$anfora_hook$ ## $address ## $, __ANFORA_MODULEID($module)), $$ ## $name)(__VA_ARGS__)

#define anfora_uhook($address, $module, ...) \
anfora_hook(, $address, $module, ## __VA_ARGS__)

#define anfora_raw_target($type, $address, $module, ...) \
(reinterpret_cast<$type>(Anfora::Internal::getHookTarget(__ANFORA_SYM_INFO, $address, __ANFORA_MODULEID($module))))(__VA_ARGS__)

#define anfora_target($name, $address, $module, ...) \
anfora_raw_target(Anfora::RawFunctionType<decltype($name)>, $address, $module, ## __VA_ARGS__)


#ifndef __ANFORA_API_INTERNAL

__ANFORA_MOD_DECL Anfora::ModInfo __anforaInfo;


namespace Anfora {

	constexpr const ModInfo& getModInfo() {
		return __anforaInfo;
	}

}

#endif
