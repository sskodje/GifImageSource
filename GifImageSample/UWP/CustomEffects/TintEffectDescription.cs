using GifImage;
using System;
using System.Collections.Generic;
using System.Text;

namespace GifImageSample
{
    /// <summary>
    /// This effect requires Windows 10.
    /// https://msdn.microsoft.com/en-us/library/windows/desktop/mt745077(v=vs.85).aspx
    /// </summary>
    public class TintEffectDescription : IEffectDescription
    {
        private Vector4F _color;
        public TintEffectDescription()
        {
            _color = new Vector4F(1.0f, 1.0f, 1.0f, 1.0f);
        }
        /// <summary>
        /// Colors from the source image are multiplied by this value.
        /// </summary>
        /// <param name="color"></param>
        public TintEffectDescription(Vector4F color)
        {
            _color = color;
        }
        public EffectGuid GetEffectGuid()
        {
            return new EffectGuid(0x36312b17, 0xf7dd, 0x4014, 0x91, 0x5d, 0xff, 0xca, 0x76, 0x8c, 0xf2, 0x11);
        }

        public EffectParameterValue GetParameter(int index)
        {
            switch (index)
            {
                case 0:
                    return new EffectParameterValue(0, _color);
                default:
                    return null;
            }
        }

        public int GetParametersCount()
        {
            return 1;
        }
    }
}
