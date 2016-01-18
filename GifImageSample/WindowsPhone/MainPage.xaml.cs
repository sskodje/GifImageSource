using GifImage;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
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
            this.DataContext = this;
            this.InitializeComponent();
            GifImage.AnimationBehavior.OnError += AnimationBehavior_OnError;
            List<MyModel> items = new List<MyModel>();

            items.AddRange(Enumerable.Range(1, 23).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList());
            items.Add(new MyModel(new Uri("http://i.imgur.com/9Bo0CZi.gif")));
            items.Add(new MyModel(new Uri("https://media.giphy.com/media/xT77XR3gI2c7NiDzEY/giphy.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/canvas_prev.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/dl_world_anim.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_opt/bunny_bgnd_lzw_gifsicle.gif")));
            items.Add(new MyModel(new Uri("https://upload.wikimedia.org/wikipedia/commons/thumb/a/a8/Tour_Eiffel_Wikimedia_Commons.jpg/800px-Tour_Eiffel_Wikimedia_Commons.jpg")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/YHoBqLR.gif")));

            this.cbGifs.ItemsSource = items;
            this.cbGifs.SelectedItem = items[0];
        }

        private void AnimationBehavior_OnError(object sender, string str)
        {
            Debug.WriteLine(str);
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.
        /// This parameter is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
          //  GifImage.AnimationBehavior.OnError += AnimationBehavior_OnError;
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
           // GifImage.AnimationBehavior.OnError-= AnimationBehavior_OnError;
        }
        private async void AppBarButtonLoad_Click(object sender, RoutedEventArgs e)
        {
            //  string uri = "http://i.imgur.com/0GNs9Lt.jpg";
            // string uri = "http://i.imgur.com/YHoBqLR.gif";
            //  string uri = "ms-appx:///Gifs/19.gif";
            //  XamlAnimatedGif.AnimationBehavior.SetSourceUri(_gifImage, new Uri(uri));
            //   GifImage.AnimationBehavior.SetImageUriSource(_gifImage, new Uri(uri));    
            //    StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(new Uri(uri));
            //  GifImage.AnimationBehavior.SetImageStreamSource(_gifImage, await file.OpenReadAsync());

            await OpenGif(((MyModel)this.cbGifs.SelectedItem).Uri);
        }

        private void AppBarButtonUnload_Click(object sender, RoutedEventArgs e)
        {
            GifImage.AnimationBehavior.SetImageUriSource(_gifImage, null);
        }

        private void AppBarButtonPlay_Click(object sender, RoutedEventArgs e)
        {
            GifImageSource source = GifImage.AnimationBehavior.GetGifImageSource(_gifImage);
            if (source != null)
                source.Start();
        }

        private void AppBarButtonPause_Click(object sender, RoutedEventArgs e)
        {
            GifImageSource source = GifImage.AnimationBehavior.GetGifImageSource(_gifImage);
            if (source != null)
                source.Stop();
        }

        private void AppBarButtonOpenListViewTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(ListViewTest), 150);
        }

        private void AppBarButtonOpenGridViewTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(GridViewTest), 20);
        }


        private async Task OpenGif(Uri uri)
        {
            //  string uri = "ms-appx:///Gifs/19.gif";
            //  XamlAnimatedGif.AnimationBehavior.SetSourceUri(_gifImage, new Uri(uri));
               GifImage.AnimationBehavior.SetImageUriSource(_gifImage,uri);    
            //StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(uri);
            //GifImage.AnimationBehavior.SetImageStreamSource(_gifImage, await file.OpenReadAsync());
        }

        private async void cbGifs_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
           
            if (e.AddedItems != null)
            {
                await OpenGif(((MyModel)e.AddedItems[0]).Uri);
            }
        }

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            GifImage.AnimationBehavior.OnError -= AnimationBehavior_OnError;
        }
    }
}
