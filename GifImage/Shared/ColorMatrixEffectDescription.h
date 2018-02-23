#pragma once
#include "pch.h"
#include "IEffectDescription.h"
namespace GifImage
{
	public enum struct ColorMatrixAlphaMode { Premultiplied = 1, Straight = 2 };

	public ref class ColorMatrixEffectDescription sealed : GifImage::IEffectDescription
	{
	public:
		ColorMatrixEffectDescription(const Platform::Array<float>^ colorMatrix, ColorMatrixAlphaMode alphaMode, bool clampOutput);
		ColorMatrixEffectDescription();

		virtual int GetParametersCount()
		{
			return 3;
		}

		virtual EffectGuid^ GetEffectGuid() {
			GUID g = CLSID_D2D1ColorMatrix;
			return ref new EffectGuid(g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
		}
		virtual GifImage::EffectParameterValue^ GetParameter(int index)
		{
			switch (index)
			{
			case 0:
				return ref new GifImage::EffectParameterValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, ColorMatrix);
			case 1:
				return ref new GifImage::EffectParameterValue(D2D1_COLORMATRIX_PROP_ALPHA_MODE, (int)AlphaMode);
			case 2:
				return ref new GifImage::EffectParameterValue(D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT, ClampOutput);
			}

			return nullptr;
		}

		property Platform::Array<float>^ ColorMatrix;
		property ColorMatrixAlphaMode AlphaMode;
		property bool ClampOutput;
	};
}

