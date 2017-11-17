using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GifImageSample.UWP
{
    public class MyModel
    {
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

        public static List<MyModel> CreateTestModels()
        {
            List<MyModel> items = new List<MyModel>();

            items.AddRange(Enumerable.Range(1, 23).Select(x => new MyModel(MyModel.GetSampleUriFromIndex(x))).ToList());
            items.Add(new MyModel(new Uri("http://i.imgur.com/9Bo0CZi.gif")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/qeu10ds.gif")));
            items.Add(new MyModel(new Uri("https://media.giphy.com/media/xT77XR3gI2c7NiDzEY/giphy.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/canvas_bgnd.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_basics/dl_world_anim.gif")));
            items.Add(new MyModel(new Uri("http://www.imagemagick.org/Usage/anim_opt/bunny_bgnd_lzw_gifsicle.gif")));
            items.Add(new MyModel(new Uri("https://upload.wikimedia.org/wikipedia/commons/thumb/a/a8/Tour_Eiffel_Wikimedia_Commons.jpg/800px-Tour_Eiffel_Wikimedia_Commons.jpg")));
            items.Add(new MyModel(new Uri("http://i.imgur.com/YHoBqLR.gif")));
            items.Add(new MyModel(new Uri("https://uploads.disquscdn.com/images/c6aed8d2d4ba77c60eee3f761004fc879c8ad0727419a7dd566081a8dad2fb34.gif?w=200")));
            items.Add(new MyModel(new Uri("http://this.is.not.a.valid.uri.com")));
            items.Add(new MyModel(new Uri("ms-appx:///this/path/does/not/exist.gif")));
            return items;
        }
    }
}
