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
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            AnimationBehavior.SetImageUriSource(_gifImage, null);

        }
        private void AppBarButtonLoad_Click(object sender, RoutedEventArgs e)
        {
            OpenGif(((MyModel)this.cbGifs.SelectedItem).Uri);
        }

        private void AppBarButtonUnload_Click(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.SetImageUriSource(_gifImage, null);
            //_gifImage.Source=null;
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

        private void MainPage_OnFrameChanged(object sender)
        {
            GifImageSource gifImage = (GifImageSource)sender;
            _progressBar.Value = gifImage.CurrentFrame;
            //Debug.WriteLine("Changed frame to: "+gifImage.CurrentFrame);
            _progressBar.Maximum = gifImage.FrameCount;
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            List<MyModel> items = MyModel.CreateTestModels();
            this.cbGifs.ItemsSource = items;
            this.cbGifs.SelectedItem = items[0];
            AnimationBehavior.OnError += AnimationBehavior_OnError;
            AnimationBehavior.OnImageLoaded += AnimationBehavior_OnImageLoaded;
        }

        private void AppBarButtonOpenSingleEmoticonTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(SingleGifTest));
        }

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.OnError -= AnimationBehavior_OnError;
            AnimationBehavior.OnImageLoaded -= AnimationBehavior_OnImageLoaded;
        }

    }
}
