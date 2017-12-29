using GifImage;
using GifImageSample.UWP;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace GifImageSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SingleGifTest : Page
    {
        public SingleGifTest()
        {
            this.InitializeComponent();
            SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = AppViewBackButtonVisibility.Visible;
            SystemNavigationManager.GetForCurrentView().BackRequested += (s, e) =>
            {
                if (Frame.CanGoBack)
                    Frame.GoBack();
                e.Handled = true;
            };
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            OpenGif(MyModel.GetSampleUriFromIndex(13));
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            AnimationBehavior.SetImageUriSource(_gifImage, null);
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {

        }
        private void OpenGif(Uri uri)
        {
            AnimationBehavior.SetImageUriSource(_gifImage, uri);

        }
        private void AppBarButtonLoad_Click(object sender, RoutedEventArgs e)
        {
            OpenGif(MyModel.GetSampleUriFromIndex(13));
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
    }
}
