using System;
using System.Collections.Generic;
using System.Text;

namespace GifImageSample
{
    public class MyModel
    {
        //   public GifImage Image {get;set;}

        public Uri Uri { get; set; }

        public MyModel(Uri uri)
        {
            this.Uri = uri;
        }

        public static Uri GetSampleUriFromIndex(int index)
        {
            Uri uri = null;

            if (index > 0 && index <= 22)
            {
                uri = new Uri(String.Format("ms-appx:///Gifs/{0}.gif", index));
            }
            else
            {
                Random r = new Random((int)DateTime.Now.Ticks + index);
                int i = r.Next(1, 22);
                uri = new Uri(String.Format("ms-appx:///Gifs/{0}.gif", i));
            }
            return uri;
        }
    }
}
