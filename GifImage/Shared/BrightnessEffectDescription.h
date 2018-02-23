#pragma once
#include "pch.h"
#include "IEffectDescription.h"
#include "EffectGuid.h"
namespace GifImage
{
	public ref class BrightnessEffectDescription sealed : IEffectDescription
	{
	public:

		BrightnessEffectDescription(Vector2^ whitePoint, Vector2^ blackPoint);
		BrightnessEffectDescription();

		virtual int GetParametersCount()
		{
			return 2;
		}

		virtual EffectGuid^ GetEffectGuid() {
			GUID g = CLSID_D2D1Brightness;
			return ref new EffectGuid(g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
		}
		virtual EffectParameterValue^ GetParameter(int index)
		{
			switch (index)
			{
			case 0:
				return ref new EffectParameterValue(D2D1_BRIGHTNESS_PROP_WHITE_POINT, WhitePoint);
			case 1:
				return ref new EffectParameterValue(D2D1_BRIGHTNESS_PROP_BLACK_POINT, BlackPoint);
			}

			return nullptr;
		}

		property Vector2^ WhitePoint;
		property Vector2^ BlackPoint;
	};
}

