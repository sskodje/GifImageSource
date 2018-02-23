#include "pch.h"
#include "ColorMatrixEffectDescription.h"

using namespace Platform;
using namespace GifImage;

GifImage::ColorMatrixEffectDescription::ColorMatrixEffectDescription(const Array<float>^ colorMatrix, ColorMatrixAlphaMode alphaMode, bool clampOutput)
{
	ColorMatrix = ref new Array<float>(colorMatrix);
	AlphaMode = alphaMode;
	ClampOutput = clampOutput;
}

GifImage::ColorMatrixEffectDescription::ColorMatrixEffectDescription()
{
	float colorMatrix[] =
	{
		0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};

	ColorMatrix = ref new Array<float>(colorMatrix, 20);

	AlphaMode = ColorMatrixAlphaMode::Premultiplied;
	ClampOutput = false;

}
