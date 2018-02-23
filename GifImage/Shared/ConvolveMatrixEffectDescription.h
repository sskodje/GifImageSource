#pragma once

#include "pch.h"
#include "IEffectDescription.h"
#include "Vector2.h"

namespace GifImage
{
	public enum struct ConvolveMatrixScaleMode { NearestNeighbor, Linear, Cubic, MultiSampleLinear, Anisotropic, HighQualityCubic };
	public enum struct ConvolveMatrixBorderMode { Soft, Hard };

	public ref class ConvolveMatrixEffectDescription sealed : IEffectDescription
	{
		UINT m_kernelSizeX;
		UINT m_kernelSizeY;
		Platform::Array<float>^ m_kernelMatrix;
	public:
		ConvolveMatrixEffectDescription(ConvolveMatrixScaleMode scaleMode, float divisor, float bias, Vector2^ kernelOffset, bool preserveAlpha, ConvolveMatrixBorderMode borderMode, bool clampOutput);
		ConvolveMatrixEffectDescription();

		virtual EffectGuid^ GetEffectGuid() {
			GUID g = CLSID_D2D1ConvolveMatrix;
			return ref new EffectGuid(g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
		}
		virtual int GetParametersCount()
		{
			return 10;
		}

		virtual EffectParameterValue^ GetParameter(int index)
		{
			switch (index)
			{
			case 0:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_SCALE_MODE, (int)ScaleMode);
			case 1:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_X, (int)m_kernelSizeX);
			case 2:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_Y, (int)m_kernelSizeY);
			case 3:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_KERNEL_MATRIX, m_kernelMatrix);
			case 4:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_DIVISOR, Divisor);
			case 5:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_BIAS, Bias);
			case 6:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_KERNEL_OFFSET, KernelOffset);
			case 7:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_PRESERVE_ALPHA, PreserveAlpha);
			case 8:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_BORDER_MODE, (int)BorderMode);
			case 9:
				return ref new EffectParameterValue(D2D1_CONVOLVEMATRIX_PROP_CLAMP_OUTPUT, ClampOutput);
			}

			return nullptr;
		}

		property ConvolveMatrixScaleMode ScaleMode;

		property float Divisor;

		property float Bias;

		void SetKernelMatrix(const Platform::Array<float>^ kernelMatrix, UINT kernelSizeX, UINT kernelSizeY);

		property Vector2^ KernelOffset;

		property bool PreserveAlpha;

		property ConvolveMatrixBorderMode BorderMode;

		property bool ClampOutput;
	};
}

