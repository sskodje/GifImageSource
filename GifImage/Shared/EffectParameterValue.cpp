#include "pch.h"
#include "EffectParameterValue.h"

using namespace Platform;
using namespace GifImage;

GifImage::EffectParameterValue::EffectParameterValue(int key, int value) : m_key(key), m_type(EffectParameterType::Int)
{
	m_value.asInt = value;
}

GifImage::EffectParameterValue::EffectParameterValue(int key, float value) : m_key(key), m_type(EffectParameterType::Float)
{
	m_value.asFloat[0] = value;
}

GifImage::EffectParameterValue::EffectParameterValue(int key, const Array<float>^ value) : m_key(key), m_type(EffectParameterType::FloatArray)
{
	m_array = ref new Array<float>(value);
}

GifImage::EffectParameterValue::EffectParameterValue(int key, Vector2^ vector) : m_key(key), m_type(EffectParameterType::FloatArray)
{
	m_array = ref new Array<float>(2);
	m_array[0] = vector->X;
	m_array[1] = vector->Y;
}

GifImage::EffectParameterValue::EffectParameterValue(int key, Vector4F^ vector) : m_key(key), m_type(EffectParameterType::FloatArray)
{
	m_array = ref new Array<float>(4);
	m_array[0] = vector->X;
	m_array[1] = vector->Y;
	m_array[2] = vector->Z;
	m_array[3] = vector->W;
}

GifImage::EffectParameterValue::EffectParameterValue(int key, bool value) : m_key(key), m_type(EffectParameterType::Bool)
{
	m_value.asBool = value;
}

