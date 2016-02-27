#pragma once
#include "GifImageSource.h"
#include <shcore.h>
#include <ppl.h>

namespace GifImage
{
	ref class AnimationBehavior;
	public delegate void ErrorEventHandler(Platform::Object^ sender, Platform::String^ error);
	public delegate void ImageLoadedEventHandler(Platform::Object^ sender, GifImageSource^ imageSource);
	public ref class AnimationBehavior sealed
	{
	public:
		AnimationBehavior();
		virtual~AnimationBehavior();

		static property Windows::UI::Xaml::DependencyProperty^ ImageUri
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_imageUriValueProperty;
			}
		};
		static Windows::Foundation::Uri^ GetImageUriSource(Windows::UI::Xaml::UIElement^ element);
		static void SetImageUriSource(Windows::UI::Xaml::UIElement^ element, Windows::Foundation::Uri^ value);

		static property Windows::UI::Xaml::DependencyProperty^ ImageStream
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_imageStreamValueProperty;
			}
		};
		static  Windows::Storage::Streams::IRandomAccessStream^ GetImageStreamSource(Windows::UI::Xaml::UIElement^ element);
		static void SetImageStreamSource(Windows::UI::Xaml::UIElement^ element, Windows::Storage::Streams::IRandomAccessStream^ value);

		static property Windows::UI::Xaml::DependencyProperty^ RepeatBehavior
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_repeatBehaviorProperty;
			}
		};

		static Windows::UI::Xaml::Media::Animation::RepeatBehavior  GetRepeatBehavior(Windows::UI::Xaml::UIElement^ element);
		static void SetRepeatBehavior(Windows::UI::Xaml::UIElement^ element, Windows::UI::Xaml::Media::Animation::RepeatBehavior value);

		static property Windows::UI::Xaml::DependencyProperty^ AutoStart
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_autoStartProperty;
			}
		};

		static Platform::Boolean  GetAutoStart(Windows::UI::Xaml::UIElement^ element);
		static void SetAutoStart(Windows::UI::Xaml::UIElement^ element, Platform::Boolean value);




		static GifImageSource^ GetGifImageSource(Windows::UI::Xaml::UIElement^ element);
		static event ImageLoadedEventHandler^ OnImageLoaded;
		static event ErrorEventHandler^ OnError;

	private:
		static void s_imageUriChanged(Windows::UI::Xaml::DependencyObject^ target, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ args);
		static void s_imageStreamChanged(Windows::UI::Xaml::DependencyObject^ target, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ args);
		static void s_repeatBehaviorChanged(Windows::UI::Xaml::DependencyObject^ target, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ args);
		static Windows::UI::Xaml::DependencyProperty^ s_imageUriValueProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_imageStreamValueProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_imageLoadedEventTokenProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_imageUnloadedEventTokenProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_repeatBehaviorProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_autoStartProperty;


		static void LoadSourceFromStorageFile(Windows::UI::Xaml::UIElement^ element, Windows::Storage::IStorageFile^ file, Windows::Foundation::Uri^ uriSource);
		static concurrency::task<GifImageSource^> GetGifImageSourceFromStorageFile(Windows::UI::Xaml::UIElement^ element, Windows::Storage::IStorageFile^ file, Windows::Foundation::Uri^ uriSource);
		static concurrency::task<GifImageSource^> GetGifImageSourceFromStream(Windows::UI::Xaml::UIElement^ element, Windows::Storage::Streams::IRandomAccessStream^ stream);

		static Windows::Foundation::EventRegistrationToken GetImageLoadedEventToken(Windows::UI::Xaml::UIElement^ element);
		static void SetImageLoadedEventToken(Windows::UI::Xaml::UIElement^ element, Platform::Object^ value);

		static Windows::Foundation::EventRegistrationToken GetImageUnloadedEventToken(Windows::UI::Xaml::UIElement^ element);
		static void SetImageUnloadedEventToken(Windows::UI::Xaml::UIElement^ element, Windows::Foundation::EventRegistrationToken value);

		static bool IsLoaded(Windows::UI::Xaml::FrameworkElement^ element);
		static void InitAnimation(Windows::UI::Xaml::UIElement^ img, Windows::Foundation::Uri^ uriSource);
		static void InitAnimation(Windows::UI::Xaml::UIElement^ img, Windows::Storage::Streams::IRandomAccessStream^ streamSource);
		static void ClearImageSource(Windows::UI::Xaml::UIElement^ element);


		static void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		static void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);

	};
}

