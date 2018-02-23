#pragma once
#include "GifImageSource.h"
#include "Effect.h"
#include <shcore.h>
#include <ppl.h>
#include "IEffectDescription.h";


namespace GifImage
{
	public delegate void ErrorEventHandler(Platform::Object^ sender, Platform::String^ error);
	public delegate void ImageLoadedEventHandler(Platform::Object^ sender, Windows::UI::Xaml::Media::ImageSource^ imageSource);

	public ref class AnimationBehavior sealed
	{
	public:
		static property Windows::Web::Http::HttpClient^ DefaultHttpClient
		{
			Windows::Web::Http::HttpClient^ get()
			{
				return s_defaultHttpClient;
			}
			void set(Windows::Web::Http::HttpClient^ value)
			{
				s_defaultHttpClient = value;
			}
		}

		static property Windows::UI::Xaml::DependencyProperty^ RenderEffectProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_renderEffectValueProperty;
			}
		};
		static Windows::Foundation::Collections::IVector<GifImage::IEffectDescription^>^ GetRenderEffects(Windows::UI::Xaml::UIElement^ element);
		static void SetRenderEffects(Windows::UI::Xaml::UIElement^ element, Windows::Foundation::Collections::IVector<GifImage::IEffectDescription^>^ value);

		static property Windows::UI::Xaml::DependencyProperty^ CustomHttpClientProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_httpClientValueProperty;
			}
		};
		static Windows::Web::Http::HttpClient^ GetHttpClient(Windows::UI::Xaml::UIElement^ element);
		static void SetHttpClient(Windows::UI::Xaml::UIElement^ element, Windows::Web::Http::HttpClient^ value);

		static property Windows::UI::Xaml::DependencyProperty^ ImageUriProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_imageUriValueProperty;
			}
		};
		static Windows::Foundation::Uri^ GetImageUriSource(Windows::UI::Xaml::UIElement^ element);
		static void SetImageUriSource(Windows::UI::Xaml::UIElement^ element, Windows::Foundation::Uri^ value);
		static void SetImageUriSource(Windows::UI::Xaml::UIElement^ element, Windows::Foundation::Uri^ value,Windows::Web::Http::HttpClient^ client);

		static property Windows::UI::Xaml::DependencyProperty^ ImageStreamProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_imageStreamValueProperty;
			}
		};
		static  Windows::Storage::Streams::IRandomAccessStream^ GetImageStreamSource(Windows::UI::Xaml::UIElement^ element);
		static Windows::Foundation::IAsyncAction^ SetImageStreamSource(Windows::UI::Xaml::UIElement^ element, Windows::Storage::Streams::IRandomAccessStream^ value);

		static property Windows::UI::Xaml::DependencyProperty^ RepeatBehaviorProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return s_repeatBehaviorProperty;
			}
		};

		static Windows::UI::Xaml::Media::Animation::RepeatBehavior  GetRepeatBehavior(Windows::UI::Xaml::UIElement^ element);
		static void SetRepeatBehavior(Windows::UI::Xaml::UIElement^ element, Windows::UI::Xaml::Media::Animation::RepeatBehavior value);

		static property Windows::UI::Xaml::DependencyProperty^ AutoStartProperty
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
		AnimationBehavior();
		~AnimationBehavior();

		static void s_imageUriChanged(Windows::UI::Xaml::DependencyObject^ target, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ args);
		static void s_repeatBehaviorChanged(Windows::UI::Xaml::DependencyObject^ target, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ args);
		static void s_renderEffectChanged(Windows::UI::Xaml::DependencyObject^ target, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ args);

		static Windows::Web::Http::HttpClient^ s_defaultHttpClient;

		static Windows::UI::Xaml::DependencyProperty^ s_renderEffectValueProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_httpClientValueProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_imageUriValueProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_imageStreamValueProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_imageLoadedEventTokenProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_imageUnloadedEventTokenProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_imageOpenedEventTokenProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_repeatBehaviorProperty;
		static Windows::UI::Xaml::DependencyProperty^ s_autoStartProperty;

		static Platform::Collections::Map<Platform::String^,int>^ s_loadingImages;

		static void LoadSourceFromStorageFile(Windows::UI::Xaml::UIElement^ element, Windows::Storage::IStorageFile^ file, Windows::Foundation::Uri^ uriSource);
		static concurrency::task<GifImageSource^> GetGifImageSourceFromStorageFile(Windows::UI::Xaml::UIElement^ element, Windows::Storage::IStorageFile^ file, Windows::Foundation::Uri^ uriSource);
		static concurrency::task<GifImageSource^> GetGifImageSourceFromStream(Windows::UI::Xaml::UIElement^ element, Windows::Storage::Streams::IRandomAccessStream^ stream);

		static concurrency::task<Windows::Storage::StorageFile^> GetStorageFileForImageCache(Windows::Foundation::Uri^ uriSource);

		static Windows::Foundation::EventRegistrationToken GetImageLoadedEventToken(Windows::UI::Xaml::UIElement^ element);
		static void SetImageLoadedEventToken(Windows::UI::Xaml::UIElement^ element, Platform::Object^ value);

		static Windows::Foundation::EventRegistrationToken GetImageUnloadedEventToken(Windows::UI::Xaml::UIElement^ element);
		static void SetImageUnloadedEventToken(Windows::UI::Xaml::UIElement^ element, Windows::Foundation::EventRegistrationToken value);

		static Windows::Foundation::EventRegistrationToken GetImageOpenedEventToken(Windows::UI::Xaml::UIElement^ element);
		static void SetImageOpenedEventToken(Windows::UI::Xaml::UIElement^ element, Windows::Foundation::EventRegistrationToken value);

		static void InitAnimation(Windows::UI::Xaml::UIElement^ img, Windows::Foundation::Uri^ uriSource);
		static Windows::Foundation::IAsyncAction^ InitAnimation(Windows::UI::Xaml::UIElement^ img, Windows::Storage::Streams::IRandomAccessStream^ streamSource);
		static void ClearImageSource(Windows::UI::Xaml::UIElement^ element);

		static void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		static void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		static void OnImageOpened(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
};
}

