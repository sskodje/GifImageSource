using GifImage;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace GifImageSample
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            InitializeComponent();
            GifImage.AnimationBehavior.OnError += AnimationBehavior_OnError;
            List<MyModel> items = new List<MyModel>();

            items.AddRange(Enumerable.Range(1, 23).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList());
            items.Add(new MyModel(new Uri("ms-appx:///Gifs/kitty.gif")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/9Bo0CZi.gif")));
            items.Add(new MyModel(new Uri("https://media.giphy.com/media/xT77XR3gI2c7NiDzEY/giphy.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/rose_sparkle.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/canvas_prev.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/dl_world_anim.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_opt/bunny_bgnd_lzw_gifsicle.gif")));
            items.Add(new MyModel(new Uri("https://upload.wikimedia.org/wikipedia/commons/thumb/a/a8/Tour_Eiffel_Wikimedia_Commons.jpg/800px-Tour_Eiffel_Wikimedia_Commons.jpg")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/YHoBqLR.gif")));
            this.cbGifs.ItemsSource = items;
            this.cbGifs.SelectedItem = items[0];
        }
        private void AnimationBehavior_OnError(object sender, string s)
        {
            Debug.WriteLine(s);
        }
        private async void Load_Click(object sender, RoutedEventArgs e)
        {
            //_gifImage.UriSource = new Uri("ms-appx:///kitty.gif");
            // GifImage.AnimationBehavior.SetImageUriSource(_gifImage, new Uri("ms-appx:///Gifs/kitty.gif"));

            //   GifImage.AnimationBehavior.SetGifImageUri(_gifImage, new Uri("http://i.imgur.com/YHoBqLR.gif"));
            //  XamlAnimatedGif.AnimationBehavior.SetSourceUri(_gifImage,new Uri("http://i.imgur.com/YHoBqLR.gif"));


            await OpenGif(((MyModel)this.cbGifs.SelectedItem).Uri);
        }

        private void Unload_Click(object sender, RoutedEventArgs e)
        {
            //_gifImage.UriSource = null;
               GifImage.AnimationBehavior.SetImageUriSource(_gifImage, null);
          //  GifImage.AnimationBehavior.SetImageStreamSource(_gifImage, null);

          //  XamlAnimatedGif.AnimationBehavior.SetSourceUri(_gifImage, null);
        }

        private void Play_Click(object sender, RoutedEventArgs e)
        {
          //  _gifImage.IsAnimating = true;
        }

        private void Pause_Click(object sender, RoutedEventArgs e)
        {
          //  _gifImage.IsAnimating = false;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(GridViewTest),48);
        }

        private void BnListViewTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(ListViewTest));
        }

        private async Task OpenGif(Uri uri)
        {
            //  string uri = "ms-appx:///Gifs/19.gif";
            //  XamlAnimatedGif.AnimationBehavior.SetSourceUri(_gifImage, new Uri(uri));
             GifImage.AnimationBehavior.SetImageUriSource(_gifImage, uri);    
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
    }
}
