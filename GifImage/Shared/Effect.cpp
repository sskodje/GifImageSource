#include "pch.h"
#include "Effect.h"
#include "Direct2DManager.h"
#include "Utilities.h"

using namespace GifImage;
using namespace Platform;
using namespace Platform::Collections;
using namespace Microsoft::WRL;

Effect::Effect(IEffectDescription^ effectDescription) : m_effectDescription(effectDescription)
{
}


Effect::~Effect(void)
{
}

ComPtr<ID2D1Effect> Effect::Apply(ComPtr<ID2D1DeviceContext> context, ComPtr<ID2D1Effect> previousD2DEffect)
{
	// Create the effect
	ComPtr<ID2D1Effect> d2dEffect;
	
	DX::ThrowIfFailed(
		context->CreateEffect(m_effectDescription->GetEffectGuid()->ToGuid(), &d2dEffect)
	);

	if (previousD2DEffect)
		d2dEffect->SetInputEffect(0, previousD2DEffect.Get());

	// Properties
	for (int index = 0; index < m_effectDescription->GetParametersCount(); index++)
	{
		auto parameter = m_effectDescription->GetParameter(index);

		switch (parameter->Type)
		{
		case EffectParameterType::Float:
			DX::ThrowIfFailed(
				d2dEffect->SetValue(parameter->Key, parameter->ValueAsFloat)
			);
			break;
		case EffectParameterType::Int:
			DX::ThrowIfFailed(
				d2dEffect->SetValue(parameter->Key, parameter->ValueAsInt)
			);
			break;
		case EffectParameterType::Bool:
			DX::ThrowIfFailed(
				d2dEffect->SetValue(parameter->Key, (BOOL)parameter->ValueAsBool)
			);
			break;
		case EffectParameterType::FloatArray:
			auto vector = parameter->ValueAsArray;
			UINT byteArraySize = vector->Length * sizeof(float);
			auto byteArray = std::vector<byte>(byteArraySize);

			memcpy(&byteArray[0], &vector[0], byteArraySize);

			DX::ThrowIfFailed(
				d2dEffect->SetValue(parameter->Key, &byteArray[0], byteArraySize)
			);
		}
	}

	return d2dEffect.Get();
}