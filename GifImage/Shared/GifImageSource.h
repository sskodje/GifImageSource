#pragma once
#include <wincodec.h>
#include "windows.foundation.h"
#include "Direct2DManager.h"

namespace GifImage
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class GifImageSource sealed : Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
	{
	public:
		GifImageSource(int width, int height);
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

	private:
#define DISPOSAL_UNSPECIFIED      0       /* No disposal specified. */
#define DISPOSE_DO_NOT            1       /* Leave image in place */
#define DISPOSE_BACKGROUND        2       /* Set area too background color */
#define DISPOSE_PREVIOUS          3       /* Restore to previous content */

		void CreateDeviceResources(boolean forceRecreate);
		bool BeginDraw();
		void EndDraw();

		//void SetupPreviousFrame();
		void SetNextInterval();
		void SelectNextFrame();
		void LoadImage(IStream* pStream);
		HRESULT QueryMetadata(IWICMetadataQueryReader *pQueryReader);

	//	Direct2DManager* m_d2dManager;

		ComPtr<ID2D1Bitmap1> m_surfaceBitmap;
		UINT m_width;
		UINT m_height;
		UINT m_dwFrameCount;
		UINT m_loopCount;
		bool m_isAnimatedGif;
		bool m_completedLoop;
		UINT m_dwCurrentFrame;
		UINT m_dwPreviousFrame;
		bool m_haveReservedDeviceResources;

		Windows::Foundation::EventRegistrationToken m_TickToken;

		std::vector<ComPtr<ID2D1Bitmap>> m_bitmaps;
		std::vector<D2D1_POINT_2F> m_offsets;
		std::vector<USHORT> m_delays;
		std::vector<USHORT> m_disposals;

		Windows::UI::Xaml::DispatcherTimer^ m_timer;
		void OnTick(Platform::Object ^sender, Platform::Object ^args);
	};
}
