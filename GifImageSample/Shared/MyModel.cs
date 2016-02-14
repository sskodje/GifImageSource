using System;
using System.Collections.Generic;
using System.Text;

namespace GifImageSample
{
    public class MyModel
    {
        //   public GifImage Image {get;set;}

        public Uri Uri { get; set; }

        public MyModel( Uri uri)
        {
            this.Uri = uri;
        }

        public static Uri GetSampleUriFromIndex(int index)
        {
            Uri uri = null;

            if (index > 0 && index <= 21)
            {
                uri = new Uri(String.Format("ms-appx:///Gifs/{0}.gif", index));
            }
            else if (index == 24)
            {
                // Uri = new Uri("https://i.imgur.com/ZR3QN22.gif");
                // Uri = new Uri("http://i.imgur.com/zxV5dpc.jpg");
                // Uri = new Uri("http://i.imgur.com/8nvvEfd.gif");
                //  Uri = new Uri("http://i.imgur.com/YHoBqLR.gif");
                uri = new Uri(String.Format("ms-appx:///Gifs/{0}.gif", "3"));
            }
            else
            {
                Random r = new Random(index);
                int i = r.Next(1, 21);
                uri = new Uri(String.Format("ms-appx:///Gifs/{0}.gif", i));
            }
            return uri;
        }
    }
}
