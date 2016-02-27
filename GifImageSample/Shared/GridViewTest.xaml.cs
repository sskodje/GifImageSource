#if WINDOWS_PHONE_APP
using Windows.Phone.UI.Input;
#endif
using GifImage;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
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
    public sealed partial class GridViewTest : Page
    {
        public ObservableCollection<MyModel> Items { get; set; }
        public GridViewTest()
        {
            this.DataContext = this;
            this.InitializeComponent();
#if WINDOWS_APP
            this.navBackButton.Style = (Style)Resources["NavigationBackButtonNormalStyle"];
            this.navBackButton.Visibility = Windows.UI.Xaml.Visibility.Visible;
#elif WINDOWS_PHONE_APP
            HardwareButtons.BackPressed += HardwareButtons_BackPressed;
#endif

        }


#if WINDOWS_PHONE_APP
        void HardwareButtons_BackPressed(object sender, BackPressedEventArgs e)
        {
            Frame rootFrame = Window.Current.Content as Frame;

            if (rootFrame != null && rootFrame.CanGoBack)
            {
                HardwareButtons.BackPressed -= HardwareButtons_BackPressed;
                e.Handled = true;
                rootFrame.GoBack();
            }
        }
#endif

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            int gifCount;
            if (e.Parameter is int)
                gifCount = (int)e.Parameter;
            else
                gifCount = 50;


            Items = new ObservableCollection<MyModel>(Enumerable.Range(1, gifCount).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList());
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            navBackButton.Click -= Button_Click;
        }
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (Frame.CanGoBack)
                Frame.GoBack();
        }

        private void Image_Unloaded(object sender, RoutedEventArgs e)
        {
            ((Image)sender).Unloaded -= Image_Unloaded;
        }

        private void bnAddItems_Click(object sender, RoutedEventArgs e)
        {
            Items.Add(new MyModel(MyModel.GetSampleUriFromIndex(new Random().Next(1, 22))));
        }

        private void bnRemoveItems_Click(object sender, RoutedEventArgs e)
        {
            if (this.Items.Count > 0)
            this.Items.Remove(this.Items.Last());
        }

        private void Image_Loaded(object sender, RoutedEventArgs e)
        {

        }
    }
}
