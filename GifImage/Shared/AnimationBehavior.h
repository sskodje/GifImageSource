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
using namespace Windows::UI::Xaml::Media::Animation;
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
				return s_imageUriValueProperty;
			}
		};
		static Uri^ GetImageUriSource(UIElement^ element);
		static void SetImageUriSource(UIElement^ element, Uri^ value);

		static property DependencyProperty^ ImageStream
		{
			DependencyProperty^ get()
			{
				return s_imageStreamValueProperty;
			}
		};
		static IRandomAccessStream^ GetImageStreamSource(UIElement^ element);
		static void SetImageStreamSource(UIElement^ element, IRandomAccessStream^ value);

		static property DependencyProperty^ RepeatBehavior
		{
			DependencyProperty^ get()
			{
				return s_repeatBehaviorProperty;
			}
		};

		static Windows::UI::Xaml::Media::Animation::RepeatBehavior  GetRepeatBehavior(UIElement^ element);
		static void SetRepeatBehavior(UIElement^ element, Windows::UI::Xaml::Media::Animation::RepeatBehavior value);

		static property DependencyProperty^ AutoStart
		{
			DependencyProperty^ get()
			{
				return s_autoStartProperty;
			}
		};

		static Platform::Boolean  GetAutoStart(UIElement^ element);
		static void SetAutoStart(UIElement^ element, Platform::Boolean value);

		static GifImageSource^ GetGifImageSource(UIElement^ element);

		static event ErrorEventHandler^ OnError;

	private:
		static void s_imageUriChanged(DependencyObject^ target, DependencyPropertyChangedEventArgs^ args);
		static void s_imageStreamChanged(DependencyObject^ target, DependencyPropertyChangedEventArgs^ args);
		static void s_repeatBehaviorChanged(DependencyObject^ target, DependencyPropertyChangedEventArgs^ args);
		static DependencyProperty^ s_imageUriValueProperty;
		static DependencyProperty^ s_imageStreamValueProperty;
		static DependencyProperty^ s_imageLoadedEventTokenProperty;
		static DependencyProperty^ s_imageUnloadedEventTokenProperty;
		static DependencyProperty^ s_repeatBehaviorProperty;
		static DependencyProperty^ s_autoStartProperty;

		static void LoadSourceFromStorageFile(UIElement^ element, IStorageFile^ file, Uri^ uriSource);
		static concurrency::task<GifImageSource^> GetGifImageSourceFromStorageFile(UIElement^ element, IStorageFile^ file, Uri^ uriSource);
		static concurrency::task<GifImageSource^> GetGifImageSourceFromStream(UIElement^ element, IRandomAccessStream^ stream);

		static Windows::Foundation::EventRegistrationToken GetImageLoadedEventToken(UIElement^ element);
		static void SetImageLoadedEventToken(UIElement^ element, Object^ value);

		static Windows::Foundation::EventRegistrationToken GetImageUnloadedEventToken(UIElement^ element);
		static void SetImageUnloadedEventToken(UIElement^ element, Windows::Foundation::EventRegistrationToken value);

		static bool IsLoaded(FrameworkElement^ element);
		static void InitAnimation(UIElement^ img, Uri^ uriSource);
		static void InitAnimation(UIElement^ img, IRandomAccessStream^ streamSource);
		static void ClearImageSource(UIElement^ element);


		static void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		static void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);

	};
}

