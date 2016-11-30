using GifImage;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkID=390556

namespace GifImageSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {

            this.InitializeComponent();
            var client = new HttpClient();
            client.DefaultRequestHeaders.Authorization = new Windows.Web.Http.Headers.HttpCredentialsHeaderValue("Bearer", "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJuYW1laWQiOiIxIiwidW5pcXVlX25hbWUiOiJBdXN0aW5ubm5ubm5tbW1tbW1tbW1tbW1tbW1tbSIsImh0dHA6Ly9zY2hlbWFzLm1pY3Jvc29mdC5jb20vYWNjZXNzY29udHJvbHNlcnZpY2UvMjAxMC8wNy9jbGFpbXMvaWRlbnRpdHlwcm92aWRlciI6IkFTUC5ORVQgSWRlbnRpdHkiLCJBc3BOZXQuSWRlbnRpdHkuU2VjdXJpdHlTdGFtcCI6IjBkOGQ3Y2FkLTBhNjUtNDBiMC04Y2ZkLTZhZjNiOTY3YmI3YyIsImVtYWlsIjoic3Nrb2RqZUBnbWFpbC5jb20iLCJpc3MiOiJodHRwOi8vaml2ZW1lc3Nlbmdlci5jb20iLCJhdWQiOiJodHRwOi8vaml2ZW1lc3Nlbmdlci5jb20vYXBpLyIsImV4cCI6MTQ3OTE0ODQ4OCwibmJmIjoxNDc5MDYyMDg4fQ.NGJZh_o-L9RpDSrI-2S1gdySwWQ6Djwfx-lxo9imZJE");
          AnimationBehavior.DefaultHttpClient = client;
            
            this.DataContext = this;
        }


        private void AnimationBehavior_OnImageLoaded(object sender, ImageSource src)
        {
            Image img = (Image)sender;

            if (img.Source is GifImageSource)
            {
                ((GifImageSource)img.Source).OnFrameChanged -= MainPage_OnFrameChanged;
                ((GifImageSource)img.Source).OnFrameChanged += MainPage_OnFrameChanged;
            }
            _progressBar.Value = 0;
            BusyIndicator.IsActive = false;
        }

        private void AnimationBehavior_OnError(object sender, string error)
        {
            Debug.WriteLine(error);

            BusyIndicator.IsActive = false;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.
        /// This parameter is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            AnimationBehavior.OnError += AnimationBehavior_OnError;
            AnimationBehavior.OnImageLoaded += AnimationBehavior_OnImageLoaded;
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            AnimationBehavior.OnError -= AnimationBehavior_OnError;
            AnimationBehavior.OnImageLoaded -= AnimationBehavior_OnImageLoaded;
            AnimationBehavior.SetImageUriSource(_gifImage, null);

        }
        private void AppBarButtonLoad_Click(object sender, RoutedEventArgs e)
        {
            OpenGif(((MyModel)this.cbGifs.SelectedItem).Uri);
        }

        private void AppBarButtonUnload_Click(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.SetImageUriSource(_gifImage, null);
            _progressBar.Value=0;
        }

        private void AppBarButtonPlay_Click(object sender, RoutedEventArgs e)
        {
            GifImageSource source = AnimationBehavior.GetGifImageSource(_gifImage);
            if (source != null)
                source.Start();
        }

        private void AppBarButtonPause_Click(object sender, RoutedEventArgs e)
        {
            GifImageSource source = AnimationBehavior.GetGifImageSource(_gifImage);
            if (source != null)
                source.Pause();
        }

        private void AppBarButtonOpenListViewTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(ListViewTest), 150);
        }

        private void AppBarButtonOpenGridViewTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(GridViewTest), 12);
        }

        private void OpenGif(Uri uri)
        {

            AnimationBehavior.SetImageUriSource(_gifImage, uri);
            BusyIndicator.IsActive = true;
        }

        private void cbGifs_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems != null)
            {
                OpenGif(((MyModel)e.AddedItems[0]).Uri);
            }
        }
        DateTime _lastFrameChange;
        private void MainPage_OnFrameChanged(object sender)
        {
            GifImageSource gifImage = (GifImageSource)sender;
            _progressBar.Value = gifImage.CurrentFrame;
            _progressBar.Maximum = gifImage.FrameCount;
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            List<MyModel> items = MyModel.CreateTestModels();
            this.cbGifs.ItemsSource = items;
            this.cbGifs.SelectedItem = items[0];

        }

        private void AppBarButtonOpenSingleEmoticonTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(SingleGifTest));
        }

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {

        }

    }
}
