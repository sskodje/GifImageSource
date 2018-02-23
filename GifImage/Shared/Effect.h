#pragma once
#include "IEffectDescription.h"

using namespace Microsoft::WRL;
using namespace GifImage;
class Effect
{
	IEffectDescription^ m_effectDescription;

public:
	Effect(IEffectDescription^ effectDescription);
	~Effect(void);

	ComPtr<ID2D1Effect> Apply(ComPtr<ID2D1DeviceContext> context, ComPtr<ID2D1Effect> previousD2DEffect);

	IEffectDescription^ GetEffectDescription()
	{
		return m_effectDescription;
	}
};