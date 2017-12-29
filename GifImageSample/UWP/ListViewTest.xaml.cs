#if WINDOWS_PHONE_APP
using Windows.Phone.UI.Input;
#endif
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
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
using Windows.Storage;
using GifImage;
using System.Diagnostics;
using Windows.UI.Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace GifImageSample.UWP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ListViewTest : Page, INotifyPropertyChanged
    {
        private ObservableCollection<MyModel> _items;
        public ObservableCollection<MyModel> Items
        {
            get { return _items; }
            set
            {
                if (_items != value)
                {
                    _items = value;
                    RaisePropertyChanged("Items");
                }
            }
        }

        private bool _isPausedWhileScrolling;
        public bool IsPausedWhileScrolling
        {
            get { return _isPausedWhileScrolling; }
            set
            {
                if (_isPausedWhileScrolling != value)
                {
                    _isPausedWhileScrolling = value;
                    RaisePropertyChanged("IsPausedWhileScrolling");
                }
            }
        }


        public ListViewTest()
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



        private void Sc_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            if (IsPausedWhileScrolling)
            {
                if (e.IsIntermediate)
                {

                    GifImageSource.PauseAllGifs();
                }
                else
                {

                    //scroll is stopping
                    GifImageSource.ResumeAllGifs();
                }
            }
        }


        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            int gifCount = 0;
            if (e.Parameter is int)
                gifCount = (int)e.Parameter;
            else
                gifCount = 150;

            Items = new ObservableCollection<MyModel>(Enumerable.Range(1, gifCount).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList());
        }
        protected void RaisePropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
        public event PropertyChangedEventHandler PropertyChanged;

        private void bnRemoveItems_Click(object sender, RoutedEventArgs e)
        {
            Items.Clear();
        }

        private void bnAddItems_Click(object sender, RoutedEventArgs e)
        {
            foreach (MyModel model in Enumerable.Range(0, 150).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList())
            {
                Items.Add(model);
            }
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            ScrollViewer sc = this.listViewTest.GetFirstDescendantOfType<ScrollViewer>();
            if (sc != null)
            {
                sc.ViewChanged += Sc_ViewChanged;
            }
        }

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            ScrollViewer sc = this.listViewTest.GetFirstDescendantOfType<ScrollViewer>();
            if (sc != null)
            {
                sc.ViewChanged -= Sc_ViewChanged;
            }
        }
    }
}
