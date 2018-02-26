#include "pch.h"
#include "AnimationBehavior.h"
#include <Robuffer.h>
#include "Utilities.h"
#include "Effect.h"

using namespace GifImage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace concurrency;
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

Platform::Collections::Map<Platform::String^, int>^ AnimationBehavior::s_loadingImages;
Windows::Web::Http::HttpClient^ AnimationBehavior::s_defaultHttpClient;
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

DependencyProperty^ AnimationBehavior::s_imageOpenedEventTokenProperty = DependencyProperty::Register(
	"ImageOpenedEventToken",
	Windows::Foundation::EventRegistrationToken::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr)
);
Windows::Foundation::EventRegistrationToken AnimationBehavior::GetImageOpenedEventToken(UIElement ^ element)
{
	Windows::Foundation::EventRegistrationToken val = safe_cast<Windows::Foundation::EventRegistrationToken>(element->GetValue(s_imageOpenedEventTokenProperty));
	return val;
}

void GifImage::AnimationBehavior::SetImageOpenedEventToken(UIElement ^ element, Windows::Foundation::EventRegistrationToken value)
{
	element->SetValue(s_imageOpenedEventTokenProperty, value);
}

#pragma endregion
#pragma region Attached Properties
DependencyProperty^ AnimationBehavior::s_renderEffectValueProperty = DependencyProperty::RegisterAttached(
	"RenderEffects",
	Windows::Foundation::Collections::IVector<IEffectDescription^>::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&AnimationBehavior::s_renderEffectChanged))
);

Windows::Foundation::Collections::IVector<GifImage::IEffectDescription^>^ AnimationBehavior::GetRenderEffects(UIElement^ element)
{

	auto val = (Windows::Foundation::Collections::IVector<GifImage::IEffectDescription^>^)element->GetValue(s_renderEffectValueProperty);
	return val;
}

void AnimationBehavior::SetRenderEffects(UIElement^ element, Windows::Foundation::Collections::IVector<GifImage::IEffectDescription^>^ value)
{
	element->SetValue(s_renderEffectValueProperty, value);
}

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
DependencyProperty^ AnimationBehavior::s_httpClientValueProperty = DependencyProperty::RegisterAttached(
	"CustomHttpClient",
	HttpClient::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr)
);

HttpClient^ AnimationBehavior::GetHttpClient(UIElement^ element)
{
	auto val = dynamic_cast<HttpClient^>(element->GetValue(s_httpClientValueProperty));
	return val;
}

void AnimationBehavior::SetHttpClient(UIElement^ element, HttpClient^ value)
{
	element->SetValue(s_httpClientValueProperty, value);
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
	element->SetValue(s_httpClientValueProperty, nullptr);
	element->SetValue(s_imageUriValueProperty, value);
}
void AnimationBehavior::SetImageUriSource(UIElement^ element, Uri^ value, HttpClient^ client)
{
	element->SetValue(s_httpClientValueProperty, client);
	element->SetValue(s_imageUriValueProperty, value);
}
void AnimationBehavior::s_imageUriChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ args)
{
	Image^ image = (Image^)d;
	if (image != nullptr)
	{
		Uri^ uriSource = dynamic_cast<Uri^>(args->NewValue);
		if (uriSource == nullptr)
			OutputDebugString(("Set uri source on image " + image->GetHashCode() + ": Null\r\n")->Data());
		else
			OutputDebugString(("Set uri source on image " + image->GetHashCode() + ": " + uriSource->AbsoluteUri + "\r\n")->Data());
		AnimationBehavior::InitAnimation(image, uriSource);
	}
}


DependencyProperty^ AnimationBehavior::s_imageStreamValueProperty = DependencyProperty::RegisterAttached(
	"ImageStreamSource",
	IRandomAccessStream::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr)
);

IRandomAccessStream^ AnimationBehavior::GetImageStreamSource(UIElement^ element)
{
	auto val = dynamic_cast<IRandomAccessStream^>(element->GetValue(s_imageStreamValueProperty));
	return val;
}

Windows::Foundation::IAsyncAction^ AnimationBehavior::SetImageStreamSource(UIElement^ element, IRandomAccessStream^ value)
{
	element->SetValue(s_imageStreamValueProperty, value);
	Image^ image = (Image^)element;
	if (value == nullptr)
		OutputDebugString(L"Set stream source on image: Null\r\n");
	else
		OutputDebugString(L"Set stream source on image\r\n");

	return AnimationBehavior::InitAnimation(image, value);
}

void GifImage::AnimationBehavior::s_repeatBehaviorChanged(DependencyObject ^ d, DependencyPropertyChangedEventArgs ^ args)
{
	Image^ image = (Image^)d;
	GifImageSource^ gifImageSource = dynamic_cast<GifImageSource^>(image->Source);
	if (gifImageSource != nullptr)
	{
		if (GetImageUriSource(image) != nullptr)
			InitAnimation(image, GetImageUriSource(image));
		else if (GetImageStreamSource(image) != nullptr)
			InitAnimation(image, GetImageStreamSource(image));
	}
}
void GifImage::AnimationBehavior::s_renderEffectChanged(DependencyObject ^ d, DependencyPropertyChangedEventArgs ^ args)
{
	Image^ image = (Image^)d;
	GifImageSource^ gifImageSource = dynamic_cast<GifImageSource^>(image->Source);
	if (gifImageSource != nullptr)
	{
		std::vector<Effect> effects;
		if (args->NewValue != nullptr) {
			auto renderEffects = (Windows::Foundation::Collections::IVector<GifImage::IEffectDescription^>^)args->NewValue;
			for each (GifImage::IEffectDescription^ desc in renderEffects)
			{
				effects.push_back(Effect(desc));
			}
		}
		gifImageSource->SetRenderEffects(effects);
	}
}

DependencyProperty^ AnimationBehavior::s_handleNonGifImagesProperty = DependencyProperty::RegisterAttached(
	"HandleNonGifImages",
	Platform::Boolean::typeid,
	AnimationBehavior::typeid,
	ref new PropertyMetadata(nullptr)
);

Platform::Boolean GifImage::AnimationBehavior::GetHandleNonGifImages(Windows::UI::Xaml::UIElement ^ element)
{
	auto val = safe_cast<Platform::Boolean>(element->GetValue(s_handleNonGifImagesProperty));
	return val;
}

void GifImage::AnimationBehavior::SetHandleNonGifImages(Windows::UI::Xaml::UIElement ^ element, Platform::Boolean value)
{
	element->SetValue(s_handleNonGifImagesProperty,value);
}
#pragma endregion
GifImageSource^ AnimationBehavior::GetGifImageSource(UIElement^ element)
{
	auto img = dynamic_cast<Image^>(element);
	if (img != nullptr)
	{
		return dynamic_cast<GifImageSource^>(img->Source);
	}
	return nullptr;
}

Windows::Foundation::IAsyncAction^ AnimationBehavior::InitAnimation(UIElement^ img, IRandomAccessStream^ streamSource)
{
	Image^ image = (Image^)img;
	if (image == nullptr)
		return nullptr;

	ClearImageSource(image);

	return create_async([streamSource, image]()
	{
		std::shared_ptr<Concurrency::event> completed = std::make_shared<concurrency::event>();
		image->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([streamSource, image, completed]()->void
		{
			auto ui = task_continuation_context::use_current();

			if (streamSource != nullptr)
			{
				if (Windows::ApplicationModel::DesignMode::DesignModeEnabled)
				{
					BitmapImage^ bitmap = ref new BitmapImage();
					bitmap->SetSourceAsync(streamSource);
					image->Source = bitmap;
					completed->set();
					return;
				}
				try
				{
					auto loadStreamTask = GetGifImageSourceFromStream(image, streamSource);
					loadStreamTask.then([image, streamSource, completed, ui](GifImageSource^ gifImageSource)
					{
						if (gifImageSource != nullptr)
						{
							//Only load the source if the stream source is unchanged
							if (GetImageStreamSource(image) == streamSource)
							{
								image->Source = gifImageSource;
								if (GetAutoStart(image) == true)
									gifImageSource->Start();
								OnImageLoaded(image, gifImageSource);
							}
							else
							{
								OutputDebugString(L"Cancelled GetGifImageSourceFromStream because the source has changed\r\n");
							}
						}
					}, ui).then([image, streamSource, completed, ui](task<void> t)
					{
						bool success = false;
						try
						{
							t.get();
							success = true;
						}
						catch (Exception^ ex)
						{
							OutputDebugString(("GetGifImageSourceFromStream failed with error: " + ex->Message + "\r\n")->Data());
							OnError(image, "GifImageSource load failed with error: " + ex->ToString());
						}

						if (!success)
						{
							try
							{
								if (GetImageStreamSource(image) == streamSource)
								{
									if (streamSource->CanRead)
									{
										streamSource->Seek(0);
										BitmapImage^ bitmap = ref new BitmapImage();
										image->Source = bitmap;
										bitmap->SetSourceAsync(streamSource);
									}
								}
							}
							catch (Exception^ ex)
							{
								OutputDebugString(("Setting BitmapImage from stream failed with error: " + ex->Message + "\r\n")->Data());
							}
						}
						completed->set();
					}, ui);

				}
				catch (Exception^ ex)
				{
					if (streamSource != nullptr)
					{
						streamSource->Seek(0);
						BitmapImage^ bitmap = ref new BitmapImage();
						bitmap->SetSourceAsync(streamSource);
						image->Source = bitmap;
					}
					Utilities::ui_task(image->Dispatcher, [image, streamSource, ex]() {

						OnError(image, "GifImageSource load failed with error: " + ex->ToString());
					});
					completed->set();
				}
			}
		}));
		completed->wait();
	});
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
			auto ui = task_continuation_context::use_current();
			getFileTask.then([uriSource, image, ui](StorageFile^ file)
			{
				LoadSourceFromStorageFile(image, file, uriSource);
			}, ui).then([image, ui, uriSource](task<void> t)
			{
				try
				{
					t.get();
				}
				catch (Exception^ ex)
				{
					image->Source = ref new BitmapImage(uriSource);
					OnError(image, "GifImageSource load failed with error: " + ex->ToString());
				}

			}, ui);
		}
		else if (uriSource->SchemeName == "http" || uriSource->SchemeName == "https")
		{

			auto getFileTask = AnimationBehavior::GetStorageFileForImageCache(uriSource);
			auto ui = task_continuation_context::use_current();
			getFileTask.then([uriSource, image, ui](StorageFile^ file)
			{
				if (GetImageUriSource(image) == uriSource)
				{

					concurrency::task<void>([uriSource, image, file, ui]()
					{
						if (s_loadingImages == nullptr)
							s_loadingImages = ref new Platform::Collections::Map<Platform::String^, int>();
						if (!s_loadingImages->HasKey(file->Name))
						{
							s_loadingImages->Insert(file->Name, 1);
							OutputDebugString(("Adding to sync list: " + file->Name + "\r\n")->Data());
						}
						else
						{
							int waitMillis = 5;
							int i = 0;
							while (s_loadingImages->HasKey(file->Name))
							{
								wait(waitMillis);
								OutputDebugString(("Waiting for sync on file: " + file->Name + "\r\n")->Data());
								i++;
								if (waitMillis*i > 60000)//timeout after one minute
								{
									throw ref new Exception(E_ABORT);
								}
							}
						}
					}, ui).then([uriSource, image, file, ui]()
					{
						task<Windows::Storage::FileProperties::BasicProperties^>(file->GetBasicPropertiesAsync())
							.then([uriSource, file, image](Windows::Storage::FileProperties::BasicProperties^ properties)
						{
							if (properties != nullptr && properties->Size > 0)
							{

								LoadSourceFromStorageFile(image, file, uriSource);
								if (s_loadingImages->HasKey(file->Name))
									s_loadingImages->Remove(file->Name);
							}
							else
							{
								if (GetImageUriSource(image) == uriSource)
								{
									auto httpClient = GetHttpClient(image);
									if (httpClient == nullptr)
									{
										httpClient = DefaultHttpClient;
										if (httpClient == nullptr)
										{
											httpClient = ref new HttpClient();
										}
									}
									task<HttpResponseMessage^>(httpClient->GetAsync(uriSource))
										.then([file, image, uriSource](HttpResponseMessage^ message)
									{
										if (message->EnsureSuccessStatusCode())
										{
											task<IBuffer^>(message->Content->ReadAsBufferAsync())
												.then([file, image, uriSource](IBuffer^ buffer)
											{
												task<void>(FileIO::WriteBufferAsync(file, buffer))
													.then([file, image, uriSource]()
												{
													LoadSourceFromStorageFile(image, file, uriSource);
												}).then([file, image, uriSource](task<void> t)
												{
													try
													{
														t.get();
													}
													catch (Exception^ ex)
													{
														image->Source = ref new BitmapImage(uriSource);

														OnError(image, "GifImageSource cache write failed with error: " + ex->ToString());
													}
													if (s_loadingImages->HasKey(file->Name))
														s_loadingImages->Remove(file->Name);
													OutputDebugString(("Removing from sync list: " + file->Name + "\r\n")->Data());
												});
											}).then([image, uriSource](task<void> t)
											{
												try
												{
													t.get();
												}
												catch (Exception^ ex)
												{
													image->Source = ref new BitmapImage(uriSource);
													OnError(image, "GifImageSource stream read failed with error: " + ex->ToString());
												}
											});
										}
									}).then([file, image, uriSource](task<void> t)
									{
										try
										{
											t.get();
										}
										catch (Exception^ ex)
										{
											OutputDebugString(("Removing from sync list: " + file->Name + "\r\n")->Data());
											if (s_loadingImages->HasKey(file->Name))
												s_loadingImages->Remove(file->Name);
											file->DeleteAsync();
											image->Source = ref new BitmapImage(uriSource);
											OnError(image, "GifImageSource httpclient get failed with error: " + ex->ToString());
										}
									});
								}
								else
									OutputDebugString(L"Cancelled CreateDownload\r\n");
							}
						}).then([image, ui, uriSource](task<void> t)
						{
							try
							{
								t.get();
							}
							catch (Exception^ ex)
							{
								image->Source = ref new BitmapImage(uriSource);
								OnError(image, "GifImageSource load failed with error: " + ex->ToString());
							}
						}, ui);
					}, ui).then([image, ui, uriSource](task<void> t)
					{
						try
						{
							t.get();
						}
						catch (Exception^ ex)
						{
							image->Source = ref new BitmapImage(uriSource);
							OnError(image, "GifImageSource load failed with error: " + ex->ToString());
						}
					}, ui);
				}
				else
					OutputDebugString(L"Cancelled GetBasicPropertiesAsync\r\n");
			}, ui).then([image, ui, uriSource](task<void> t)
			{
				try
				{
					t.get();
				}
				catch (Exception^ ex)
				{
					image->Source = ref new BitmapImage(uriSource);
					OnError(image, "GifImageSource load failed with error: " + ex->ToString());
				}
			}, ui);
		}
		else
		{
			image->Source = ref new BitmapImage(uriSource);
			OnError(image, "Image URI is not in a valid format.");
		}
	}
}

concurrency::task<StorageFile^> AnimationBehavior::GetStorageFileForImageCache(Uri^ uriSource)
{
	StorageFolder^ tempFolder = ApplicationData::Current->TemporaryFolder;
	return task<StorageFolder^>(tempFolder->CreateFolderAsync("GifImageSource", Windows::Storage::CreationCollisionOption::OpenIfExists))
		.then([uriSource](StorageFolder^ folder)
	{
		Platform::String^ cacheName = Utilities::GetCacheFileName(uriSource->AbsoluteUri);
		OutputDebugString(("Creating cache file: " + cacheName + "\r\n")->Data());
		return folder->CreateFileAsync(cacheName, CreationCollisionOption::OpenIfExists);
	});
}

void AnimationBehavior::ClearImageSource(UIElement^ element)
{
	Image^ image = (Image^)element;
	auto source = image->Source;

	if (source != nullptr)
	{
		try
		{
			GifImageSource^ src = dynamic_cast<GifImageSource^>(source);
			if (src != nullptr)
			{
				src->StopAndClear();
			}

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
			//Only load the source if the url is unchanged
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
			{
				OutputDebugString(L"Cancelled LoadFromStorageFile because the source has changed\r\n");
			}
		}
		else
		{
			OnError(image, "Could not create GifImageSource, source was not set.");
		}

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
			OutputDebugString((ex->Message + "\r\n")->Data());
			OnError(image, "GifImageSource load failed with error: " + ex->ToString());
		}

		if (!success)
		{
			if (GetImageUriSource(image) == uriSource)
			{
				BitmapImage^ bitmap = ref new BitmapImage(uriSource);
				image->Source = bitmap;
				auto token = image->ImageOpened += ref new Windows::UI::Xaml::RoutedEventHandler(&GifImage::AnimationBehavior::OnImageOpened);
				SetImageOpenedEventToken(image, token);
			}
		}
	});
}

void GifImage::AnimationBehavior::OnImageOpened(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	Image^ image = (Image^)sender;
	auto token = GetImageOpenedEventToken(image);
	image->ImageOpened -= token;
	BitmapImage^bitmap = dynamic_cast<BitmapImage^>(image->Source);

	if (bitmap != nullptr)
	{
		OnImageLoaded(image, bitmap);
	}
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

	std::string signature = Utilities::ReadStringFromStream(pStream, 16);

	Utilities::ImageFileType type = Utilities::getImageTypeByMagic(signature.data());

	UINT width = 0;
	UINT height = 0;

	LARGE_INTEGER li;
	switch (type)
	{
	case Utilities::IMAGE_FILE_GIF:
		li.QuadPart = 6;
		pStream->Seek(li, STREAM_SEEK_SET, nullptr);
		width = Utilities::ReadIntFromStream(pStream, 2);
		height = Utilities::ReadIntFromStream(pStream, 2);
		break;
	case Utilities::IMAGE_FILE_JPG:
	case Utilities::IMAGE_FILE_TIFF:
	case Utilities::IMAGE_FILE_BMP:
	case Utilities::IMAGE_FILE_PNG:
	{
		if (!GetHandleNonGifImages(element)) {
			throw ref new InvalidArgumentException("File is not a valid GIF file");
		}
		li.QuadPart = 0;
		pStream->Seek(li, STREAM_SEEK_SET, nullptr);
		auto d2dManager = Direct2DManager::GetInstance(Window::Current->GetHashCode());
		// WIC Decoder
		ComPtr<IWICBitmapDecoder> decoder;
		DX::ThrowIfFailed(
			d2dManager->GetIWICFactory()->CreateDecoderFromStream(
				pStream,
				nullptr,
				WICDecodeMetadataCacheOnDemand,
				&decoder
			)
		);

		// Frame
		ComPtr<IWICBitmapFrameDecode> frame;
		DX::ThrowIfFailed(
			decoder->GetFrame(0, &frame)
		);
		DX::ThrowIfFailed(frame->GetSize(&width, &height));
	}
	break;
	default:
		throw ref new InvalidArgumentException("File is not a valid image file");
		break;
	}

	IBox<Windows::UI::Xaml::Media::Animation::RepeatBehavior>^ repeatBehavior = nullptr;

	try
	{
		repeatBehavior = GetRepeatBehavior(element);
	}
	catch (Platform::Exception^ ex) {}

	Windows::Foundation::Collections::IVector<GifImage::IEffectDescription^>^ renderEffects = nullptr;
	try
	{
		renderEffects = GetRenderEffects(element);
	}
	catch (Platform::Exception^ ex) {}

	GifImageSource^ imageSource = ref new GifImageSource(width, height, repeatBehavior);

	if (renderEffects) {

		std::vector<Effect> effects;
		for each (GifImage::IEffectDescription^ desc in renderEffects)
		{
			effects.push_back(Effect(desc));
		}

		imageSource->SetRenderEffects(effects);
	}
	li.QuadPart = 0;
	pStream->Seek(li, STREAM_SEEK_SET, nullptr);
	auto setSourceTask = create_task(imageSource->SetSourceAsync(pStream, type));

	return	setSourceTask.then([imageSource]()->GifImageSource^
	{
		return imageSource;
	});
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
		if (Utilities::IsLoaded(image))
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
				OutputDebugString(("Unloaded image" + " (" + image->GetHashCode() + ")" + " with source: " + GetImageUriSource(image)->AbsoluteUri + "\r\n")->Data());
			}
		}
	}
}


