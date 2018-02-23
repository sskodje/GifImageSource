#pragma once
#include "pch.h"
#include "IEffectDescription.h"
namespace GifImage
{
	public ref class HueEffectDescription sealed: IEffectDescription
	{
	public:
		HueEffectDescription(float angle);
		HueEffectDescription();

		virtual EffectGuid^ GetEffectGuid() {
			GUID g = CLSID_D2D1HueRotation;
			return ref new EffectGuid(g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
		}
		virtual int GetParametersCount()
		{
			return 1;
		}

		virtual EffectParameterValue^ GetParameter(int index)
		{
			switch (index)
			{
			case 0:
				return ref new EffectParameterValue(D2D1_HUEROTATION_PROP_ANGLE, Angle);
			}

			return nullptr;
		}

		property float Angle;
	};
}

