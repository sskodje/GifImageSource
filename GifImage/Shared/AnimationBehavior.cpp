#include "pch.h"
#include "AnimationBehavior.h"
#include "Robuffer.h"
#include "Utilities.h";




using namespace GifImage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace concurrency;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;
using namespace Platform;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Microsoft::WRL;

AnimationBehavior::AnimationBehavior()
{
}


AnimationBehavior::~AnimationBehavior()
{
}
#pragma region Dependency Properties


DependencyProperty^ AnimationBehavior::s_autoStartProperty = DependencyProperty::Register(
	"AutoStart",
	Windows::Foundation::EventRegistrationToken::typeid,
	Platform::Boolean::typeid,
	ref new PropertyMetadata(true)
	);

Platform::Boolean AnimationBehavior::GetAutoStart(UIElement^ element)
{
	auto val = safe_cast<Platform::Boolean>(element->GetValue(s_autoStartProperty));
	return val;
}

void GifImage::AnimationBehavior::SetAutoStart(UIElement ^ element, Platform::Boolean value)
{
	element->SetValue(s_autoStartProperty, value);
}

DependencyProperty^ AnimationBehavior::s_imageLoadedEventTokenProperty = DependencyProperty::Register(
	"ImageLoadedEventToken",
	Windows::Foundation::EventRegistrationToken::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr)
	);

Windows::Foundation::EventRegistrationToken AnimationBehavior::GetImageLoadedEventToken(UIElement^ element)
{
	auto val = safe_cast<Windows::Foundation::EventRegistrationToken>(element->GetValue(s_imageLoadedEventTokenProperty));
	return val;
}

void GifImage::AnimationBehavior::SetImageLoadedEventToken(UIElement ^ element, Object^ value)
{
	element->SetValue(s_imageLoadedEventTokenProperty, value);
}

DependencyProperty^ AnimationBehavior::s_imageUnloadedEventTokenProperty = DependencyProperty::Register(
	"ImageUnloadedEventToken",
	Windows::Foundation::EventRegistrationToken::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr)
	);
Windows::Foundation::EventRegistrationToken AnimationBehavior::GetImageUnloadedEventToken(UIElement ^ element)
{
	Windows::Foundation::EventRegistrationToken val = safe_cast<Windows::Foundation::EventRegistrationToken>(element->GetValue(s_imageUnloadedEventTokenProperty));
	return val;
}

void GifImage::AnimationBehavior::SetImageUnloadedEventToken(UIElement ^ element, Windows::Foundation::EventRegistrationToken value)
{
	element->SetValue(s_imageUnloadedEventTokenProperty, value);
}
#pragma endregion
#pragma region Attached Properties
DependencyProperty^ AnimationBehavior::s_repeatBehaviorProperty = DependencyProperty::RegisterAttached(
	"RepeatBehavior",
	Windows::UI::Xaml::Media::Animation::RepeatBehavior::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(Windows::UI::Xaml::Media::Animation::RepeatBehavior(), ref new PropertyChangedCallback(&AnimationBehavior::s_repeatBehaviorChanged))
	);

Windows::UI::Xaml::Media::Animation::RepeatBehavior AnimationBehavior::GetRepeatBehavior(UIElement^ element)
{

	auto val = (Windows::UI::Xaml::Media::Animation::RepeatBehavior)element->GetValue(s_repeatBehaviorProperty);
	return val;
}

void AnimationBehavior::SetRepeatBehavior(UIElement^ element, Windows::UI::Xaml::Media::Animation::RepeatBehavior value)
{
	element->SetValue(s_repeatBehaviorProperty, value);
}

DependencyProperty^ AnimationBehavior::s_imageUriValueProperty = DependencyProperty::RegisterAttached(
	"ImageUriSource",
	Uri::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&AnimationBehavior::s_imageUriChanged))
	);

Uri^ AnimationBehavior::GetImageUriSource(UIElement^ element)
{
	auto val = dynamic_cast<Uri^>(element->GetValue(s_imageUriValueProperty));
	return val;
}

void AnimationBehavior::SetImageUriSource(UIElement^ element, Uri^ value)
{
	element->SetValue(s_imageUriValueProperty, value);
}
void AnimationBehavior::s_imageUriChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ args)
{
	Image^ image = (Image^)d;
	Uri^ uriSource = dynamic_cast<Uri^>(args->NewValue);
	if (uriSource == nullptr)
		OutputDebugString(L"Set uri source on image: Null\r\n");
	else
		OutputDebugString(("Set uri source on image: " + uriSource->AbsoluteUri + "\r\n")->Data());
	AnimationBehavior::InitAnimation(image, uriSource);
}


DependencyProperty^ AnimationBehavior::s_imageStreamValueProperty = DependencyProperty::RegisterAttached(
	"ImageStreamSource",
	IRandomAccessStream::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&AnimationBehavior::s_imageStreamChanged))
	);

IRandomAccessStream^ AnimationBehavior::GetImageStreamSource(UIElement^ element)
{
	auto val = dynamic_cast<IRandomAccessStream^>(element->GetValue(s_imageStreamValueProperty));
	return val;
}

void AnimationBehavior::SetImageStreamSource(UIElement^ element, IRandomAccessStream^ value)
{
	element->SetValue(s_imageStreamValueProperty, value);
}
void AnimationBehavior::s_imageStreamChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ args)
{
	Image^ image = (Image^)d;
	IRandomAccessStream^ streamSource = dynamic_cast<IRandomAccessStream^>(args->NewValue);
	if (streamSource == nullptr)
		OutputDebugString(L"Set stream source on image: Null\r\n");
	else
		OutputDebugString(L"Set stream source on image\r\n");

	AnimationBehavior::InitAnimation(image, streamSource);
}
void GifImage::AnimationBehavior::s_repeatBehaviorChanged(DependencyObject ^ d, DependencyPropertyChangedEventArgs ^ args)
{
	Image^ image = (Image^)d;
	if (image->Source != nullptr && image->Source->GetType() == GifImageSource::typeid)
	{
		if (GetImageUriSource(image) != nullptr)
			InitAnimation(image, GetImageUriSource(image));
		else if (GetImageStreamSource(image) != nullptr)
			InitAnimation(image, GetImageStreamSource(image));
	}
}
#pragma endregion

GifImageSource^ AnimationBehavior::GetGifImageSource(UIElement^ element)
{
	Image^ img = safe_cast<Image^>(element);
	if (img != nullptr)
	{
		if (img->Source != nullptr)
		{
			GifImageSource^ src = safe_cast<GifImageSource^>(img->Source);
			if (src != nullptr)
				return src;
		}
	}
	return nullptr;
}

void AnimationBehavior::InitAnimation(UIElement^ img, IRandomAccessStream^ streamSource)
{
	Image^ image = (Image^)img;
	if (image == nullptr)
		return;
	ClearImageSource(image);
	if (streamSource != nullptr)
	{
		if (Windows::ApplicationModel::DesignMode::DesignModeEnabled)
		{
			BitmapImage^ bitmap = ref new BitmapImage();
			bitmap->SetSourceAsync(streamSource);
			image->Source = bitmap;
			return;
		}
		auto loadStreamTask = GetGifImageSourceFromStream(image, streamSource);
		loadStreamTask.then([image, streamSource](GifImageSource^ source)
		{
			if (source != nullptr)
			{
				if (GetImageStreamSource(image) == streamSource)
				{
					image->Source = source;
					if (GetAutoStart(image) == true)
						source->Start();
					OnImageLoaded(image, source);
				}
				else
					OutputDebugString(L"Cancelled GetGifImageSourceFromStream\r\n");
			}
		});
	}
}
void AnimationBehavior::InitAnimation(UIElement^ img, Uri^ uriSource)
{
	Image^ image = (Image^)img;
	if (image == nullptr)
		return;
	if (Windows::ApplicationModel::DesignMode::DesignModeEnabled)
	{
		BitmapImage^ bitmap = ref new BitmapImage(uriSource);
		image->Source = bitmap;
		return;
	}
	ClearImageSource(image);
	if (uriSource != nullptr)
	{
		if (uriSource->SchemeName == "ms-appx" || uriSource->SchemeName == "ms-appdata")
		{
			auto getFileTask = create_task(StorageFile::GetFileFromApplicationUriAsync(uriSource));
			getFileTask.then([uriSource, image](StorageFile^ file)
			{
				LoadSourceFromStorageFile(image, file, uriSource);
			});
		}
		else if (uriSource->SchemeName == "http" || uriSource->SchemeName == "https")
		{
			StorageFolder^ folder = ApplicationData::Current->TemporaryFolder;
			String^ fileName = uriSource->AbsoluteUri;
			std::wstring wsstr(fileName->Data());
			wsstr = Utilities::RemoveForbiddenChar(wsstr);
			String^ sanitizedString = ref new String(wsstr.c_str(), wsstr.length());

			task<StorageFile^>(folder->CreateFileAsync(sanitizedString, CreationCollisionOption::OpenIfExists))
				.then([uriSource, image](StorageFile^ file)
			{
				if (GetImageUriSource(image) == uriSource)
				{
					task<Windows::Storage::FileProperties::BasicProperties^>(file->GetBasicPropertiesAsync())
						.then([uriSource, file, image](Windows::Storage::FileProperties::BasicProperties^ properties)
					{
						if (properties->Size > 0)
						{

							LoadSourceFromStorageFile(image, file, uriSource);
						}
						else
						{
							if (GetImageUriSource(image) == uriSource)
							{
								auto httpClient = ref new HttpClient();
								try
								{
									task<HttpResponseMessage^>(httpClient->GetAsync(uriSource)).then([file, image, uriSource](HttpResponseMessage^ message)
									{
										if (message->EnsureSuccessStatusCode())
										{
											task<IBuffer^>(message->Content->ReadAsBufferAsync()).then([file, image, uriSource](IBuffer^ buffer)
											{
												task<void>(FileIO::WriteBufferAsync(file, buffer)).then([file, image, uriSource]()
												{
													LoadSourceFromStorageFile(image, file, uriSource);
												});
											});
										}
									}).then([file, image](task<void> t)
									{
										bool success = false;
										try
										{
											t.get();
											success = true;
										}
										catch (Exception^ ex)
										{
											OnError(image, "GifImageSource load failed with error: " + ex->ToString());
										}

										if (!success)
										{

											file->DeleteAsync();
										}
									});

								}
								catch (Exception^ ex)
								{
									// Details in ex.Message and ex.HResult.       
									file->DeleteAsync();
								}
							}
							else
								OutputDebugString(L"Cancelled CreateDownload\r\n");
						}
					});
				}
				else
					OutputDebugString(L"Cancelled GetBasicPropertiesAsync\r\n");
			});
		}
		else
			OnError(image, "Image URI is not in a valid format, Image Source was not set");
	}
}

void AnimationBehavior::ClearImageSource(UIElement^ element)
{
	Image^ image = (Image^)element;
	auto source = image->Source;

	if (source != nullptr)
	{
		try
		{
			auto src = (GifImageSource^)image->Source;
			src->StopAndClear();
		}
		catch (Exception^ ex)
		{
			OnError(image, "ClearImageSource failed with error: " + ex->ToString());
		}

		try
		{
			auto token = GetImageUnloadedEventToken(image);
			image->Unloaded -= token;
		}
		catch (...)
		{

		}
	}

	image->Source = nullptr;
}

void AnimationBehavior::LoadSourceFromStorageFile(UIElement^ element, IStorageFile^ file, Uri^ uriSource)
{
	Image^ image = (Image^)element;
	auto loadFileTask = GetGifImageSourceFromStorageFile(image, file, uriSource);
	loadFileTask.then([image, uriSource](GifImageSource^ imageSource)
	{
		if (imageSource != nullptr)
		{
			if (GetImageUriSource(image) == uriSource)
			{


				image->Source = imageSource;

				if (GetAutoStart(image) == true)
					imageSource->Start();
				OnImageLoaded(image, imageSource);
				auto token = image->Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(&GifImage::AnimationBehavior::OnUnloaded);
				SetImageUnloadedEventToken(image, token);
			}
			else
				OutputDebugString(L"Cancelled LoadFromStorageFile\r\n");
		}
		else
			OnError(image, "Could not create GifImageSource, source was not set.");

	}).then([image, uriSource](task<void> t)
	{
		bool success = false;
		try
		{
			t.get();
			success = true;
		}
		catch (Exception^ ex)
		{
			OutputDebugString(ex->Message->Data());
			OnError(image, "GifImageSource load failed with error: " + ex->ToString());
		}

		if (!success)
		{

			if (GetImageUriSource(image) == uriSource)
			{
				BitmapImage^ bitmap = ref new BitmapImage(uriSource);
				image->Source = bitmap;
			}
		}
	});
}

concurrency::task<GifImageSource^> AnimationBehavior::GetGifImageSourceFromStorageFile(UIElement^ element, IStorageFile^ file, Uri^ uriSource)
{
	Image^ image = (Image^)element;
	auto ui = task_continuation_context::use_current();

	auto readStreamTask = create_task(file->OpenReadAsync());
	return readStreamTask.then([ui, image, uriSource](IRandomAccessStream^ stream)
	{
		if (GetImageUriSource(image) == uriSource)
		{
			auto getSourceFromStreamTask = GetGifImageSourceFromStream(image, stream);
			return getSourceFromStreamTask;
		}
		else
		{
			OutputDebugString(L"Cancelled createDecoderTask\r\n");
			return task_from_result((GifImageSource^)nullptr);
		}
	}, ui);
}

concurrency::task<GifImageSource^> AnimationBehavior::GetGifImageSourceFromStream(UIElement^ element, IRandomAccessStream^ stream)
{
	ComPtr<IStream> pIStream;
	DX::ThrowIfFailed(
		CreateStreamOverRandomAccessStream(
			reinterpret_cast<IUnknown*>(stream),
			IID_PPV_ARGS(&pIStream)));

	auto pStream = pIStream.Get();
	//stream = nullptr;
	std::string str = Utilities::ReadStringFromStream(pStream, 3);
	if (str != "GIF")
		throw ref new InvalidArgumentException("File is not a valid GIF file");

	str = Utilities::ReadStringFromStream(pStream, 3);
	if (str != "89a" && str != "87a")
		throw ref new InvalidArgumentException("Unsupported GIF version");


	int width = Utilities::ReadIntFromStream(pStream, 2);
	int height = Utilities::ReadIntFromStream(pStream, 2);
	IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ repeatBehavior = nullptr;

	try
	{
		repeatBehavior = GetRepeatBehavior(element);
	}
	catch (Platform::Exception^ ex)
	{

	}

	GifImageSource^ imageSource = ref new GifImageSource(width, height, repeatBehavior);

	auto setSourceTask = create_task(imageSource->SetSourceAsync(stream));
	return	setSourceTask.then([imageSource]()->GifImageSource^
	{
		return imageSource;
	});
}

bool AnimationBehavior::IsLoaded(FrameworkElement^ element)
{
	return Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(element) != nullptr;
}

void AnimationBehavior::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	auto image = (Image^)sender;

	try
	{
		auto token = GetImageLoadedEventToken(image);
		image->Loaded -= token;
	}
	catch (InvalidCastException^ ex) {}

	if (GetImageUriSource(image) != nullptr)
	{
		Uri^ sourceUri = GetImageUriSource(image);
		InitAnimation(image, sourceUri);
		OutputDebugString(("Loaded image" + " (" + image->GetHashCode() + ")" + " with source: " + sourceUri + "\r\n")->Data());
	}
	else if (GetImageStreamSource(image) != nullptr)
	{
		auto streamSource = GetImageStreamSource(image);
		InitAnimation(image, streamSource);
		OutputDebugString(("Loaded image" + " (" + image->GetHashCode() + ")" + " with stream source\r\n")->Data());
	}
}

void AnimationBehavior::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	Image^ image = (Image^)sender;
	if (image != nullptr)
	{
		//If the image is still in the visual tree we need to back off.
		if (IsLoaded(image))
			return;
		if (image->Source != nullptr)
		{
			GifImageSource^  src = dynamic_cast<GifImageSource^>(image->Source);
			if (src != nullptr)
			{
				auto loadedToken = image->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(&GifImage::AnimationBehavior::OnLoaded);
				SetImageLoadedEventToken(image, loadedToken);
				try
				{
					auto unloadedToken = GetImageUnloadedEventToken(image);
					image->Unloaded -= unloadedToken;
				}
				catch (InvalidCastException^ ex)
				{

				}

				AnimationBehavior::ClearImageSource(image);

			}
			OutputDebugString(("Unloaded image" + " (" + image->GetHashCode() + ")" + " with source: " + GetImageUriSource(image)->AbsoluteUri + "\r\n")->Data());
		}
	}
}


