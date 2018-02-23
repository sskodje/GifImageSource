#include "pch.h"
#include "ConvolveMatrixEffectDescription.h"
using namespace GifImage;
using namespace Platform;
GifImage::ConvolveMatrixEffectDescription::ConvolveMatrixEffectDescription(ConvolveMatrixScaleMode scaleMode, float divisor, float bias, Vector2^ kernelOffset, bool preserveAlpha,
	ConvolveMatrixBorderMode borderMode, bool clampOutput)
{
	ScaleMode = scaleMode;
	Divisor = divisor;
	Bias = bias;
	KernelOffset = kernelOffset;
	PreserveAlpha = preserveAlpha;
	BorderMode = borderMode;
	ClampOutput = clampOutput;
}

GifImage::ConvolveMatrixEffectDescription::ConvolveMatrixEffectDescription()
{
	ScaleMode = ConvolveMatrixScaleMode::Linear;
	Divisor = 1.0f;
	Bias = 0;
	KernelOffset = ref new Vector2(0, 0);
	PreserveAlpha = false;
	BorderMode = ConvolveMatrixBorderMode::Soft;
	ClampOutput = false;
}

void GifImage::ConvolveMatrixEffectDescription::SetKernelMatrix(const Array<float>^ kernelMatrix, UINT kernelSizeX, UINT kernelSizeY)
{
	if (kernelSizeX * kernelSizeY != kernelMatrix->Length)
		throw Platform::InvalidArgumentException::CreateException(-1);

	m_kernelSizeX = kernelSizeX;
	m_kernelSizeY = kernelSizeY;
	m_kernelMatrix = ref new Array<float>(kernelMatrix);
}
