﻿using GifImage;
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
        List<EffectWrapper> _effects = new List<EffectWrapper>();
        public MainPage()
        {
            this.InitializeComponent();

            //This is an example on setting a global default HttpClient with an auth token, if you use the library with a service that requires it.
            var client = new HttpClient();
            client.DefaultRequestHeaders.Authorization = new Windows.Web.Http.Headers.HttpCredentialsHeaderValue("Bearer", "eyJ0eXAiOiJKV1Q(..rest of your token here)");
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

            _effects.Add(new EffectWrapper("Brightness", new BrightnessEffectDescription(new Vector2(1.0f, 0.8f), new Vector2(0, 0.1f))));
            _effects.Add(new EffectWrapper("ColorMatrix", new ColorMatrixEffectDescription(
                new[] {
                            0.2125f, 0.2125f, 0.2125f, 0.0f,
                            0.7154f, 0.7154f, 0.7154f, 0.0f,
                            0.0721f, 0.0721f, 0.0721f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f,
                            0.0f, 0.0f, 0.0f, 0.0f
                },
                ColorMatrixAlphaMode.Straight, true)));

            var convolve = new ConvolveMatrixEffectDescription();
            convolve.SetKernelMatrix(new[] { -1.0f, -1.0f, -1.0f, -1.0f, 9.0f, -1.0f, -1.0f, -1.0f, -1.0f }, 3, 3);
            _effects.Add(new EffectWrapper("Convolve", convolve));
            _effects.Add(new EffectWrapper("Hue", new HueEffectDescription(180.0f)));
            _effects.Add(new EffectWrapper("Saturation", new SaturationEffectDescription()));
            _effects.Add(new EffectWrapper("Gaussian Blur", new GaussianBlurEffectDescription(5.0f, GaussianBlurOptimization.Speed, GaussianBlurBorderMode.Soft)));
            _effects.Add(new EffectWrapper("Tint", new TintEffectDescription(new Vector4F(1.0f, 1.0f, 5.0f, 1.0f))));
            this.cbEffects.ItemsSource = _effects;
            this.cbEffects.PlaceholderText = "No effects selected";
            //this.cbEffects.SelectedItem = _effects[0];
        }

        private void AppBarButtonOpenSingleEmoticonTest_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(SingleGifTest));
        }

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {

        }
        private void CheckBox_CheckedChanged(object sender, RoutedEventArgs e)
        {
            List<IEffectDescription> effects = _effects.Where(x => x.IsActive).Select(x => x.EffectDescription).ToList();
            AnimationBehavior.SetRenderEffects(_gifImage, effects);
            this.cbEffects.PlaceholderText = String.Join(",",_effects.Where(x => x.IsActive).Select(x => x.Name));
            if(String.IsNullOrEmpty(this.cbEffects.PlaceholderText))
            {
                this.cbEffects.PlaceholderText = "No effects selected";
            }
        }
    }
}
