#pragma once
#include "EffectParameterValue.h"
#include "EffectGuid.h"

namespace GifImage
{
	public interface class IEffectDescription
	{
		int GetParametersCount();
		EffectGuid^ GetEffectGuid();
		EffectParameterValue^ GetParameter(int index);
	};
}