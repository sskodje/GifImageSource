#pragma once
#include <wincodec.h>
#include "windows.foundation.h"
#include "Direct2DManager.h"

using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::Foundation;
using namespace Platform;;
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
		/// Renders a single frame and increments the current frame index.
		/// </summary>
		/// <returns>
		/// True if an animation loop was just completed, false otherwise.
		/// </returns>
		bool RenderFrame();
		/// <summary>
		/// Loads the image from the specified image stream.
		/// </summary>
		Windows::Foundation::IAsyncAction^ SetSourceAsync(Windows::Storage::Streams::IRandomAccessStream^ pStream);


	internal:
		/// <summary>
		/// Clears all resources currently held in-memory.
		/// </summary>
		void ClearResources();
		void SetRepeatBehavior(Platform::IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ repeatBehavior);

	private:
#define DISPOSAL_UNSPECIFIED      0       /* No disposal specified. */
#define DISPOSE_DO_NOT            1       /* Leave image in place */
#define DISPOSE_BACKGROUND        2       /* Set area too background color */
#define DISPOSE_PREVIOUS          3       /* Restore to previous content */

		void CreateDeviceResources(boolean forceRecreate);
		bool BeginDraw();
		void EndDraw();

		void StartDurationTimer();
		void StopDurationTimer();
		void SetNextInterval();
		void SelectNextFrame();
		void LoadImage(IStream* pStream);
		HRESULT QueryMetadata(IWICMetadataQueryReader *pQueryReader);
		HRESULT ReadGifApplicationExtension(IWICMetadataQueryReader *pQueryReader);

		ComPtr<ID2D1Bitmap1> m_surfaceBitmap;
		UINT m_width;
		UINT m_height;
		UINT m_dwFrameCount;
		//UINT m_loopCount;
		bool m_isAnimatedGif;
		UINT m_completedLoopCount;
		UINT m_dwCurrentFrame;
		UINT m_dwPreviousFrame;
		UINT m_bitsPerPixel;
		bool m_haveReservedDeviceResources;
		Platform::IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ m_repeatBehavior;

		Windows::Foundation::EventRegistrationToken m_tickToken;
		Windows::Foundation::EventRegistrationToken m_durationTickToken;
		ComPtr<IStream> m_stream;
		std::vector<ComPtr<ID2D1Bitmap>> m_bitmaps;
		std::vector<D2D1_POINT_2F> m_offsets;
		std::vector<USHORT> m_delays;
		std::vector<USHORT> m_disposals;

		Windows::UI::Xaml::DispatcherTimer^ m_renderTimer;
		Windows::UI::Xaml::DispatcherTimer^ m_durationTimer;
		void OnTick(Platform::Object ^sender, Platform::Object ^args);
		void OnDurationEndedTick(Platform::Object ^sender, Platform::Object ^args);
		
	};
}
