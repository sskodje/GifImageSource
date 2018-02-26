#pragma once
#include "pch.h"
#include <shcore.h>
#include <chrono>
#include "IEffectDescription.h"

namespace Utilities
{
	enum ImageFileType
	{
		IMAGE_FILE_JPG,      // joint photographic experts group - .jpeg or .jpg
		IMAGE_FILE_PNG,      // portable network graphics
		IMAGE_FILE_GIF,      // graphics interchange format 
		IMAGE_FILE_TIFF,     // tagged image file format
		IMAGE_FILE_BMP,      // Microsoft bitmap format
		IMAGE_FILE_WEBP,     // Google WebP format, a type of .riff file
		IMAGE_FILE_ICO,      // Microsoft icon format
		IMAGE_FILE_INVALID,  // unidentified image types.
	};
	static ImageFileType getImageTypeByMagic(const char* data)
	{
		//if (len < 16) return IMAGE_FILE_INVALID;

		// .jpg:  FF D8 FF
		// .png:  89 50 4E 47 0D 0A 1A 0A
		// .gif:  GIF87a      
		//        GIF89a
		// .tiff: 49 49 2A 00
		//        4D 4D 00 2A
		// .bmp:  BM 
		// .webp: RIFF ???? WEBP 
		// .ico   00 00 01 00
		//        00 00 02 00 ( cursor files )

		switch (data[0])
		{
		case '\xFF':
			return (!strncmp((const char*)data, "\xFF\xD8\xFF", 3)) ?
				IMAGE_FILE_JPG : IMAGE_FILE_INVALID;

		case '\x89':
			return (!strncmp((const char*)data,
				"\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8)) ?
				IMAGE_FILE_PNG : IMAGE_FILE_INVALID;

		case 'G':
			return (!strncmp((const char*)data, "GIF87a", 6) ||
				!strncmp((const char*)data, "GIF89a", 6)) ?
				IMAGE_FILE_GIF : IMAGE_FILE_INVALID;

		case 'I':
			return (!strncmp((const char*)data, "\x49\x49\x2A\x00", 4)) ?
				IMAGE_FILE_TIFF : IMAGE_FILE_INVALID;

		case 'M':
			return (!strncmp((const char*)data, "\x4D\x4D\x00\x2A", 4)) ?
				IMAGE_FILE_TIFF : IMAGE_FILE_INVALID;

		case 'B':
			return ((data[1] == 'M')) ?
				IMAGE_FILE_BMP : IMAGE_FILE_INVALID;

		case 'R':
			if (strncmp((const char*)data, "RIFF", 4))
				return IMAGE_FILE_INVALID;
			if (strncmp((const char*)(data + 8), "WEBP", 4))
				return IMAGE_FILE_INVALID;
			return IMAGE_FILE_WEBP;

		case '\0':
			if (!strncmp((const char*)data, "\x00\x00\x01\x00", 4))
				return IMAGE_FILE_ICO;
			if (!strncmp((const char*)data, "\x00\x00\x02\x00", 4))
				return IMAGE_FILE_ICO;
			return IMAGE_FILE_INVALID;

		default:
			return IMAGE_FILE_INVALID;
		}
	}

	template <typename Func>
	static void ui_task(Windows::UI::Core::CoreDispatcher^ dispatcher, Func func)
	{
		dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([=]()
		{
			try
			{
				func();
			}
			catch (...)
			{
				OutputDebugString(L"unknown error in ui_task\r\n");
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
		OutputDebugString(("Duration for " + name + ": " + duration + "ms\r\n")->Data());
#endif

	}

	static int16 ReadIntFromStream(IStream *stream, uint32_t count)
	{
		const int16 PE_POINTER_OFFSET = 60;
		int16 width;
		ULONG* bytesRead = 0;
		stream->Read(reinterpret_cast<int16*>(&width), count, bytesRead);
		return width;
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

	static bool IsLoaded(Windows::UI::Xaml::FrameworkElement^ element)
	{
		return Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(element) != nullptr;
	}

}

