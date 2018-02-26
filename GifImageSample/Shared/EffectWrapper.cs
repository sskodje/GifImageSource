using GifImage;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GifImageSample
{
    public class EffectWrapper
    {
        public string Name { get; set; }
        public IEffectDescription EffectDescription { get; set; }
        public bool IsActive { get; set; }

        public EffectWrapper(string name,IEffectDescription effect)
        {
            Name = name;
            EffectDescription = effect;
        }
    }
}
