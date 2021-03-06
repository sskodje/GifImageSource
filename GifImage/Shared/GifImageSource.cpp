﻿#include <pch.h>
#include <ppl.h>
#include <shcore.h>
#include "GifImageSource.h"
#include "Utilities.h"
#include "IEffectDescription.h"
#include <windows.foundation.h>

using namespace std;
using namespace std::chrono;
using namespace GifImage;
using namespace Platform;
using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml::Media::Animation;

bool GifImageSource::s_isAllAnimationsPaused;

template <typename T>
inline void SafeRelease(T *&pI)
{
	if (nullptr != pI)
	{
		pI->Release();
		pI = nullptr;
	}
}

GifImageSource::GifImageSource(int width, int height, Platform::IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ repeatBehavior)
	: SurfaceImageSource(width, height),
	m_width(width),
	m_height(height),
	m_dwFrameCount(0),
	m_bitmaps(NULL),
	m_realtimeBitmapBuffer(NULL),
	m_offsets(NULL),
	m_delays(NULL),
	m_dwCurrentFrame(0),
	m_dwPreviousFrame(0),
	m_completedLoopCount(0),
	m_bitsPerPixel(8),
	m_canCacheMoreFrames(true),
	m_haveReservedDeviceResources(false),
	m_isRenderingFrame(false),
	m_repeatBehavior(nullptr),
	m_isAnimatedGif(false)
{
	if (width < 0 || height < 0)
		throw ref new Platform::InvalidArgumentException("Height and Width cannot be less than zero");

	m_repeatBehavior = repeatBehavior;
	CreateDeviceResources(false);
	Application::Current->Suspending += ref new SuspendingEventHandler(this, &GifImageSource::OnSuspending);
}

GifImageSource::~GifImageSource()
{
	ClearResources();
	OutputDebugString(L"Destructed GifImageSource\r\n");
}


void GifImageSource::OnPropertyChanged(String^ propertyName)
{
	PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
}

void GifImageSource::ClearResources()
{
	StopDurationTimer();

	if (m_haveReservedDeviceResources)
	{
		Direct2DManager::ReturnInstance(m_windowID);
		m_haveReservedDeviceResources = false;
	}
}


//Renders the frame to screen
bool GifImageSource::RenderFrame()
{
	if (m_effectDescriptions.size() > 0) {
		return RenderFrameWithEffect();
	}
	else {
		return RenderFrameWithoutEffect();
	}
}

//Renders the frame to screen
bool GifImageSource::RenderFrameWithEffect()
{
	auto d2dContext = Direct2DManager::GetInstance(m_windowID)->GetD2DContext();
	//Create first effect in the list
	ComPtr<ID2D1Effect> effect = m_effectDescriptions[0].Apply(d2dContext, effect);

	HRESULT hr = BeginDraw();
	if (SUCCEEDED(hr))
	{
		ComPtr<ID2D1BitmapRenderTarget> bitmapRenderTarget;
		d2dContext->Clear();
		d2dContext->CreateCompatibleRenderTarget(&bitmapRenderTarget);
		bitmapRenderTarget->BeginDraw();
		ComPtr<ID2D1Bitmap> foo;
		hr = bitmapRenderTarget->GetBitmap(&foo);
		if (m_bitmaps.at(CurrentFrame) != nullptr)
		{
			m_pPreviousRawFrame = nullptr;
			for (uint32 i = 0; i < CurrentFrame; i++)
			{
				int disposal = m_disposals[i];
				auto bitmap = m_bitmaps.at(i).Get();
				if (bitmap == nullptr)
				{
					//Something has gone wrong, this should not happen. We reset to first frame and return true for loop complete.
					EndDraw();
					CurrentFrame = 0;
					throw ref new NullReferenceException("frame cannot be null");
				}
				switch (disposal)
				{
				case DISPOSAL_UNSPECIFIED:
				case DISPOSE_DO_NOT:
				{
					effect->SetInput(0, bitmap);
					D2D1_RECT_F rect;
					rect.left = m_offsets.at(i).x;
					rect.top = m_offsets.at(i).y;
					rect.right = bitmap->GetPixelSize().width + m_offsets.at(i).x;
					rect.bottom = bitmap->GetPixelSize().height + m_offsets.at(i).y;
					bitmapRenderTarget->DrawBitmap(bitmap, rect);
					break;
				}
				case DISPOSE_BACKGROUND:
				{
					auto offset = m_offsets.at(i);

					d2dContext->PushAxisAlignedClip(
						D2D1::RectF(
							static_cast<float>(offset.x),
							static_cast<float>(offset.y),
							static_cast<float>(offset.x + bitmap->GetPixelSize().width),
							static_cast<float>(offset.y + bitmap->GetPixelSize().height)
						),
						D2D1_ANTIALIAS_MODE_ALIASED);


					d2dContext->Clear();
					d2dContext->PopAxisAlignedClip();

					break;
				}
				case DISPOSE_PREVIOUS:
				default:
					// We don't need to render the intermediate frame if intIt's not
					// going to update the intermediate buffer anyway.
					break;
				}
			}
		}

		hr = GetRawFrame(CurrentFrame);
		if (hr == E_ABORT)
		{
			OutputDebugString(("Bitmap not decoded, delaying frame " + CurrentFrame + "\r\n")->Data());
			EndDraw();
			return false;
		}

		if (m_pPreviousRawFrame != nullptr)
		{
			bitmapRenderTarget->DrawBitmap(m_pPreviousRawFrame.Get());
		}
		//draw current frame on top
		effect->SetInput(0, m_pRawFrame.Get());
		D2D1_RECT_F rect;
		rect.left = m_offsets.at(CurrentFrame).x;
		rect.top = m_offsets.at(CurrentFrame).y;
		rect.right = m_pRawFrame.Get()->GetPixelSize().width + m_offsets.at(CurrentFrame).x;
		rect.bottom = m_pRawFrame.Get()->GetPixelSize().height + m_offsets.at(CurrentFrame).y;
		bitmapRenderTarget->DrawBitmap(m_pRawFrame.Get(), rect);
		bitmapRenderTarget->Flush();
		bitmapRenderTarget->EndDraw();

		ComPtr<ID2D1Bitmap> bmp;
		bitmapRenderTarget->GetBitmap(&bmp);
		effect->SetInput(0, bmp.Get());

		//Apply the remaining effects
		int i = 0;
		for each (Effect e in m_effectDescriptions)
		{
			if (i > 0) {
				effect = e.Apply(d2dContext, effect);
			}
			i++;
		}

		d2dContext->DrawImage(effect.Get());
		hr = d2dContext->Flush();


		if (CurrentFrame + 1 < m_dwFrameCount&& m_bitmaps.at(CurrentFrame + 1) == nullptr)
		{
			hr = CopyCurrentFrameToBitmap();
		}
		EndDraw();
		m_realtimeBitmapBuffer[CurrentFrame] = nullptr;
		SelectNextFrame();
	}

	// Returns true if we just completed a loop
	return CurrentFrame == 0;
}


//Renders the frame to screen
bool GifImageSource::RenderFrameWithoutEffect()
{
	HRESULT hr = BeginDraw();
	if (SUCCEEDED(hr))
	{
		auto d2dContext = Direct2DManager::GetInstance(m_windowID)->GetD2DContext();
		d2dContext->Clear();

		if (m_bitmaps.at(CurrentFrame) != nullptr)
		{
			m_pPreviousRawFrame = nullptr;
			for (uint32 i = 0; i < CurrentFrame; i++)
			{
				int disposal = m_disposals[i];
				auto bitmap = m_bitmaps.at(i).Get();
				if (bitmap == nullptr)
				{
					//Something has gone wrong, this should not happen. We reset to first frame and return true for loop complete.
					EndDraw();
					CurrentFrame = 0;
					throw ref new NullReferenceException("frame cannot be null");
				}
				switch (disposal)
				{
				case DISPOSAL_UNSPECIFIED:
				case DISPOSE_DO_NOT:
				{
					d2dContext->DrawImage(bitmap, m_offsets.at(i));
					break;
				}
				case DISPOSE_BACKGROUND:
				{
					auto offset = m_offsets.at(i);

					d2dContext->PushAxisAlignedClip(
						D2D1::RectF(
							static_cast<float>(offset.x),
							static_cast<float>(offset.y),
							static_cast<float>(offset.x + bitmap->GetPixelSize().width),
							static_cast<float>(offset.y + bitmap->GetPixelSize().height)
						),
						D2D1_ANTIALIAS_MODE_ALIASED);


					d2dContext->Clear();
					d2dContext->PopAxisAlignedClip();

					break;
				}
				case DISPOSE_PREVIOUS:
				default:
					// We don't need to render the intermediate frame if intIt's not
					// going to update the intermediate buffer anyway.
					break;
				}
			}
		}

		hr = GetRawFrame(CurrentFrame);
		if (hr == E_ABORT)
		{
			OutputDebugString(("Bitmap not decoded, delaying frame " + CurrentFrame + "\r\n")->Data());
			EndDraw();
			return false;
		}

		if (m_pPreviousRawFrame != nullptr)
		{
			d2dContext->DrawImage(m_pPreviousRawFrame.Get());
		}
		//draw current frame on top
		d2dContext->DrawImage(m_pRawFrame.Get(), m_offsets.at(CurrentFrame));
		hr = d2dContext->Flush();


		if (CurrentFrame + 1 < m_dwFrameCount&& m_bitmaps.at(CurrentFrame + 1) == nullptr)
		{
			hr = CopyCurrentFrameToBitmap();
		}
		EndDraw();
		m_realtimeBitmapBuffer[CurrentFrame] = nullptr;
		SelectNextFrame();
	}

	// Returns true if we just completed a loop
	return CurrentFrame == 0;
}

//Save the bitmap to m_pPreviousRawFrame, so we can use intIt as a base frame for the next frame.
//We do this to avoid having to potentially decode and draw hundreds of base frames for each new frame,
//since we need to clear the SurfaceImageSource for each draw operation.
HRESULT GifImageSource::CopyCurrentFrameToBitmap()
{
	auto d2dContext = Direct2DManager::GetInstance(m_windowID)->GetD2DContext();

	HRESULT hr;
	//This is a bitmaprender we use to create the bitmaps off-screen.
	ComPtr<ID2D1BitmapRenderTarget> renderTarget;
	hr = d2dContext->CreateCompatibleRenderTarget(&renderTarget);
	if (FAILED(hr))
	{
		return hr;
	}

	renderTarget->BeginDraw();

	if (m_pPreviousRawFrame != nullptr)
		renderTarget->DrawBitmap(m_pPreviousRawFrame.Get());
	else if (CurrentFrame > 0)
	{
		//In this situation we have missing frames in the cache, so we have to draw each previous frame from 0 to m_dwCurrentFrame
		//in order to make a complete base frame for this gif.
		for (uint32 i = 0; i < CurrentFrame; i++)
		{
			int disposal = m_disposals[i];
			auto offset = m_offsets.at(i);
			auto bitmap = m_bitmaps.at(i).Get();
			if (bitmap == nullptr)
			{
				//Something has gone wrong, this should not happen.
				return E_FAIL;
			}
			auto rect = D2D1::RectF(
				static_cast<float>(offset.x),
				static_cast<float>(offset.y),
				static_cast<float>(offset.x + bitmap->GetSize().width),
				static_cast<float>(offset.y + bitmap->GetSize().height)
			);
			switch (disposal)
			{
			case DISPOSAL_UNSPECIFIED:
			case DISPOSE_DO_NOT:
			{
				renderTarget->DrawBitmap(bitmap, rect);
				break;
			}
			case DISPOSE_BACKGROUND:
			{
				renderTarget->PushAxisAlignedClip(
					rect,
					D2D1_ANTIALIAS_MODE_ALIASED);

				renderTarget->Clear();
				renderTarget->PopAxisAlignedClip();
				break;
			}
			case DISPOSE_PREVIOUS:
			default:
				// We don't need to render the intermediate frame if intIt's not
				// going to update the intermediate buffer anyway.
				break;
			}
		}
	}

	int disposal = m_disposals[CurrentFrame];
	auto offset = m_offsets.at(CurrentFrame);
	auto rect = D2D1::RectF(
		static_cast<float>(offset.x),
		static_cast<float>(offset.y),
		static_cast<float>(offset.x + m_pRawFrame->GetSize().width),
		static_cast<float>(offset.y + m_pRawFrame->GetSize().height)
	);

	switch (disposal)
	{
	case DISPOSAL_UNSPECIFIED:
	case DISPOSE_DO_NOT:
	{
		renderTarget->DrawBitmap(m_pRawFrame.Get(), rect);
		break;
	}
	case DISPOSE_BACKGROUND:
	{

		renderTarget->PushAxisAlignedClip(
			rect,
			D2D1_ANTIALIAS_MODE_ALIASED);


		renderTarget->Clear();
		renderTarget->PopAxisAlignedClip();

		break;
	}
	case DISPOSE_PREVIOUS:
	default:
		// We don't need to render the intermediate frame if intIt's not
		// going to update the intermediate buffer anyway.
		break;
	}
	hr = renderTarget->Flush();
	hr = renderTarget->EndDraw();
	hr = renderTarget->GetBitmap(&m_pPreviousRawFrame);
	return hr;
}



//Sets the value of m_pRawFrame for the current frame index, and starts the GetRawFramesTask if necessary.
HRESULT GifImageSource::GetRawFrame(int uFrameIndex)
{
	bool startPreRenderTask = false;
	int startIndex = 0;
	if (!s_isAllAnimationsPaused) {

		for (int i = CurrentFrame; i < min(CurrentFrame + FRAMECOUNT_TO_PRERENDER, m_dwFrameCount); i++)
		{
			if (m_bitmaps.at(i) != nullptr || m_realtimeBitmapBuffer.at(i) != nullptr)
				continue;
			startIndex = i;
			startPreRenderTask = true;
			break;
		}
	}
	if (!m_isCachingFrames && startPreRenderTask)
	{
		m_isCachingFrames = true;
		int startFrame = startIndex;
		int framecount = m_dwFrameCount;
		int endFrame = min(startIndex + FRAMECOUNT_TO_PRERENDER, m_dwFrameCount);
		auto token = cancellationTokenSource.get_token();
		OutputDebugString(("Starting GetRawFramesTask for frame " + startFrame + " to " + endFrame + ".\r\n")->Data());
		create_task([this, startFrame, endFrame, token]() {GetRawFramesTask(startFrame, endFrame, token); }, token)
			.then([&](task<void> t)
		{
			m_isCachingFrames = false;
			bool success = false;
			try
			{
				t.get();
				success = true;
			}
			catch (const task_canceled& e)
			{
				OutputDebugString(L"GetRawFramesTask was canceled.\r\n");
			}
			catch (Exception^ ex)
			{
				OutputDebugString(ex->Message->Data());
			}
		});
	};
	try
	{
		auto bitmap = m_bitmaps.at(uFrameIndex);
		if (bitmap != nullptr)
		{
			m_pRawFrame = bitmap.Get();
			return S_OK;
		}

		bitmap = m_realtimeBitmapBuffer.at(uFrameIndex);

		if (bitmap != nullptr)
		{
			m_pRawFrame = bitmap.Get();
			return S_OK;
		}
	}
	catch (Exception^ ex)
	{
		OutputDebugString(ex->Message->Data());
	}
	return E_ABORT;
}

//Reads a buffer of MAX_CACHED_FRAMES_PER_GIF ahead of current frame.
void GifImageSource::GetRawFramesTask(int startFrame, int endFrame, cancellation_token token)
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	if (startFrame > m_dwFrameCount || endFrame > m_dwFrameCount)
		return;
	int start = startFrame;
	int end = endFrame;
	for (int index = start; index < end; index++)
	{
		if (token.is_canceled())
		{
			cancel_current_task();
		}
		if (m_bitmaps.size() == 0
			|| m_bitmaps.at(index) != nullptr
			|| m_realtimeBitmapBuffer.size() == 0
			|| m_realtimeBitmapBuffer.at(index) != nullptr)
			continue;
		ComPtr<IWICFormatConverter> pConverter = nullptr;
		ComPtr<IWICBitmapFrameDecode> pWicFrame = nullptr;

		PROPVARIANT propValue;
		PropVariantInit(&propValue);

		if (token.is_canceled())
		{
			cancel_current_task();
		}
		// IWICBitmapDecoder is roughly analogous to BitmapDecoder
		// Retrieve the current frame
		HRESULT hr = m_pDecoder->GetFrame(index, &pWicFrame);
		if (SUCCEEDED(hr))
		{
			// Format convert to 32bppPBGRA which D2D expects
			hr = Direct2DManager::GetInstance(m_windowID)->GetIWICFactory()->CreateFormatConverter(&pConverter);
		}
		if (token.is_canceled())
		{
			cancel_current_task();
		}
		if (SUCCEEDED(hr))
		{
			hr = pConverter->Initialize(
				pWicFrame.Get(),
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.f,
				WICBitmapPaletteTypeCustom);
		}
		if (token.is_canceled())
		{
			cancel_current_task();
		}
		ComPtr<ID2D1Bitmap> pRawFrame = nullptr;
		auto d2dContext = Direct2DManager::GetInstance(m_windowID)->GetD2DContext();
		if (SUCCEEDED(hr))
		{
			// Create a D2DBitmap from IWICBitmapSource
			hr = d2dContext->CreateBitmapFromWicBitmap(
				pConverter.Get(),
				nullptr,
				&pRawFrame);
		}
		if (FAILED(hr)) //Frame could not be created. Break out of render loop.
		{
			break;
		}
#if WINDOWS_PHONE_DLL
		ULONG millisecondsSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
		if (millisecondsSinceEpoch - m_lastMemoryCheckEpochTime > 100)
		{
			CheckMemoryLimits();
			m_lastMemoryCheckEpochTime = millisecondsSinceEpoch;
		}
#endif
		if (token.is_canceled())
		{
			cancel_current_task();
		}
		if (m_canCacheMoreFrames
			&& index < MAX_CACHED_FRAMES_PER_GIF
			&& (index == 0 || m_bitmaps.at(index - 1) != nullptr))
		{
			if (m_bitmaps.size() > index)
				m_bitmaps[index] = pRawFrame;
		}
		else
		{
			if (m_realtimeBitmapBuffer.size() > index)
				m_realtimeBitmapBuffer[index] = pRawFrame;
		}
		PropVariantClear(&propValue);
		pConverter = nullptr;
		pWicFrame = nullptr;
		if (index + 1 == end)
		{
			int prevFrame = CurrentFrame;
			for (int i = 0; i < 6; i++)
			{
				if (token.is_canceled() || m_delays.size() < CurrentFrame)
				{
					cancel_current_task();
				}
				long duration = m_delays.at(CurrentFrame) * 5;
				wait(duration);
				if (CurrentFrame != prevFrame)
					break;

			}
		}
		end = min(CurrentFrame + FRAMECOUNT_TO_PRERENDER, m_dwFrameCount);
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(t2 - t1).count();
	OutputDebugString(("Duration for GetRawFrameTask: " + duration + "ms\r\n")->Data());
}



void GifImageSource::SelectNextFrame()
{
	CurrentFrame = (CurrentFrame + 1) % m_dwFrameCount;
}


Windows::Foundation::IAsyncAction^ GifImageSource::SetSourceAsync(IRandomAccessStream^ stream)
{
	if (stream == nullptr)
		throw ref new Platform::InvalidArgumentException();
	ComPtr<IStream> pIStream;
	DX::ThrowIfFailed(
		CreateStreamOverRandomAccessStream(
			reinterpret_cast<IUnknown*>(stream),
			IID_PPV_ARGS(&pIStream)));

	std::string signature = Utilities::ReadStringFromStream(pIStream.Get(), 16);

	Utilities::ImageFileType type = Utilities::getImageTypeByMagic(signature.data());
	LARGE_INTEGER li;
	li.QuadPart = 0;
	pIStream->Seek(li, STREAM_SEEK_SET, nullptr);
	return SetSourceAsync(pIStream, type);
}

Windows::Foundation::IAsyncAction^ GifImageSource::SetSourceAsync(ComPtr<IStream> pIStream, Utilities::ImageFileType filetype)
{
	if (pIStream == nullptr)
		throw ref new Platform::InvalidArgumentException();
	CurrentFrame = 0;
	m_dwPreviousFrame = 0;
	m_completedLoopCount = 0;
	m_cachedKB = 0;
	return create_async([this, pIStream, filetype]() -> void
	{
		switch (filetype)
		{
		case Utilities::IMAGE_FILE_GIF:
			LoadGif(pIStream.Get());
			break;
		case Utilities::IMAGE_FILE_JPG:
		case Utilities::IMAGE_FILE_PNG:
		case Utilities::IMAGE_FILE_TIFF:
		case Utilities::IMAGE_FILE_BMP:
			LoadImage(pIStream.Get());
			break;
		case Utilities::IMAGE_FILE_INVALID:
		default:
			throw ref new InvalidArgumentException("File is not a valid image file");
			break;
		}

	});
}
void GifImageSource::LoadImage(IStream *pStream)
{
	PROPVARIANT var;

	PropVariantInit(&var);
	auto pIWICFactory = Direct2DManager::GetInstance(m_windowID)->GetIWICFactory();
	// IWICBitmapDecoder is roughly analogous to BitmapDecoder
	DX::ThrowIfFailed(pIWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &m_pDecoder));

	m_dwFrameCount = 1;

	m_bitmaps = std::vector<ComPtr<ID2D1Bitmap>>(m_dwFrameCount);
	m_realtimeBitmapBuffer = std::vector<ComPtr<ID2D1Bitmap>>(m_dwFrameCount);
	m_offsets = std::vector<D2D1_POINT_2F>(m_dwFrameCount);
	m_delays = std::vector<USHORT>(m_dwFrameCount);
	m_disposals = std::vector<USHORT>(m_dwFrameCount);

	// IWICBitmapFrameDecode is roughly analogous to BitmapFrame
	ComPtr<IWICBitmapFrameDecode> pFrameDecode;
	DX::ThrowIfFailed(m_pDecoder->GetFrame(0, &pFrameDecode));

	FLOAT fOffsetX = 0.0;
	FLOAT fOffsetY = 0.0;
	USHORT dwDelay = 10; // default to 10 hundredths of a second (100 ms)
	USHORT dwDisposal = 0;

	// Set up converter 
	ComPtr<IWICFormatConverter> pConvertedBitmap;
	DX::ThrowIfFailed(pIWICFactory->CreateFormatConverter(&pConvertedBitmap));
	// Convert bitmap to B8G8R8A8
	DX::ThrowIfFailed(pConvertedBitmap->Initialize(pFrameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom));

	// Store converted bitmap into IWICBitmap so D2D can use intIt
	ComPtr<IWICBitmap> pWicBitmap;
	DX::ThrowIfFailed(pIWICFactory->CreateBitmapFromSource(pConvertedBitmap.Get(), WICBitmapCacheOnDemand, &pWicBitmap));
	auto d2dContext = Direct2DManager::GetInstance(m_windowID)->GetD2DContext();

	// Finally, get ID2D1Bitmap for this frame
	ComPtr<ID2D1Bitmap> pBitmap;

	DX::ThrowIfFailed(d2dContext->CreateBitmapFromWicBitmap(pWicBitmap.Get(), &pBitmap));

	// Push raw frames into bitmaps array. These need to be processed into proper frames before being drawn to screen.
	m_bitmaps[0] = pBitmap;
	m_offsets[0] = { fOffsetX, fOffsetY };
	m_delays[0] = dwDelay;
	m_disposals[0] = dwDisposal;
	m_isLoaded = true;
	//render frame
	Utilities::ui_task(Dispatcher, [&]()
	{
		OnPropertyChanged("FrameCount");
		RenderFrame();
	});

	PropVariantClear(&var);
}

void GifImageSource::LoadGif(IStream *pStream)
{
	PROPVARIANT var;

	PropVariantInit(&var);
	auto pIWICFactory = Direct2DManager::GetInstance(m_windowID)->GetIWICFactory();
	// IWICBitmapDecoder is roughly analogous to BitmapDecoder
	DX::ThrowIfFailed(pIWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &m_pDecoder));


	// IWICMetadataQueryReader is roughly analogous to BitmapPropertiesView
	ComPtr<IWICMetadataQueryReader> pQueryReader;
	DX::ThrowIfFailed(m_pDecoder->GetMetadataQueryReader(&pQueryReader));

	// Get frame count
	UINT dwFrameCount = 0;
	DX::ThrowIfFailed(m_pDecoder->GetFrameCount(&dwFrameCount));
	m_dwFrameCount = dwFrameCount;
	// Get image metadata
	DX::ThrowIfFailed(QueryMetadata(pQueryReader.Get()));

	m_bitmaps = std::vector<ComPtr<ID2D1Bitmap>>(m_dwFrameCount);
	m_realtimeBitmapBuffer = std::vector<ComPtr<ID2D1Bitmap>>(m_dwFrameCount);
	m_offsets = std::vector<D2D1_POINT_2F>(m_dwFrameCount);
	m_delays = std::vector<USHORT>(m_dwFrameCount);
	m_disposals = std::vector<USHORT>(m_dwFrameCount);

	//This number is a little bit arbitrary, but intIt is used to set the max size of frames to cache in raw pixel size.
	int maxBytesToCache = 1000000;
	//The frame size for this gif in bytes
	double frameSizeBytes = (m_width*m_height)*(m_bitsPerPixel) / 8;
	double frameCountToPreload = 1;
	//If the gifs are globally paused, only preload first frame to avoid wasting resources in a scenario where the GIF might never play.
	if (!s_isAllAnimationsPaused) {
		frameCountToPreload = maxBytesToCache / frameSizeBytes;
		frameCountToPreload = max(FRAMECOUNT_TO_PRERENDER, (int)frameCountToPreload);//make sure to always prerender enough frames to don't start the the prerender thread on first frame render.
		frameCountToPreload = min(MAX_CACHED_FRAMES_PER_GIF, (int)frameCountToPreload);//make sure to not exceed MAX_CACHED_FRAMES_PER_GIF count.
	}
	OutputDebugString(("frames to preload: " + frameCountToPreload + "\r\n")->Data());
	//	 Get and convert each frame bitmap into ID2D1Bitmap
	for (UINT dwFrameIndex = 0; dwFrameIndex < dwFrameCount; dwFrameIndex++)
	{
		// IWICBitmapFrameDecode is roughly analogous to BitmapFrame
		ComPtr<IWICBitmapFrameDecode> pFrameDecode;
		DX::ThrowIfFailed(m_pDecoder->GetFrame(dwFrameIndex, &pFrameDecode));

		// Need to get delay and offset metadata for each frame
		ComPtr<IWICMetadataQueryReader> pFrameQueryReader;
		DX::ThrowIfFailed(pFrameDecode->GetMetadataQueryReader(&pFrameQueryReader));

		FLOAT fOffsetX = 0.0;
		FLOAT fOffsetY = 0.0;
		USHORT dwDelay = 10; // default to 10 hundredths of a second (100 ms)
		USHORT dwDisposal = 0;
		// Get delay
		PropVariantClear(&var);
		DX::ThrowIfFailed(pFrameQueryReader->GetMetadataByName(L"/grctlext/Delay", &var));
		dwDelay = var.uiVal;

		//Get disposal
		PropVariantClear(&var);
		DX::ThrowIfFailed(pFrameQueryReader->GetMetadataByName(L"/grctlext/Disposal", &var));
		dwDisposal = var.uiVal;

		// Get offset
		PropVariantClear(&var);
		DX::ThrowIfFailed(pFrameQueryReader->GetMetadataByName(L"/imgdesc/Left", &var));
		fOffsetX = var.uiVal;

		PropVariantClear(&var);
		DX::ThrowIfFailed(pFrameQueryReader->GetMetadataByName(L"/imgdesc/Top", &var));
		fOffsetY = var.uiVal;
		//Preload the bitmaps for the first frames
		if (dwFrameIndex < frameCountToPreload)
		{
			// Set up converter 
			ComPtr<IWICFormatConverter> pConvertedBitmap;
			DX::ThrowIfFailed(pIWICFactory->CreateFormatConverter(&pConvertedBitmap));

			// Convert bitmap to B8G8R8A8
			DX::ThrowIfFailed(pConvertedBitmap->Initialize(pFrameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom));

			// Store converted bitmap into IWICBitmap so D2D can use intIt
			ComPtr<IWICBitmap> pWicBitmap;
			DX::ThrowIfFailed(pIWICFactory->CreateBitmapFromSource(pConvertedBitmap.Get(), WICBitmapCacheOnDemand, &pWicBitmap));
			auto d2dContext = Direct2DManager::GetInstance(m_windowID)->GetD2DContext();

			// Finally, get ID2D1Bitmap for this frame
			ComPtr<ID2D1Bitmap> pBitmap;

			DX::ThrowIfFailed(d2dContext->CreateBitmapFromWicBitmap(pWicBitmap.Get(), &pBitmap));

			// Push raw frames into bitmaps array. These need to be processed into proper frames before being drawn to screen.
			m_bitmaps[dwFrameIndex] = pBitmap;
		}
		m_offsets[dwFrameIndex] = { fOffsetX, fOffsetY };
		m_delays[dwFrameIndex] = dwDelay;
		m_disposals[dwFrameIndex] = dwDisposal;
	}
	m_isLoaded = true;
	//render first frame
	Utilities::ui_task(Dispatcher, [&]()
	{
		OnPropertyChanged("FrameCount");
		RenderFrame();
	});

	PropVariantClear(&var);
}

void GifImageSource::SetRenderEffects(std::vector<Effect> effects)
{
	m_effectDescriptions = effects;
	if (m_isLoaded)
		RenderFrame();
}

HRESULT GifImageSource::QueryMetadata(IWICMetadataQueryReader *pQueryReader)
{
	HRESULT hr = S_OK;
	PROPVARIANT var;
	PropVariantInit(&var);

	//If the GIF contains more than one frame, assume intIt's animated.
	if (m_dwFrameCount > 1)
		m_isAnimatedGif = true;

	hr = pQueryReader->GetMetadataByName(L"/logscrdesc/ColorResolution", &var);
	if (FAILED(hr))
		return hr;
	m_bitsPerPixel = var.uintVal + 1;
	PropVariantClear(&var);

	HRESULT extHr = ReadGifApplicationExtension(pQueryReader);
	if (FAILED(extHr) || extHr == S_FALSE)
	{
		//If we can't find any application extension, default to one repeat.
		auto behavior = RepeatBehavior{ 1 };
		m_repeatBehavior = ref new Platform::Box<RepeatBehavior>(behavior);
	}
	return hr;
}

//This checks for the NETSCAPE2.0 or ANIMEXTS1.0 extensions.
//These extensions tell us of the GIF is animated or not.
HRESULT GifImageSource::ReadGifApplicationExtension(IWICMetadataQueryReader *pQueryReader)
{
	PROPVARIANT var;
	PropVariantInit(&var);
	HRESULT hr = pQueryReader->GetMetadataByName(L"/appext/Application", &var);
	if (SUCCEEDED(hr))
	{

		if (var.vt != (VT_UI1 | VT_VECTOR))
			return S_FALSE;
		if (var.caub.cElems != 0x0B) // must be exactly 11 bytes long
			return S_FALSE;

		static LPCSTR strNetscape = "NETSCAPE2.0";
		static LPCSTR strAnimexts = "ANIMEXTS1.0";

		std::string str(var.caub.pElems, var.caub.pElems + var.caub.cElems);
		if (str != strNetscape && str != strAnimexts)
		{
			return S_FALSE;
		}

		PropVariantClear(&var);
		hr = pQueryReader->GetMetadataByName(L"/appext/Data", &var);
		if (FAILED(hr))
			return hr;

		// Only handle case where the application data block is exactly 4 bytes long
		if (var.caub.cElems != 4)
			return S_FALSE;

		auto count = *var.caub.pElems; // Data length; we generally expect this value to be 3
		if (count >= 1)
		{
			m_isAnimatedGif = *(var.caub.pElems + 1) != 0; // is animated gif; we generally expect this byte to be 1
		}
		if (count == 3)
		{
			auto defaultRepeat = RepeatBehavior();
			//hack: find intIt how to compare to default class instead of this.
			if (m_repeatBehavior == nullptr || (defaultRepeat.Type == m_repeatBehavior->Value.Type
				&& defaultRepeat.Count == m_repeatBehavior->Value.Count
				&& defaultRepeat.Duration.Duration == m_repeatBehavior->Value.Duration.Duration))
			{
				// iteration count; 2nd element is LSB, 3rd element is MSB
				// we generally expect this value to be 0
				auto loopCount = (*(var.caub.pElems + 3) << 8) + *(var.caub.pElems + 2);
				if (loopCount == 0)
					m_repeatBehavior = RepeatBehavior::Forever;
				else
				{
					auto behavior = RepeatBehavior{ (double)loopCount };
					m_repeatBehavior = ref new Platform::Box<RepeatBehavior>(behavior);
				}
			}
		}
	}
	return hr;
}

void GifImageSource::CreateDeviceResources(boolean forceRecreate)
{
	auto currentWindow = Window::Current;
	if (currentWindow == nullptr)
	{
		throw ref new Exception(E_FAIL, "Gif must have a window parent");
	}
	m_windowID = currentWindow->GetHashCode();

	if (forceRecreate)
	{
		Direct2DManager::GetInstance(m_windowID)->Recreate();
	}

	if (!m_haveReservedDeviceResources)
	{
		Direct2DManager::GetInstance(m_windowID);
		Direct2DManager::ReserveInstance(m_windowID);
		m_haveReservedDeviceResources = true;
	}






	auto dxgiDevice = Direct2DManager::GetInstance(m_windowID)->GetDXGIDevice();

	// Query for ISurfaceImageSourceNative interface.
	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
	);

	// Associate the DXGI device with the SurfaceImageSource. 
	DX::ThrowIfFailed(
		sisNative->SetDevice(dxgiDevice.Get()));
}



HRESULT GifImageSource::BeginDraw()
{
	RECT updateRect = { 0, 0, m_width, m_height };
	POINT offset = { 0 };

	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
	);
	ComPtr<IDXGISurface> surface;
	// Begin drawing - returns a target surface and an offset to use as the top left origin when drawing. 
	HRESULT beginDrawHR = sisNative->BeginDraw(updateRect, &surface, &offset);
	if (SUCCEEDED(beginDrawHR))
	{
		Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext = Direct2DManager::GetInstance(m_windowID)->GetD2DContext();

		// Create render target. 
		DX::ThrowIfFailed(
			d2dContext->CreateBitmapFromDxgiSurface(surface.Get(), nullptr, &m_surfaceBitmap));

		// Set context's render target. 
		d2dContext->SetTarget(m_surfaceBitmap.Get());

		// Begin drawing using D2D context. 
		d2dContext->BeginDraw();

		// Apply a clip and transform to constrain updates to the target update area. 
		// This is required to ensure coordinates within the target surface remain 
		// consistent by taking into account the offset returned by BeginDraw, and 
		// can also improve performance by optimizing the area that is drawn by D2D. 
		// Apps should always account for the offset output parameter returned by  
		// BeginDraw, since intIt may not match the passed updateRect input parameter's location. 
		d2dContext->PushAxisAlignedClip(
			D2D1::RectF(
				static_cast<float>(offset.x),
				static_cast<float>(offset.y),
				static_cast<float>(offset.x + updateRect.right),
				static_cast<float>(offset.y + updateRect.bottom)
			),
			D2D1_ANTIALIAS_MODE_ALIASED);

		d2dContext->SetTransform(
			D2D1::Matrix3x2F::Translation(static_cast<float>(offset.x), static_cast<float>(offset.y)));

	}
	else if (beginDrawHR == DXGI_ERROR_DEVICE_REMOVED || beginDrawHR == DXGI_ERROR_DEVICE_RESET)
	{
		// If the device has been removed or reset, attempt to recreate intIt and continue drawing. 
		CreateDeviceResources(true);
		BeginDraw();
	}
	return beginDrawHR;
}

void GifImageSource::EndDraw()
{
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext = Direct2DManager::GetInstance(m_windowID)->GetD2DContext();
	// Remove the transform and clip applied in BeginDraw since 
	// the target area can change on every update. 
	d2dContext->SetTransform(D2D1::IdentityMatrix());
	d2dContext->PopAxisAlignedClip();

	// Remove the render target and end drawing. 
	DX::ThrowIfFailed(d2dContext->EndDraw());

	d2dContext->SetTarget(nullptr);
	m_surfaceBitmap = nullptr;

	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
	);

	DX::ThrowIfFailed(sisNative->EndDraw());
}



void GifImageSource::StartDurationTimer()
{
	if (m_durationTimer == nullptr && m_repeatBehavior != nullptr && m_repeatBehavior->Value.Type == RepeatBehaviorType::Duration)
	{
		auto durationTimerCall = new concurrency::call<int>(
			[this](int)
		{
			// Timer might've been stopped just prior to RenderAndPrepareFrame
			if (m_durationTimer == nullptr)
			{
				return;
			}
			Utilities::ui_task(Dispatcher, [this]()
			{
				Stop();
				CurrentFrame = 0;
				RenderFrame();
			});
		});

		long millis = m_repeatBehavior->Value.Duration.Duration / 10000;
		m_durationTimer = new concurrency::timer<int>(millis, 0, durationTimerCall, false);
		m_durationTimer->start();
	}
}

void GifImageSource::StopDurationTimer()
{
	if (m_durationTimer != nullptr)
	{
		m_durationTimer->stop();
		delete m_durationTimer;
		m_durationTimer = nullptr;
	}
}

long GifImageSource::SetNextInterval()
{
	auto delay = m_delays.at(CurrentFrame);
	if (delay < 2)
	{
		delay = 10; // default to 100ms if delay is too short
	}

	return 10 * delay; // 10ms * delay. 
}

void GifImageSource::Start()
{

	if (!m_isAnimatedGif)
		return;
	if (m_isAnimating)
		return;

	m_isAnimating = true;
	cancellationTokenSource = cancellation_token_source();
	if (m_RenderingToken.Value > 0)
	{
		try
		{
			Windows::UI::Xaml::Media::CompositionTarget::Rendering -= m_RenderingToken;
		}
		catch (...) {}
	}
	m_RenderingToken = Windows::UI::Xaml::Media::CompositionTarget::Rendering += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &GifImage::GifImageSource::OnRendering);


	StartDurationTimer();
}



void GifImageSource::StopAndClear()
{
	cancellationTokenSource.cancel();
	Utilities::ui_task(Dispatcher, [&]()
	{
		if (m_RenderingToken.Value > 0)
		{
			Windows::UI::Xaml::Media::CompositionTarget::Rendering -= m_RenderingToken;
		}
	});
	m_isAnimating = false;
	ClearResources();
}

void GifImageSource::Pause()
{
	Utilities::ui_task(Dispatcher, [&]()
	{
		if (m_RenderingToken.Value > 0)
		{
			Windows::UI::Xaml::Media::CompositionTarget::Rendering -= m_RenderingToken;
		}
	});
	m_isAnimating = false;
}
void GifImageSource::Stop()
{
	if (m_RenderingToken.Value > 0)
	{
		Windows::UI::Xaml::Media::CompositionTarget::Rendering -= m_RenderingToken;
	}
	cancellationTokenSource.cancel();
	CurrentFrame = 0;
	m_isAnimating = false;
	RenderFrame();

}

void GifImageSource::PauseAllGifs()
{
	s_isAllAnimationsPaused = true;
}

void GifImageSource::ResumeAllGifs()
{
	s_isAllAnimationsPaused = false;
}

///Renders the current frame and sets up the timing for the next one.
long GifImageSource::RenderAndPrepareFrame()
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();


	long span = SetNextInterval();

	HRESULT hr = GetRawFrame(CurrentFrame);

	if (hr == E_ABORT)
	{
		OutputDebugString(("Bitmap not decoded, delaying frame " + CurrentFrame + "\r\n")->Data());
	}
	else
	{
		try
		{
			bool completedLoop = RenderFrame();
			OnFrameChanged(this);
			if (completedLoop)
			{
				m_completedLoopCount++;
				m_pPreviousRawFrame = nullptr;

				if (m_repeatBehavior != nullptr)
				{
					// Stop animation if this is not a perpetually looping gif
					if (m_repeatBehavior->Value.Type == RepeatBehaviorType::Count && m_repeatBehavior->Value.Count > 0 && m_repeatBehavior->Value.Count == m_completedLoopCount)
					{

						Stop();
						for (int i = 0; i < m_dwFrameCount; i++)
						{
							m_bitmaps.at(i) = nullptr;
						}
						m_pRawFrame = nullptr;
						OnAnimationCompleted(this);
					}
				}
			}
		}
		catch (Exception^ ex)
		{
			StopAndClear();
		}
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(t2 - t1).count();
	long delayTime = max(0, span - duration);
	m_nextFrameTimePoint = t2 + milliseconds(delayTime);
	return delayTime;
}

void GifImageSource::CheckMemoryLimits()
{
#if WINDOWS_PHONE_DLL

	double mbMemoryLimit = Windows::System::MemoryManager::AppMemoryUsageLimit / 1024 / 1024;
	double mbMemoryUsed = Windows::System::MemoryManager::AppMemoryUsage / 1024 / 1024;
	//OutputDebugString(("Memory usage is: " + mbMemoryUsed + "mb / " + mbMemoryLimit + "mb\r\n")->Data());
	double  percentUsed = (mbMemoryUsed / mbMemoryLimit) * 100;
	if (m_canCacheMoreFrames == true || percentUsed > MEMORY_PERCENT_TO_DELETE_FRAME_CACHE)
	{
		if (percentUsed > MEMORY_PERCENT_TO_DELETE_FRAME_CACHE)
		{
			for (int i = CurrentFrame; i < m_dwFrameCount; i++)
			{
				m_bitmaps.at(i) = nullptr;
			}
			OutputDebugString(("Stopping bitmap caching and clearing all cached bitmaps, memory usage is " + percentUsed + "%\r\n")->Data());
		}
		else if (percentUsed > MEMORY_PERCENT_TO_STOP_FRAME_CACHE)
		{
			m_canCacheMoreFrames = false;
			OutputDebugString(("Stopping bitmap caching, memory usage is " + percentUsed + "%\r\n")->Data());
		}
	}
	else
	{
		if (percentUsed < MEMORY_PERCENT_TO_STOP_FRAME_CACHE && m_canCacheMoreFrames == false)
		{
			m_canCacheMoreFrames = true;
			OutputDebugString(("Resuming bitmap caching, memory usage is " + percentUsed + "%\r\n")->Data());
		}
	}

#endif
}

void GifImageSource::OnSuspending(Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e)
{
	ComPtr<IDXGIDevice3> dxgiDevice;
	Direct2DManager::GetInstance(m_windowID)->GetD3DDevice().As(&dxgiDevice);
	Direct2DManager::GetInstance(m_windowID)->GetD3DContext()->Flush();
	Direct2DManager::GetInstance(m_windowID)->GetD3DContext()->ClearState();
	// Hints to the driver that the app is entering an idle state and that its memory can be used temporarily for other apps.
	dxgiDevice->Trim();

}


void GifImageSource::OnRendering(Platform::Object ^sender, Platform::Object ^args)
{
	if (m_isRenderingFrame || s_isAllAnimationsPaused || high_resolution_clock::now() < m_nextFrameTimePoint)
		return;

	m_isRenderingFrame = true;
	RenderAndPrepareFrame();
	m_isRenderingFrame = false;
}
