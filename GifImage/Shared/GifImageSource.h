#pragma once
#include <wincodec.h>
#include "windows.foundation.h"
#include "Direct2DManager.h"
#include <agents.h>
#include <chrono>

namespace GifImage
{
	public delegate void EventHandler(Platform::Object^ sender);
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class GifImageSource sealed : Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
	{
	public:
		GifImageSource(int width, int height, Platform::IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ repeatBehavior);
		virtual ~GifImageSource();

		event EventHandler^ OnAnimationCompleted;
		event EventHandler^ OnFrameChanged;

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
		/// Gets the current frame.
		/// </summary>
		property int CurrentFrame
		{
			int get() { return m_dwCurrentFrame; }
		}

		/// <summary>
		/// Gets the total number of frames.
		/// </summary>
		property int FrameCount
		{
			int get() { return m_dwFrameCount; }
		}

		/// <summary>
		/// Starts the animation, if image is animated.
		/// </summary>
		void Start();

		/// <summary>
		/// Pause the animation.
		/// </summary>
		void Pause();

		/// <summary>
		/// Stops the animation and return to first frame.
		/// </summary>
		void Stop();


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
#define FRAMECOUNT_TO_PRERENDER	5	/* The number of frames to cache ahead */
#define MEMORY_PERCENT_TO_STOP_FRAME_CACHE	50	/* The percent of total allowed app memory used before we stop caching frames */
#define MEMORY_PERCENT_TO_DELETE_FRAME_CACHE	70	/* The percent of total allowed app memory used before we delete all cached frames */

		Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_surfaceBitmap;
		Microsoft::WRL::ComPtr<IWICImagingFactory> m_pIWICFactory;
		Microsoft::WRL::ComPtr<ID2D1Bitmap>            m_pRawFrame;
		Microsoft::WRL::ComPtr<ID2D1Bitmap>            m_pPreviousRawFrame;
		Microsoft::WRL::ComPtr<IWICBitmapDecoder> m_pDecoder;

		UINT m_width;
		UINT m_height;
		UINT m_dwFrameCount;
		bool m_isAnimatedGif;
		UINT m_completedLoopCount;
		UINT m_dwCurrentFrame;
		UINT m_dwPreviousFrame;
		UINT m_bitsPerPixel;
		UINT m_cachedKB;
		ULONG m_lastMemoryCheckEpochTime;

		bool m_haveReservedDeviceResources;
		bool m_canCacheMoreFrames;
		bool m_isCachingFrames;
		bool m_isRendering;
		int m_windowID;
		std::chrono::high_resolution_clock::time_point  m_nextFrameTimePoint;


		Platform::IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ m_repeatBehavior;

		std::vector<Microsoft::WRL::ComPtr<ID2D1Bitmap>> m_bitmaps;
		std::vector<Microsoft::WRL::ComPtr<ID2D1Bitmap>> m_realtimeBitmapBuffer;
		std::vector<D2D1_POINT_2F> m_offsets;
		std::vector<USHORT> m_delays;
		std::vector<USHORT> m_disposals;

		concurrency::timer<int> *m_durationTimer;
		Windows::Foundation::EventRegistrationToken m_RenderingToken;

		concurrency::task<void> complete_after(unsigned int timeout);


		/// <summary>
		/// Renders a single frame and increments the current frame index.
		/// </summary>
		bool RenderFrame();
		void CreateDeviceResources(boolean forceRecreate);
		HRESULT BeginDraw();
		void EndDraw();
		void CheckMemoryLimits();
		void StartDurationTimer();
		void StopDurationTimer();
		long SetNextInterval();
		void SelectNextFrame();
		void LoadImage(IStream* pStream);

		void OnSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e);

		HRESULT CopyCurrentFrameToBitmap();

		concurrency::cancellation_token_source cancellationTokenSource;
		void GetRawFramesTask(int startFrame, int endFrame);

		long RenderAndPrepareFrame();
		HRESULT QueryMetadata(IWICMetadataQueryReader *pQueryReader);
		HRESULT ReadGifApplicationExtension(IWICMetadataQueryReader *pQueryReader);
		HRESULT GetRawFrame(int uFrameIndex);
		void OnRendering(Platform::Object ^sender, Platform::Object ^args);
	};
}
