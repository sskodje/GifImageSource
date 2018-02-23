#include "pch.h"
#include "BrightnessEffectDescription.h"

GifImage::BrightnessEffectDescription::BrightnessEffectDescription(Vector2^ whitePoint, Vector2^ blackPoint)
{
	WhitePoint = whitePoint;
	BlackPoint = blackPoint;
}

GifImage::BrightnessEffectDescription::BrightnessEffectDescription()
{
	WhitePoint = ref new Vector2(1.0f, 1.0f);
	BlackPoint = ref new Vector2(0, 0);
}
