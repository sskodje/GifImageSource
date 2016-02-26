using GifImage;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
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
            this.DataContext = this;
            this.InitializeComponent();
            AnimationBehavior.OnError += AnimationBehavior_OnError;
            AnimationBehavior.OnImageLoaded += AnimationBehavior_OnImageLoaded;
            List<MyModel> items = new List<MyModel>();

            items.AddRange(Enumerable.Range(1, 23).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList());
            items.Add(new MyModel(new Uri("http://i.imgur.com/9Bo0CZi.gif")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/qeu10ds.gif")));
            items.Add(new MyModel(new Uri("https://media.giphy.com/media/xT77XR3gI2c7NiDzEY/giphy.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/canvas_bgnd.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/dl_world_anim.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_opt/bunny_bgnd_lzw_gifsicle.gif")));
            items.Add(new MyModel(new Uri("https://upload.wikimedia.org/wikipedia/commons/thumb/a/a8/Tour_Eiffel_Wikimedia_Commons.jpg/800px-Tour_Eiffel_Wikimedia_Commons.jpg")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/YHoBqLR.gif")));
            this.cbGifs.ItemsSource = items;
            this.cbGifs.SelectedItem = items[0];
        }

        private void AnimationBehavior_OnImageLoaded(object sender, GifImageSource imageSource)
        {
            BusyIndicator.IsActive = false;
        }

        private void AnimationBehavior_OnError(object sender, string str)
        {
            Debug.WriteLine(str);
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
        }
        private void AppBarButtonLoad_Click(object sender, RoutedEventArgs e)
        {
            OpenGif(((MyModel)this.cbGifs.SelectedItem).Uri);
        }

        private void AppBarButtonUnload_Click(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.SetImageUriSource(_gifImage, null);
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
            this.Frame.Navigate(typeof(GridViewTest), 16);
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

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.OnError -= AnimationBehavior_OnError;
        }
    }
}
