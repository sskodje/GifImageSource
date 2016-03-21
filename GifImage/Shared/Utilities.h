#pragma once
#include "pch.h"
#include <shcore.h>
#include <chrono>

//using namespace Platform;
//using namespace  Windows::Security::Cryptography::Core;
//using namespace Windows::Security::Cryptography;
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

	static std::wstring StringToWideString(const std::string& s) {
		int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length(), NULL, 0);
		std::wstring ws(L"", len);
		wchar_t* pWSBuf = const_cast<wchar_t*>(ws.c_str());
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, pWSBuf, len);
		return ws;
	}

	static std::string WideStringToString(const std::wstring& ws) {
		int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), ws.length(), 0, 0, NULL, NULL);
		std::string r("", len);
		char* pRBuf = const_cast<char*>(r.c_str());
		WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), ws.length(), pRBuf, len, NULL, NULL);
		return r;
	}

	static Platform::String^ LPSTRToPlatformString(const char* lpstr) {
		if (lpstr == nullptr || strlen(lpstr) == 0)
			return ref new Platform::String();

		int len = MultiByteToWideChar(CP_UTF8, 0, lpstr, strlen(lpstr), NULL, 0);
		auto ps = ref new Platform::String(L"", len);
		wchar_t* pPSBuf = const_cast<wchar_t*>(ps->Data());
		MultiByteToWideChar(CP_UTF8, 0, lpstr, -1, pPSBuf, len);
		return ps;
	}

	static Platform::String^ StringToPlatformString(const std::string& s) {
		if (s.empty())
			return ref new Platform::String();

		int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length(), NULL, 0);
		auto ps = ref new Platform::String(L"", len);
		wchar_t* pPSBuf = const_cast<wchar_t*>(ps->Data());
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, pPSBuf, len);
		return ps;
	}

	static std::string PlatformStringToString(Platform::String^ ps) {
		int len = WideCharToMultiByte(CP_UTF8, 0, ps->Data(), ps->Length(), 0, 0, NULL, NULL);
		std::string r("", len);
		char* pRBuf = const_cast<char*>(r.c_str());
		WideCharToMultiByte(CP_UTF8, 0, ps->Data(), ps->Length(), pRBuf, len, NULL, NULL);
		return r;
	}

	static std::string GetFileExtension(std::string file) {

		std::size_t found = file.find_last_of(".");
		std::string ext = file.substr(found);
		return ext;
	}

	 static Platform::String^ GetCacheFileName(Platform::String^ str)
	{
		auto provider = Windows::Security::Cryptography::Core::HashAlgorithmProvider::OpenAlgorithm(Windows::Security::Cryptography::Core::HashAlgorithmNames::Md5);
		Windows::Security::Cryptography::Core::CryptographicHash^ objHash = provider->CreateHash();
		Windows::Storage::Streams::IBuffer^ buffMsg1 = Windows::Security::Cryptography::CryptographicBuffer::ConvertStringToBinary(str, Windows::Security::Cryptography::BinaryStringEncoding::Utf8);
		objHash->Append(buffMsg1);
		Windows::Storage::Streams::IBuffer^ buffHash1 = objHash->GetValueAndReset();
		return Windows::Security::Cryptography::CryptographicBuffer::EncodeToHexString(buffHash1);
	}
}

