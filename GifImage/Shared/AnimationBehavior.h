#pragma once
#include "GifImageSource.h"
#include <shcore.h>
#include <ppltasks.h>
#include <ppl.h>

using namespace Platform;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
namespace GifImage
{
	ref class AnimationBehavior;
	public delegate void ErrorEventHandler(Object^ sender, Platform::String^ s);

	public ref class AnimationBehavior sealed
	{
	public:
		AnimationBehavior();
		virtual~AnimationBehavior();

		static property DependencyProperty^ ImageUri
		{
			DependencyProperty^ get()
			{
				return s_imageUriValue;
			}
		};
		static Uri^ GetImageUriSource(UIElement^ element);
		static void SetImageUriSource(UIElement^ element, Uri^ value);

		static property DependencyProperty^ ImageStream
		{
			DependencyProperty^ get()
			{
				return s_imageStreamValue;
			}
		};
		static IRandomAccessStream^ GetImageStreamSource(UIElement^ element);
		static void SetImageStreamSource(UIElement^ element, IRandomAccessStream^ value);

		static GifImageSource^ GetGifImageSource(UIElement^ element);

		static event ErrorEventHandler^ OnError;

	private:
		static void s_imageUriChanged(DependencyObject^ target, DependencyPropertyChangedEventArgs^ args);
		static void s_imageStreamChanged(DependencyObject^ target, DependencyPropertyChangedEventArgs^ args);
		static DependencyProperty^ s_imageUriValue;
		static DependencyProperty^ s_imageStreamValue;
		//static DependencyProperty^ s_imageLoadedEventToken;
		//static DependencyProperty^ s_imageUnloadedEventToken;

		static void LoadSourceFromStorageFile(UIElement^ element, IStorageFile^ file, Uri^ uriSource);
		static concurrency::task<GifImageSource^> GetGifImageSourceFromStorageFile(UIElement^ element, IStorageFile^ file, Uri^ uriSource);
		static concurrency::task<GifImageSource^> GetGifImageSourceFromStream(UIElement^ element, IRandomAccessStream^ stream);

		//static Windows::Foundation::EventRegistrationToken GetImageLoadedEventToken(UIElement^ element);
		//static void SetImageLoadedEventToken(UIElement^ element, Object^ value);

		//static Windows::Foundation::EventRegistrationToken GetImageUnloadedEventToken(UIElement^ element);
		//static void SetImageUnloadedEventToken(UIElement^ element, Windows::Foundation::EventRegistrationToken value);

		static bool IsLoaded(FrameworkElement^ element);
		static void InitAnimation(UIElement^ img, Uri^ uriSource);
		static void InitAnimation(UIElement^ img, IRandomAccessStream^ streamSource);
		static void ClearImageSource(UIElement^ element);
		static std::wstring removeForbiddenChar(std::wstring s);

		static void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		static void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);

	};
}

