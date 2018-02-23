#include "pch.h"
#include "GaussianBlurEffectDescription.h"

GifImage::GaussianBlurEffectDescription::GaussianBlurEffectDescription(float deviation,
																	 GaussianBlurOptimization optimization, 
																	 GaussianBlurBorderMode borderMode)
{
	Deviation = deviation;
	Optimization = optimization;
	BorderMode = borderMode;
}

GifImage::GaussianBlurEffectDescription::GaussianBlurEffectDescription()
{
	Deviation = 1.0f;
	Optimization = GaussianBlurOptimization::Balanced;
	BorderMode = GaussianBlurBorderMode::Soft;
}
