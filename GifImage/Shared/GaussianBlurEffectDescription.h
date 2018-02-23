#pragma once

#include "pch.h"
#include "IEffectDescription.h"


namespace GifImage
{
	public enum struct GaussianBlurOptimization {Speed, Balanced, Quality};
	public enum struct GaussianBlurBorderMode {Soft, Hard};

	public ref class GaussianBlurEffectDescription sealed : IEffectDescription
	{
	public:		
		GaussianBlurEffectDescription(float deviation, GaussianBlurOptimization optimization, GaussianBlurBorderMode borderMode);
		GaussianBlurEffectDescription();

		virtual EffectGuid^ GetEffectGuid() {
			GUID g = CLSID_D2D1GaussianBlur;
			return ref new EffectGuid(g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
		}
		virtual int GetParametersCount()
		{
			return 3;
		}

		virtual EffectParameterValue^ GetParameter(int index)
		{
			switch (index)
			{
			case 0:
				return ref new EffectParameterValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, Deviation);
			case 1:
				return ref new EffectParameterValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, (int)Optimization);
			case 2:
				return ref new EffectParameterValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, (int)BorderMode);
			}

			return nullptr;
		}

		property float Deviation;

		property GaussianBlurOptimization Optimization;

		property GaussianBlurBorderMode BorderMode;

	};
}

