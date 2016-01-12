#include "pch.h"
#include "AnimationBehavior.h"



using namespace GifImage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace concurrency;
using namespace Windows::Networking::BackgroundTransfer;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;

AnimationBehavior::AnimationBehavior()
{
}


AnimationBehavior::~AnimationBehavior()
{
}

GifImageSource^ AnimationBehavior::GetGifImageSource(UIElement^ element)
{
	Image^ img = safe_cast<Image^>(element);
	if(img!= nullptr)
	{
		if(img->Source!= nullptr)
		{
			GifImageSource^ src = safe_cast<GifImageSource^>(img->Source);
			if (src != nullptr)
				return src;
		}
	}
	return nullptr;
}

//DependencyProperty^ AnimationBehavior::s_imageLoadedEventToken = DependencyProperty::Register(
//	"ImageLoadedEventToken",
//	Windows::Foundation::EventRegistrationToken::typeid,
//	AnimationBehavior::typeid,
//	ref new PropertyMetadata(nullptr)
//	);
//
//Windows::Foundation::EventRegistrationToken AnimationBehavior::GetImageLoadedEventToken(UIElement^ element)
//{
//	auto val = safe_cast<Windows::Foundation::EventRegistrationToken>(element->GetValue(s_imageLoadedEventToken));
//	return val;
//}
//
//void GifImage::AnimationBehavior::SetImageLoadedEventToken(UIElement ^ element, Object^ value)
//{
//	element->SetValue(s_imageLoadedEventToken, value);
//}
//
//DependencyProperty^ AnimationBehavior::s_imageUnloadedEventToken = DependencyProperty::Register(
//	"ImageUnloadedEventToken",
//	Windows::Foundation::EventRegistrationToken::typeid,
//	AnimationBehavior::typeid,
//	ref new PropertyMetadata(nullptr)
//	);
//Windows::Foundation::EventRegistrationToken AnimationBehavior::GetImageUnloadedEventToken(UIElement ^ element)
//{
//	Windows::Foundation::EventRegistrationToken val = safe_cast<Windows::Foundation::EventRegistrationToken>(element->GetValue(s_imageUnloadedEventToken));
//	return val;
//}
//
//void GifImage::AnimationBehavior::SetImageUnloadedEventToken(UIElement ^ element, Windows::Foundation::EventRegistrationToken value)
//{
//	element->SetValue(s_imageUnloadedEventToken, value);
//}

DependencyProperty^ AnimationBehavior::s_imageUriValue = DependencyProperty::RegisterAttached(
	"ImageUriSource",
	Uri::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&AnimationBehavior::s_imageUriChanged))
	);

Uri^ AnimationBehavior::GetImageUriSource(UIElement^ element)
{
	auto val = safe_cast<Uri^>(element->GetValue(s_imageUriValue));
	return val;
}

void AnimationBehavior::SetImageUriSource(UIElement^ element, Uri^ value)
{
	element->SetValue(s_imageUriValue, value);
}
void AnimationBehavior::s_imageUriChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ args)
{
	Image^ image = (Image^)d;
	Uri^ uriSource = safe_cast<Uri^>(args->NewValue);
	if (uriSource == nullptr)
		OutputDebugString(L"Set uri source on image: Null\r\n");
	else
		OutputDebugString(("Set uri source on image: " + uriSource->AbsoluteUri + "\r\n")->Data());
	AnimationBehavior::InitAnimation(image, uriSource);
}


DependencyProperty^ AnimationBehavior::s_imageStreamValue = DependencyProperty::RegisterAttached(
	"ImageStreamSource",
	IRandomAccessStream::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&AnimationBehavior::s_imageStreamChanged))
	);

IRandomAccessStream^ AnimationBehavior::GetImageStreamSource(UIElement^ element)
{
	auto val = safe_cast<IRandomAccessStream^>(element->GetValue(s_imageStreamValue));
	return val;
}

void AnimationBehavior::SetImageStreamSource(UIElement^ element, IRandomAccessStream^ value)
{
	element->SetValue(s_imageStreamValue, value);
}
void AnimationBehavior::s_imageStreamChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ args)
{
	Image^ image = (Image^)d;
	IRandomAccessStream^ streamSource = safe_cast<IRandomAccessStream^>(args->NewValue);
	if (streamSource == nullptr)
		OutputDebugString(L"Set stream source on image: Null\r\n");
	else
		OutputDebugString(L"Set stream source on image\r\n");

	AnimationBehavior::InitAnimation(image, streamSource);
}
void AnimationBehavior::InitAnimation(UIElement^ img, IRandomAccessStream^ streamSource)
{
	Image^ image = (Image^)img;
	if (image == nullptr)
		return;
	if (streamSource != nullptr)
	{
		auto loadStreamTask = GetGifImageSourceFromStream(image, streamSource);
		loadStreamTask.then([image, streamSource](GifImageSource^ source)
		{
			if (source != nullptr)
			{
				if (GetImageStreamSource(image) == streamSource)
				{
					image->Source = source;
					source->Start();
				}
				else
					OutputDebugString(L"Cancelled GetGifImageSourceFromStream\r\n");
			}
		});
	}
	else
	{
		ClearImageSource(image);
	}
}
void AnimationBehavior::InitAnimation(UIElement^ img, Uri^ uriSource)
{
	Image^ image = (Image^)img;
	if (image == nullptr)
		return;

	if (uriSource != nullptr)
	{
		if (IsLoaded(image))
		{
			Windows::Foundation::EventRegistrationToken token = image->Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(&GifImage::AnimationBehavior::OnUnloaded);
			//AnimationBehavior::SetImageUnloadedEventToken(image, token);
		}
		else
		{
			Windows::Foundation::EventRegistrationToken token = image->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(&GifImage::AnimationBehavior::OnLoaded);
			//AnimationBehavior::SetImageLoadedEventToken(image, token);
			return;
		}

		ClearImageSource(image);
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
			wsstr = removeForbiddenChar(wsstr);
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
									}).then([file](task<void> t)
									{
										bool success = false;
										try
										{
											t.get();
											success = true;
										}
										catch (Exception^ ex)
										{
											OnError(nullptr, "GifImageSource load failed with error: " + ex->ToString());
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
			OnError(nullptr, "Image URI is not in a valid format, Image Source was not set");
	}
	else
	{
		ClearImageSource(image);
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
			auto  src = dynamic_cast<GifImageSource^>(image->Source);

			if (src != nullptr)
			{
				src->Stop();
				src->ClearResources();
			}
		}
		catch (Exception^ ex)
		{
			OnError(nullptr, "ClearImageSource failed with error: " + ex->ToString());
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
				imageSource->Start();
			}
			else
				OutputDebugString(L"Cancelled LoadFromStorageFile\r\n");
		}
		else
			OnError(nullptr, "Could not create GifImageSource, source was not set.");

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
			OnError(nullptr, "GifImageSource load failed with error: " + ex->ToString());
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
	auto createDecoderTask = create_task(BitmapDecoder::CreateAsync(stream));
	return	createDecoderTask.then([stream](BitmapDecoder^ decoder)
	{
		GifImageSource^ imageSource = ref new GifImageSource(decoder->PixelWidth, decoder->PixelHeight);

		auto setSourceTask = create_task(imageSource->SetSourceAsync(stream));
		return	setSourceTask.then([imageSource]()->GifImageSource^
		{
			return imageSource;
		});
	});
}

bool AnimationBehavior::IsLoaded(FrameworkElement^ element)
{
	return Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(element) != nullptr;
}

void AnimationBehavior::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	auto image = (Image^)sender;
	//auto token = GetImageLoadedEventToken(image);

	//image->Loaded -= token;
	//image->ClearValue(s_imageLoadedEventToken);

	Uri^ sourceUri = GetImageUriSource(image);
	OutputDebugString(("Loaded image with source: " + sourceUri + "\r\n")->Data());

	InitAnimation(image, sourceUri);
}

void AnimationBehavior::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	Image^ image = (Image^)sender;
	if (image != nullptr)
	{
		//try
		//{
			//auto token = GetImageUnloadedEventToken(image);
			//image->Unloaded -= token;
			//image->ClearValue(s_imageUnloadedEventToken);
		//}
		//catch (Platform::Exception^ ex)
		//{
		//	OnError(nullptr, ex->ToString());
		//}
		image->ClearValue(s_imageUriValue);
		if (image->Source != nullptr)
		{

			//auto loadedToken = image->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(&GifImage::AnimationBehavior::OnLoaded);
			//SetImageLoadedEventToken(image, loadedToken);

			OutputDebugString(("Unloaded image with source: " + GetImageUriSource(image)->AbsoluteUri + "\r\n")->Data());
			//AnimationBehavior::ClearImageSource(image);
		}
	}
}


std::wstring AnimationBehavior::removeForbiddenChar(std::wstring s)
{
	const std::string illegalChars = "\\/:?\"<>|";
	std::wstring::iterator it;
	for (it = s.begin(); it < s.end(); ++it) {
		bool found = illegalChars.find(*it) != std::string::npos;
		if (found) {
			*it = '_';
		}
	}
	return s;
}