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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace GifImageSample
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

        public ListViewTest()
        {
            this.DataContext = this;

            this.InitializeComponent();
#if WINDOWS_APP
            this.navBackButton.Style = (Style)Resources["NavigationBackButtonNormalStyle"];
            this.navBackButton.Visibility = Windows.UI.Xaml.Visibility.Visible;
#elif WINDOWS_PHONE_APP
            HardwareButtons.BackPressed += HardwareButtons_BackPressed;
#endif
           // this.NavigationCacheMode = NavigationCacheMode.Disabled;
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
            Items = new ObservableCollection<MyModel>(Enumerable.Range(0, 150).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList());
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (Frame.CanGoBack)
                Frame.GoBack();
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {

          //  GifImage.AnimationBehavior.OnError += AnimationBehavior_OnError;
        }

        private void AnimationBehavior_OnError(object sender, string s)
        {
         //   Debug.WriteLine(s);
        }

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            this.Unloaded -= Page_Unloaded;
            this.Items = null;
            this.DataContext = null;
            //  GifImage.AnimationBehavior.OnError -= AnimationBehavior_OnError;
        }
    }
}
