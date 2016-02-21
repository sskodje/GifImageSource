#pragma once
#include "pch.h"
#include <shcore.h>
#include <chrono>
namespace Utilities
{
	template <typename Func>
	static void ui_task(Windows::UI::Core::CoreDispatcher^ dispatcher, Func func)
	{
		dispatcher->RunAsync(CoreDispatcherPriority::Normal,ref new Windows::UI::Core::DispatchedHandler([=]()
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

	template <typename Func>
	static void timed_task(Platform::String^ name, Func func)
	{

#if DEBUG
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
				func();
#if DEBUG
				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(t2 - t1).count();
				OutputDebugString(("Duration for "+ name +": "+ duration + "ms\r\n")->Data());
#endif
		
	}

	static int16 ReadIntFromStream(IStream *stream, uint32_t count)
	{
		const int16 PE_POINTER_OFFSET = 60;
		auto bytes = std::unique_ptr<char[]>(new char[count]);
		ULONG* bytesRead = 0;
		stream->Read(bytes.get(), count, bytesRead);
		int16 head_addr = *reinterpret_cast<int16*>(bytes.get());
		return head_addr;
	}
	static std::string ReadStringFromStream(IStream *stream, uint32_t count)
	{
		auto bytes = std::unique_ptr<char[]>(new char[count]);
		ULONG* bytesRead = 0;
		stream->Read(bytes.get(), count, bytesRead);
		return std::string(bytes.get(), count);
	}

	static std::wstring RemoveForbiddenChar(std::wstring s)
	{
		const std::string illegalChars = "\\/:?\"<>|";
		std::wstring::iterator it;
		for (it = s.begin(); it < s.end(); ++it) {
			bool found = illegalChars.find(*it) != std::string::npos;
			if (found) {
				*it = '_';
			}
		}
		return s;
	}
}

