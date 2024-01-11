#define __ANFORA_API_INTERNAL
#define __ANFORA_API_EXPORT
#include "anfora/api.hpp"


#include "loader.hpp"

#include <string>


static constexpr const wchar_t* LogSeparator = L"================================================================================\n";


static Loader loader;


static int handleFatalError(const std::wstring& text = {}, unsigned int button = MB_OK) {

	constexpr const wchar_t* Title = L"Anfora ModLoader";
	constexpr const wchar_t* Message = L"Anfora caught a fatal error\nCheck cm2.log for further information";

	std::wstring message = Message;

	if (!text.empty()) {
		message += L"\n\n" + text;
	}

	return MessageBoxW(nullptr, message.c_str(), Title, button | MB_ICONERROR);

}


// Launcher API

__ANFORA_API bool Anfora::Internal::initialize(bool *startVanilla) {

	FtGetTextDevice()->Log(FTextDevice::FMT_ABOVE_WARNING, LogSeparator);

	bool success = loader.init() && loader.install();

	FtGetTextDevice()->Log(FTextDevice::FMT_ABOVE_WARNING, LogSeparator);

	if (success) {
		return true;
	}

	int result = handleFatalError(L"Do you wish to start the vanilla game?", MB_YESNO);

	*startVanilla = result == IDYES;

	return false;

}

__ANFORA_API void Anfora::Internal::exit() {

	FtGetTextDevice()->Log(FTextDevice::FMT_ABOVE_WARNING, LogSeparator);

	loader.exit();

	FtGetTextDevice()->Log(FTextDevice::FMT_ABOVE_WARNING, LogSeparator);

}


// Mod API

__ANFORA_API void* Anfora::Internal::getHookTarget(const Anfora::ModInfo& info, uintptr_t address, unsigned int moduleID) {

	void* target = loader.getHookTarget(info, address, moduleID);

	if (target) {
		return target;
	}

	handleFatalError();

	std::abort();

}
