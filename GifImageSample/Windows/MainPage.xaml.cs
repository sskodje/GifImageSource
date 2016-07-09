using GifImage;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace GifImageSample
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            InitializeComponent();

        }

        private void AnimationBehavior_OnImageLoaded(object sender,ImageSource src)
        {
            Image img = (Image)sender;
            if (img.Source is GifImageSource)
            {
                ((GifImageSource)img.Source).OnFrameChanged -= MainPage_OnFrameChanged;
                ((GifImageSource)img.Source).OnFrameChanged += MainPage_OnFrameChanged;
            }
            _progressBar.Value=0;
            BusyIndicator.IsActive = false;
        }

        private void MainPage_OnFrameChanged(object sender)
        {
            GifImageSource gifImage = (GifImageSource)sender;
            _progressBar.Value = gifImage.CurrentFrame;
            //Debug.WriteLine("Changed frame to: "+gifImage.CurrentFrame);
            _progressBar.Maximum = gifImage.FrameCount;
        }

        private void AnimationBehavior_OnError(object sender, string error)
        {
            Debug.WriteLine(error);
            BusyIndicator.IsActive = false;
        }

        private void Load_Click(object sender, RoutedEventArgs e)
        {
            OpenGif(((MyModel)this.cbGifs.SelectedItem).Uri);
            AnimationBehavior.SetRepeatBehavior(_gifImage, new Windows.UI.Xaml.Media.Animation.RepeatBehavior(5));
        }
        private async void LoadAsStream_Click(object sender, RoutedEventArgs e)
        {
            await OpenGifAsStream(((MyModel)this.cbGifs.SelectedItem).Uri);
        }
        private void Unload_Click(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.SetImageUriSource(_gifImage, null);
            AnimationBehavior.SetImageStreamSource(_gifImage, null);

        }

        private void Play_Click(object sender, RoutedEventArgs e)
        {
            GifImageSource source = AnimationBehavior.GetGifImageSource(_gifImage);
            if (source != null)
                source.Start();
        }

        private void Pause_Click(object sender, RoutedEventArgs e)
        {
            GifImageSource source = AnimationBehavior.GetGifImageSource(_gifImage);
            if (source != null)
                source.Pause();
        }
        private void Stop_Click(object sender, RoutedEventArgs e)
        {
            GifImageSource source = AnimationBehavior.GetGifImageSource(_gifImage);
            if (source != null)
                source.Stop();
        }
        private void BnGridViewTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(GridViewTest), 48);
        }

        private void BnListViewTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(ListViewTest));
        }

        private void OpenGif(Uri uri)
        {
            AnimationBehavior.SetImageUriSource(_gifImage, uri);
            BusyIndicator.IsActive = true;
        }
        private async Task OpenGifAsStream(Uri uri)
        {
            if (uri.Scheme == "ms-appx" || uri.Scheme == "ms-appdata")
            {
                try
                {
                    BusyIndicator.IsActive = true;
                    StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(uri);
                    await AnimationBehavior.SetImageStreamSource(_gifImage, await file.OpenReadAsync());
                }
                catch(IOException ex)
                {
                    Debug.WriteLine(ex);
                    BusyIndicator.IsActive = false;
                }
            }
            else
            {
                BusyIndicator.IsActive = true;
                HttpClient client = new HttpClient();
                byte[] data = await client.GetByteArrayAsync(uri);
                IRandomAccessStream ras = data.AsBuffer().AsStream().AsRandomAccessStream();
                await AnimationBehavior.SetImageStreamSource(_gifImage, ras);
            }
        }
        private void cbGifs_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems != null)
            {
                OpenGif(((MyModel)e.AddedItems[0]).Uri);
            }
        }
        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.OnError += AnimationBehavior_OnError;
            AnimationBehavior.OnImageLoaded += AnimationBehavior_OnImageLoaded;
            List<MyModel> items = MyModel.CreateTestModels();

            this.cbGifs.ItemsSource = items;
            this.cbGifs.SelectedItem = items[0];
        }
        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.OnError -= AnimationBehavior_OnError;
            AnimationBehavior.OnImageLoaded -= AnimationBehavior_OnImageLoaded;
        }

        private void BnSingleGifTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(SingleGifTest));
        }
    }
}
