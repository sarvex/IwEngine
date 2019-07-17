#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Keyboard.h"

namespace IwInput {
	class WindowsRawKeyboard
		: public RawKeyboard
	{
	public:
		WindowsRawKeyboard(
			InputCallback& callback);

		void HandleEvent(
			OsEvent& event);
	};
}
#endif
