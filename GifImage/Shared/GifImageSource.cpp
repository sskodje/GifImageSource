#include "pch.h"
#include <ppl.h>
#include <shcore.h>
#include "GifImageSource.h"
#include "Utilities.h"
#include <chrono>
using namespace std;
using namespace std::chrono;
using namespace GifImage;
using namespace Platform;
using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;

using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage;
using namespace Windows::Networking::BackgroundTransfer;

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
	m_offsets(NULL),
	m_delays(NULL),
	m_dwCurrentFrame(0),
	m_dwPreviousFrame(0),
	m_completedLoopCount(0),
	//m_loopCount(0),
	m_bitsPerPixel(8),
	m_haveReservedDeviceResources(false),
	m_repeatBehavior(nullptr),
	m_isAnimatedGif(false)
{
	if (width < 0 || height < 0)
		throw ref new Platform::InvalidArgumentException("Height and Width cannot be less than zero");

	m_repeatBehavior = repeatBehavior;
	CreateDeviceResources(false);
}

GifImageSource::~GifImageSource()
{
	ClearResources();

	OutputDebugString(L"Destructed GifImageSource\r\n");
}


void GifImageSource::ClearResources()
{
	m_pDecoder = nullptr;
	m_pRawFrame=nullptr;

	Stop();
	if (m_renderTimer != nullptr)
	{
		m_renderTimer->Tick -= m_tickToken;
		m_renderTimer = nullptr;

	}
	StopDurationTimer();
	//while (!m_bitmaps.empty())
	//{
	//	m_bitmaps.back() = nullptr;
	//	m_bitmaps.pop_back();
	//}
	m_bitmaps.clear();
	m_offsets.clear();
	m_delays.clear();
	m_disposals.clear();

	m_width = 0;
	m_height = 0;
	//m_loopCount = 0;
	m_completedLoopCount = 0;
	m_isAnimatedGif = false;

	m_repeatBehavior = nullptr;


	m_dwFrameCount = 0;
	m_dwCurrentFrame = 0;
	m_dwPreviousFrame = 0;

	m_stream = nullptr;
	m_surfaceBitmap = nullptr;
	m_pIWICFactory = nullptr;
	if (m_haveReservedDeviceResources)
	{
		Direct2DManager::ReturnInstance();
		m_haveReservedDeviceResources = false;
	}
}



bool GifImageSource::RenderFrame()
{

	auto bCanDraw = BeginDraw();
	auto m_d2dContext = Direct2DManager::GetInstance()->GetD2DContext();
	if (bCanDraw)
	{
		//auto bitmap = m_bitmaps.at(m_dwCurrentFrame).Get();
		HRESULT hr = GetRawFrame(m_dwCurrentFrame);
		if (FAILED(hr))//bitmap == nullptr)
		{
			OutputDebugString(L"Bitmap is null, returning from RenderFrame");
			return false;
		}
		//if (m_dwCurrentFrame == 0)
			m_d2dContext->Clear();

		for (uint32 i = 0; i < m_dwCurrentFrame; i++)
		{
			int disposal = m_disposals[i];

			switch (disposal)
			{
			case DISPOSAL_UNSPECIFIED:
			case DISPOSE_DO_NOT:
				//m_d2dContext->DrawImage(m_bitmaps.at(i).Get(), m_offsets.at(i));
				hr = GetRawFrame(m_dwCurrentFrame);
				if(SUCCEEDED(hr))
					m_d2dContext->DrawImage(m_pRawFrame.Get(), m_offsets.at(i));
				break;
			case DISPOSE_BACKGROUND:
			{
				auto offset = m_offsets.at(i);
				//auto bitmap = m_bitmaps.at(i).Get();
				hr = GetRawFrame(m_dwCurrentFrame);
				if (SUCCEEDED(hr))
				{
					m_d2dContext->PushAxisAlignedClip(
						D2D1::RectF(
							static_cast<float>(offset.x),
							static_cast<float>(offset.y),
							static_cast<float>(offset.x + m_pRawFrame->GetPixelSize().width),
							static_cast<float>(offset.y + m_pRawFrame->GetPixelSize().height)
							),
						D2D1_ANTIALIAS_MODE_ALIASED);


					m_d2dContext->Clear();
					m_d2dContext->PopAxisAlignedClip();
				}
				break;
			}
			case DISPOSE_PREVIOUS:
			default:
				// We don't need to render the intermediate frame if it's not
				// going to update the intermediate buffer anyway.
				break;
			}
		}

		//draw current frame on top
		//m_d2dContext->DrawImage(m_bitmaps.at(m_dwCurrentFrame).Get(), m_offsets.at(m_dwCurrentFrame));
		m_d2dContext->DrawImage(m_pRawFrame.Get(), m_offsets.at(m_dwCurrentFrame));

		SetNextInterval();
		//SetupPreviousFrame();
		EndDraw();
		SelectNextFrame();
	}
	else
	{
	}

	// Returns true if we just completed a loop
	return m_dwCurrentFrame == 0;
}

HRESULT GifImageSource::GetRawFrame(UINT uFrameIndex)
{
	IWICFormatConverter *pConverter = nullptr;
	IWICBitmapFrameDecode *pWicFrame = nullptr;
	IWICMetadataQueryReader *pFrameMetadataQueryReader = nullptr;

	PROPVARIANT propValue;
	PropVariantInit(&propValue);

	auto m_d2dContext = Direct2DManager::GetInstance()->GetD2DContext();
	// IWICBitmapDecoder is roughly analogous to BitmapDecoder
	


	// Retrieve the current frame
	HRESULT hr = m_pDecoder->GetFrame(uFrameIndex, &pWicFrame);
	if (SUCCEEDED(hr))
	{
		// Format convert to 32bppPBGRA which D2D expects
		hr = m_pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pWicFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.f,
			WICBitmapPaletteTypeCustom);
	}

	if (SUCCEEDED(hr))
	{
		// Create a D2DBitmap from IWICBitmapSource
		m_pRawFrame = nullptr;
		hr = m_d2dContext->CreateBitmapFromWicBitmap(
			pConverter,
			nullptr,
			&m_pRawFrame);
	}

	//if (SUCCEEDED(hr))
	//{
	//	// Get Metadata Query Reader from the frame
	//	hr = pWicFrame->GetMetadataQueryReader(&pFrameMetadataQueryReader);
	//}

	//// Get the Metadata for the current frame
	//if (SUCCEEDED(hr))
	//{
	//	hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Left", &propValue);
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
	//		if (SUCCEEDED(hr))
	//		{
	//			m_framePosition.left = static_cast<float>(propValue.uiVal);
	//		}
	//		PropVariantClear(&propValue);
	//	}
	//}

	//if (SUCCEEDED(hr))
	//{
	//	hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Top", &propValue);
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
	//		if (SUCCEEDED(hr))
	//		{
	//			m_framePosition.top = static_cast<float>(propValue.uiVal);
	//		}
	//		PropVariantClear(&propValue);
	//	}
	//}

	//if (SUCCEEDED(hr))
	//{
	//	hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Width", &propValue);
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
	//		if (SUCCEEDED(hr))
	//		{
	//			m_framePosition.right = static_cast<float>(propValue.uiVal)
	//				+ m_framePosition.left;
	//		}
	//		PropVariantClear(&propValue);
	//	}
	//}

	//if (SUCCEEDED(hr))
	//{
	//	hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Height", &propValue);
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
	//		if (SUCCEEDED(hr))
	//		{
	//			m_framePosition.bottom = static_cast<float>(propValue.uiVal)
	//				+ m_framePosition.top;
	//		}
	//		PropVariantClear(&propValue);
	//	}
	//}

	//if (SUCCEEDED(hr))
	//{
	//	// Get delay from the optional Graphic Control Extension
	//	if (SUCCEEDED(pFrameMetadataQueryReader->GetMetadataByName(
	//		L"/grctlext/Delay",
	//		&propValue)))
	//	{
	//		hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
	//		if (SUCCEEDED(hr))
	//		{
	//			// Convert the delay retrieved in 10 ms units to a delay in 1 ms units
	//			hr = UIntMult(propValue.uiVal, 10, &m_uFrameDelay);
	//		}
	//		PropVariantClear(&propValue);
	//	}
	//	else
	//	{
	//		// Failed to get delay from graphic control extension. Possibly a
	//		// single frame image (non-animated gif)
	//		m_uFrameDelay = 0;
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		// Insert an artificial delay to ensure rendering for gif with very small
	//		// or 0 delay.  This delay number is picked to match with most browsers' 
	//		// gif display speed.
	//		//
	//		// This will defeat the purpose of using zero delay intermediate frames in 
	//		// order to preserve compatibility. If this is removed, the zero delay 
	//		// intermediate frames will not be visible.
	//		if (m_uFrameDelay < 30)
	//		{
	//			m_uFrameDelay = 30;
	//		}
	//	}
	//}

	//if (SUCCEEDED(hr))
	//{
	//	if (SUCCEEDED(pFrameMetadataQueryReader->GetMetadataByName(
	//		L"/grctlext/Disposal",
	//		&propValue)))
	//	{
	//		hr = (propValue.vt == VT_UI1) ? S_OK : E_FAIL;
	//		if (SUCCEEDED(hr))
	//		{
	//			m_uFrameDisposal = propValue.bVal;
	//		}
	//	}
	//	else
	//	{
	//		// Failed to get the disposal method, use default. Possibly a 
	//		// non-animated gif.
	//		m_uFrameDisposal = DM_UNDEFINED;
	//	}
	//}

	PropVariantClear(&propValue);

	SafeRelease(pConverter);
	SafeRelease(pWicFrame);
	SafeRelease(pFrameMetadataQueryReader);

	return hr;
}

void GifImageSource::SelectNextFrame()
{
	//////m_dwPreviousFrame = m_dwCurrentFrame;
	//int disposal = m_disposals[m_dwCurrentFrame];

	//	switch (disposal)
	//	{
	//	case DISPOSE_BACKGROUND:
	//	{
	//		auto offset = m_offsets[m_dwCurrentFrame];
	//		if (offset.x == 0 && offset.y == 0)
	//		{
	//			m_dwPreviousFrame = m_dwCurrentFrame;
	//		}
	//		break;
	//	}
	//	case DISPOSAL_UNSPECIFIED:
	//	case DISPOSE_DO_NOT:
	//	case DISPOSE_PREVIOUS:
	//	default:
	//		// Corrupt disposal.
	//		//m_dwPreviousFrame = m_dwCurrentFrame;
	//		break;

	//	}

	m_dwCurrentFrame = (m_dwCurrentFrame + 1) % m_dwFrameCount;

	//if (m_dwCurrentFrame < m_dwPreviousFrame)
	//{
	//	m_dwPreviousFrame = 0;
	//}
}


Windows::Foundation::IAsyncAction^ GifImageSource::SetSourceAsync(IRandomAccessStream^ pStream)
{
	if (pStream == nullptr)
		throw ref new Platform::InvalidArgumentException();

	return create_async([this, pStream]() -> void
	{
		m_dwCurrentFrame = 0;
		m_dwPreviousFrame = 0;
		m_completedLoopCount = 0;

		ComPtr<IStream> pIStream;
		DX::ThrowIfFailed(
			CreateStreamOverRandomAccessStream(
				reinterpret_cast<IUnknown*>(pStream),
				IID_PPV_ARGS(&pIStream)));

		LoadImage(pIStream.Get());
	});
}

void GifImageSource::LoadImage(IStream *pStream)
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	HRESULT hr = S_OK;
	PROPVARIANT var;

	PropVariantInit(&var);

	// IWICImagingFactory is where it all begins
	ComPtr<IWICImagingFactory> pFactory;
	hr = CoCreateInstance(CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&pFactory);

	// IWICBitmapDecoder is roughly analogous to BitmapDecoder
	//ComPtr<IWICBitmapDecoder> pDecoder;
	hr = pFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &m_pDecoder);
	DX::ThrowIfFailed(hr);

	// IWICMetadataQueryReader is roughly analogous to BitmapPropertiesView
	ComPtr<IWICMetadataQueryReader> pQueryReader;
	hr = m_pDecoder->GetMetadataQueryReader(&pQueryReader);
	DX::ThrowIfFailed(hr);
	// Get image metadata
	hr = QueryMetadata(pQueryReader.Get());
	DX::ThrowIfFailed(hr);
	// Get frame count
	UINT dwFrameCount = 0;
	hr = m_pDecoder->GetFrameCount(&dwFrameCount);

	m_dwFrameCount = dwFrameCount;

	m_bitmaps = std::vector<ComPtr<ID2D1Bitmap>>(m_dwFrameCount);
	m_offsets = std::vector<D2D1_POINT_2F>(m_dwFrameCount);
	m_delays = std::vector<USHORT>(m_dwFrameCount);
	m_disposals = std::vector<USHORT>(m_dwFrameCount);
	// Get and convert each frame bitmap into ID2D1Bitmap
	//for (UINT dwFrameIndex = 0; dwFrameIndex < dwFrameCount; dwFrameIndex++)
	//{
	//	// IWICBitmapFrameDecode is roughly analogous to BitmapFrame
	//	ComPtr<IWICBitmapFrameDecode> pFrameDecode;
	//	hr = m_pDecoder->GetFrame(dwFrameIndex, &pFrameDecode);

	//	// Need to get delay and offset metadata for each frame
	//	ComPtr<IWICMetadataQueryReader> pFrameQueryReader;
	//	hr = pFrameDecode->GetMetadataQueryReader(&pFrameQueryReader);

	//	FLOAT fOffsetX = 0.0;
	//	FLOAT fOffsetY = 0.0;
	//	USHORT dwDelay = 10; // default to 10 hundredths of a second (100 ms)
	//	USHORT dwDisposal = 0;
	//	// Get delay
	//	PropVariantClear(&var);
	//	hr = pFrameQueryReader->GetMetadataByName(L"/grctlext/Delay", &var);
	//	dwDelay = var.uiVal;

	//	//Get disposal
	//	PropVariantClear(&var);
	//	hr = pFrameQueryReader->GetMetadataByName(L"/grctlext/Disposal", &var);
	//	dwDisposal = var.uiVal;

	//	// Get offset
	//	PropVariantClear(&var);
	//	hr = pFrameQueryReader->GetMetadataByName(L"/imgdesc/Left", &var);
	//	fOffsetX = var.uiVal;

	//	PropVariantClear(&var);
	//	hr = pFrameQueryReader->GetMetadataByName(L"/imgdesc/Top", &var);
	//	fOffsetY = var.uiVal;
	//	//if (dwFrameIndex == 0)
	//	//{
	//		// Set up converter 
	//	ComPtr<IWICFormatConverter> pConvertedBitmap;
	//	hr = pFactory->CreateFormatConverter(&pConvertedBitmap);

	//	// Convert bitmap to B8G8R8A8
	//	hr = pConvertedBitmap->Initialize(pFrameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);

	//	// Store converted bitmap into IWICBitmap so D2D can use it
	//	ComPtr<IWICBitmap> pWicBitmap;
	//	hr = pFactory->CreateBitmapFromSource(pConvertedBitmap.Get(), WICBitmapCacheOnDemand, &pWicBitmap);



	//	auto m_d2dContext = Direct2DManager::GetInstance()->GetD2DContext();
	//	//auto m_d2dContext = m_d2dManager->GetD2DContext();

	//	// Finally, get ID2D1Bitmap for this frame
	//	ComPtr<ID2D1Bitmap> pBitmap;

	//	hr = m_d2dContext->CreateBitmapFromWicBitmap(pWicBitmap.Get(), &pBitmap);
	//	DX::ThrowIfFailed(hr);

	//	// Push raw frames into bitmaps array. These need to be processed into proper frames before being drawn to screen.
	//	m_bitmaps[dwFrameIndex] = pBitmap;
	//	/*	}*/
	//	m_offsets[dwFrameIndex] = { fOffsetX, fOffsetY };
	//	m_delays[dwFrameIndex] = dwDelay;
	//	m_disposals[dwFrameIndex] = dwDisposal;
	//}

	m_stream = pStream;
	PropVariantClear(&var);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(t2 - t1).count();
	OutputDebugString(("Duration for LoadImage: " + duration + "ms\r\n")->Data());
}

HRESULT GifImageSource::QueryMetadata(IWICMetadataQueryReader *pQueryReader)
{
	HRESULT hr = S_OK;
	PROPVARIANT var;
	PropVariantInit(&var);

	// Default to non-animated gif
	m_isAnimatedGif = true;

	hr = pQueryReader->GetMetadataByName(L"/logscrdesc/ColorResolution", &var);
	if (FAILED(hr))
		return hr;
	m_bitsPerPixel = var.uintVal + 1;
	PropVariantClear(&var);

	HRESULT extHr = ReadGifApplicationExtension(pQueryReader);

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
		//for (ULONG i = 0; i < var.caub.cElems; i++)
		//{
		//	// Manual strcmp
		//	if (*(var.caub.pElems + i) != strNetscape[i])
		//		return S_FALSE;
		//}

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
			//hack: find it how to compare to default class instead of this.
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
}

void GifImageSource::CreateDeviceResources(boolean forceRecreate)
{
	//m_d2dManager = Direct2DManager::GetInstance();
	if (forceRecreate)
	{
		Direct2DManager::GetInstance()->Recreate();
		//	m_d2dManager->Recreate();
	}
	if (!m_haveReservedDeviceResources)
	{
		Direct2DManager::ReserveInstance();
		m_haveReservedDeviceResources = true;
	}


	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&m_pIWICFactory);

	auto dxgiDevice = Direct2DManager::GetInstance()->GetDXGIDevice();

	// Query for ISurfaceImageSourceNative interface.
	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
		);

	// Associate the DXGI device with the SurfaceImageSource. 
	DX::ThrowIfFailed(
		sisNative->SetDevice(dxgiDevice.Get()));
}

bool GifImageSource::BeginDraw()
{
	ComPtr<IDXGISurface> surface;
	RECT updateRect = { 0, 0, m_width, m_height };
	POINT offset = { 0 };

	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
		);

	// Begin drawing - returns a target surface and an offset to use as the top left origin when drawing. 
	HRESULT beginDrawHR = sisNative->BeginDraw(updateRect, &surface, &offset);
	//	m_offset = offset;
	if (SUCCEEDED(beginDrawHR))
	{
		auto m_d2dContext = Direct2DManager::GetInstance()->GetD2DContext();
		//	auto m_d2dContext = m_d2dManager->GetD2DContext();
			// Create render target. 
		DX::ThrowIfFailed(
			m_d2dContext->CreateBitmapFromDxgiSurface(surface.Get(), nullptr, &m_surfaceBitmap));

		// Set context's render target. 
		m_d2dContext->SetTarget(m_surfaceBitmap.Get());

		// Begin drawing using D2D context. 
		m_d2dContext->BeginDraw();

		// Apply a clip and transform to constrain updates to the target update area. 
		// This is required to ensure coordinates within the target surface remain 
		// consistent by taking into account the offset returned by BeginDraw, and 
		// can also improve performance by optimizing the area that is drawn by D2D. 
		// Apps should always account for the offset output parameter returned by  
		// BeginDraw, since it may not match the passed updateRect input parameter's location. 
		m_d2dContext->PushAxisAlignedClip(
			D2D1::RectF(
				static_cast<float>(offset.x),
				static_cast<float>(offset.y),
				static_cast<float>(offset.x + updateRect.right),
				static_cast<float>(offset.y + updateRect.bottom)
				),
			D2D1_ANTIALIAS_MODE_ALIASED);

		m_d2dContext->SetTransform(
			D2D1::Matrix3x2F::Translation(static_cast<float>(offset.x), static_cast<float>(offset.y)));
	}
	else if (beginDrawHR == DXGI_ERROR_DEVICE_REMOVED || beginDrawHR == DXGI_ERROR_DEVICE_RESET)
	{
		// If the device has been removed or reset, attempt to recreate it and continue drawing. 
		CreateDeviceResources(true);
		BeginDraw();
	}

	else
	{
		return false;
	}

	return true;
}

void GifImageSource::EndDraw()
{
	auto m_d2dContext = Direct2DManager::GetInstance()->GetD2DContext();
	// Remove the transform and clip applied in BeginDraw since 
	// the target area can change on every update. 
	m_d2dContext->SetTransform(D2D1::IdentityMatrix());
	m_d2dContext->PopAxisAlignedClip();

	// Remove the render target and end drawing. 
	DX::ThrowIfFailed(m_d2dContext->EndDraw());


	m_d2dContext->SetTarget(nullptr);

	m_surfaceBitmap = nullptr;


	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(
		reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
		);

	DX::ThrowIfFailed(sisNative->EndDraw());
}


void GifImageSource::Restart()
{
	m_completedLoopCount = 0;
	StopDurationTimer();
	StartDurationTimer();
}
void GifImageSource::StartDurationTimer()
{
	if (m_repeatBehavior != nullptr && m_repeatBehavior->Value.Type == RepeatBehaviorType::Duration)
	{
		//double milliseconds = duration.Duration * 10000; 
		m_durationTimer = ref new DispatcherTimer();
		m_durationTimer->Interval = m_repeatBehavior->Value.Duration;
		m_durationTickToken = m_durationTimer->Tick += ref new EventHandler<Object^>(this, &GifImageSource::OnDurationEndedTick);
		m_durationTimer->Start();
	}

}
void GifImageSource::StopDurationTimer()
{
	if (m_renderTimer != nullptr)
	{
		m_durationTimer->Tick -= m_durationTickToken;
		m_durationTimer->Stop();
		m_durationTimer = nullptr;
	}
}
void GifImageSource::SetNextInterval()
{
	auto delay = m_delays.at(m_dwCurrentFrame);
	if (delay < 3)
	{
		delay = 10; // default to 100ms if delay is too short
	}

	auto timespan = TimeSpan();
	timespan.Duration = 80000L * delay; // 8ms * delay. (should be 10ms, but this gives the impression of awesome perf)
	if (m_renderTimer != nullptr)
		m_renderTimer->Interval = timespan;
}

void GifImageSource::Start()
{
	if (!m_isAnimatedGif)
		return;
	if (m_renderTimer == nullptr)
	{
		m_renderTimer = ref new DispatcherTimer();
		m_tickToken = m_renderTimer->Tick += ref new EventHandler<Object^>(this, &GifImageSource::OnTick);
	}
	if (m_renderTimer->IsEnabled)
		return;

	if (m_completedLoopCount == 0)
	{
		m_renderTimer->Start();
	}
	StartDurationTimer();
}

void GifImageSource::Stop()
{
	if (m_renderTimer != nullptr && m_renderTimer->IsEnabled)
	{
		m_renderTimer->Stop();
		m_completedLoopCount = 0;
	}
}

void GifImageSource::OnTick(Object ^sender, Object ^args)
{
	// Timer might've been stopped just prior to OnTick
	if (m_renderTimer == nullptr || !m_renderTimer->IsEnabled)
	{
		return;
	}

	try
	{
		auto completedLoop = RenderFrame();
		if (completedLoop)
		{
			m_completedLoopCount++;

			if (m_repeatBehavior != nullptr)
			{
				// Stop animation if this is not a perpetually looping gif
				if (m_repeatBehavior->Value.Type == RepeatBehaviorType::Count && m_repeatBehavior->Value.Count > 0 && m_repeatBehavior->Value.Count == m_completedLoopCount)
				{
					//Reset the gif to it's first frame before we stop
					RenderFrame();
					Stop();
				}
			}
		}
	}
	catch (Platform::Exception^)
	{

	}
}

void GifImageSource::OnDurationEndedTick(Object ^sender, Object ^args)
{
	// Timer might've been stopped just prior to OnTick
	if (m_durationTimer == nullptr || !m_durationTimer->IsEnabled)
	{
		return;
	}
	m_dwCurrentFrame = 0;
	RenderFrame();

	Stop();
	m_dwCurrentFrame = 0;
}



