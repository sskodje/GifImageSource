﻿#pragma once
#include <wincodec.h>
#include "windows.foundation.h"
#include "Direct2DManager.h"
#include <agents.h>

using namespace Windows::UI::Xaml::Media::Animation;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace Platform;

namespace GifImage
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class GifImageSource sealed : Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
	{
	public:
		GifImageSource(int width, int height, Platform::IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ repeatBehavior);
		//GifImageSource(int width, int height, IBox<RepeatBehavior>^ repeatBehavior);
		virtual ~GifImageSource();

		/// <summary>
		/// Gets the width of the image.
		/// </summary>
		property int Width
		{
			int get() { return m_width; }
		}

		/// <summary>
		/// Gets the height of the image.
		/// </summary>
		property int Height
		{
			int get() { return m_height; }
		}

		/// <summary>
		/// Starts the animation, if image is animated.
		/// </summary>
		void Start();

		/// <summary>
		/// Stops the animation.
		/// </summary>
		void Stop();

		/// <summary>
		/// Resets the animation to the first frame.
		/// </summary>
		void Restart();


		/// <summary>
		/// Loads the image from the specified image stream.
		/// </summary>
		Windows::Foundation::IAsyncAction^ SetSourceAsync(Windows::Storage::Streams::IRandomAccessStream^ pStream);


	internal:
		/// <summary>
		/// Stops the animation and clears all resources
		/// </summary>
		void StopAndClear();
		/// <summary>
		/// Clears all resources currently held in-memory.
		/// </summary>
		void ClearResources();

	private:
#define DISPOSAL_UNSPECIFIED      0       /* No disposal specified. */
#define DISPOSE_DO_NOT            1       /* Leave image in place */
#define DISPOSE_BACKGROUND        2       /* Set area too background color */
#define DISPOSE_PREVIOUS          3       /* Restore to previous content */

#define MAX_MEMORY_KILOBYTES_PER_GIF	10240	/* We allocated a max of 10 megabytes of pixel memory per gif, if it is over, the entire GIF have to be decoded in realtime. */
#define MAX_CACHED_FRAMES_PER_GIF	50 /* Caching frames usually uses less cpu power than realtime rendering, but it slows down when caching too many frames, so we limit it */
#define FRAMECOUNT_TO_PRERENDER	5

		ComPtr<ID2D1Bitmap1> m_surfaceBitmap;
		ComPtr<IWICImagingFactory> m_pIWICFactory;
		ComPtr<ID2D1Bitmap>            m_pRawFrame;
		ComPtr<ID2D1Bitmap>            m_pPreviousRawFrame;
		ComPtr<IWICBitmapDecoder> m_pDecoder;

		UINT m_width;
		UINT m_height;
		UINT m_dwFrameCount;
		bool m_isAnimatedGif;
		UINT m_completedLoopCount;
		UINT m_dwCurrentFrame;
		UINT m_dwPreviousFrame;
		UINT m_bitsPerPixel;
		UINT m_cachedKB;
		UINT m_millisSinceLastMemoryCheck;
		bool m_haveReservedDeviceResources;
		bool m_canCacheMoreFrames;
		bool m_isCachingFrames;
		bool m_isDestructing;
		bool m_isRunningRenderTask;


		Platform::IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ m_repeatBehavior;

		Windows::Foundation::EventRegistrationToken m_memoryTickToken;
		Windows::Foundation::EventRegistrationToken m_tickToken;
		Windows::Foundation::EventRegistrationToken m_durationTickToken;

		std::vector<ComPtr<ID2D1Bitmap>> m_bitmaps;
		std::vector<ComPtr<ID2D1Bitmap>> m_realtimeBitmapBuffer;
		std::vector<D2D1_POINT_2F> m_offsets;
		std::vector<USHORT> m_delays;
		std::vector<USHORT> m_disposals;

		concurrency::timer<int> *m_durationTimer;
		concurrency::timer<int> *m_memoryTimer;

		/// <summary>
		/// Renders a single frame and increments the current frame index.
		/// </summary>
		bool RenderFrame();
		void CreateDeviceResources(boolean forceRecreate);
		bool BeginDraw();
		void EndDraw();
		void WaitForAsync(IAsyncAction ^A);
		void CheckMemoryLimits();
		void StartDurationTimer();
		void StopDurationTimer();
		void StartMemoryTimer();
		void StopMemoryTimer();
		long SetNextInterval();
		void SelectNextFrame();
		void LoadImage(IStream* pStream);
		void CopyCurrentFrameToBitmap();

		cancellation_token_source cancellationTokenSource;
		IAsyncAction^ GetRawFramesTask(int startFrame, int endFrame);

		IAsyncAction^ OnTick();
		HRESULT QueryMetadata(IWICMetadataQueryReader *pQueryReader);
		HRESULT ReadGifApplicationExtension(IWICMetadataQueryReader *pQueryReader);
		HRESULT GetRawFrame(int uFrameIndex);
	};
}
