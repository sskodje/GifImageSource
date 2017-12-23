using GifImage;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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

namespace GifImageSample.UWP
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

    }
}
