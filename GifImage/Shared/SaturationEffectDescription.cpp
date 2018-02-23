#include "pch.h"
#include "SaturationEffectDescription.h"


GifImage::SaturationEffectDescription::SaturationEffectDescription(float saturation)
{
	Saturation = saturation;
}

GifImage::SaturationEffectDescription::SaturationEffectDescription()
{
	Saturation = 0.5f;
}