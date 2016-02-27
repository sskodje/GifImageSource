﻿using GifImage;
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
            AnimationBehavior.OnError += AnimationBehavior_OnError;
            List<MyModel> items = new List<MyModel>();

            items.AddRange(Enumerable.Range(1, 23).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList());
            items.Add(new MyModel(new Uri("http://i.imgur.com/9Bo0CZi.gif")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/qeu10ds.gif")));
            items.Add(new MyModel(new Uri("https://media.giphy.com/media/xT77XR3gI2c7NiDzEY/giphy.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/canvas_bgnd.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/dl_world_anim.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_opt/bunny_bgnd_lzw_gifsicle.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_opt/speed_map.gif")));
            items.Add(new MyModel(new Uri("https://upload.wikimedia.org/wikipedia/commons/thumb/a/a8/Tour_Eiffel_Wikimedia_Commons.jpg/800px-Tour_Eiffel_Wikimedia_Commons.jpg")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/YHoBqLR.gif")));

            this.cbGifs.ItemsSource = items;
            this.cbGifs.SelectedItem = items[0];
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
        }

        private void AnimationBehavior_OnError(object sender, string s)
        {
            Debug.WriteLine(s);
        }

        private void Load_Click(object sender, RoutedEventArgs e)
        {
            OpenGif(((MyModel)this.cbGifs.SelectedItem).Uri);
            AnimationBehavior.SetRepeatBehavior(_gifImage, new Windows.UI.Xaml.Media.Animation.RepeatBehavior(5));
        }

        private void Unload_Click(object sender, RoutedEventArgs e)
        {
            AnimationBehavior.SetImageUriSource(_gifImage, null);
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

        private async void OpenGif(Uri uri)
        {
                AnimationBehavior.SetImageUriSource(_gifImage, uri);
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
