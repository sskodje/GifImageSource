#pragma once
#include "pch.h"
#include <shcore.h>
namespace Utilities
{
	template <typename Func>
	static void ui_task(Windows::UI::Core::CoreDispatcher^ dispatcher, Func func)
	{
		dispatcher->RunIdleAsync(ref new Windows::UI::Core::IdleDispatchedHandler([=](Windows::UI::Core::IdleDispatchedHandlerArgs^ args)
		{
			try
			{
				func();
			}
			catch (...)
			{
				OutputDebugString(L"unknown error in delayed_ui_task");
			}
		}, Platform::CallbackContext::Any));
	}

	static std::string ReadStringFromStream(IStream *stream, uint32_t count)
	{
		auto bytes = std::unique_ptr<char[]>(new char[count]);
		ULONG* bytesRead = 0;
		stream->Read(bytes.get(), count, bytesRead);

		return std::string(bytes.get(), count);
	}
};

