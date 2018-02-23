#pragma once
#include "pch.h"
#include "Vector2.h"
#include "Vector4F.h"

namespace GifImage
{
	public enum class EffectParameterType { Float, Int, FloatArray, Vector2, Bool };

	union ValueType
	{
		int		asInt;
		float	asFloat[2];
		bool	asBool;
	};

	public ref class EffectParameterValue sealed
	{
		int					m_key;
		EffectParameterType m_type;
		ValueType			m_value;
		Platform::Array<float>^		m_array;

	public:
		[Windows::Foundation::Metadata::DefaultOverloadAttribute]
		EffectParameterValue(int key, int value);
		EffectParameterValue(int key, float value);
		EffectParameterValue(int key, const Platform::Array<float>^ array);
		EffectParameterValue(int key, GifImage::Vector2^ vector);
		EffectParameterValue(int key, GifImage::Vector4F^ vector);
		EffectParameterValue(int key, bool value);

		property GifImage::EffectParameterType Type {
			GifImage::EffectParameterType get() {
				return m_type;
			}
		}

		property int Key {
			int get() {
				return m_key;
			}
		}

		property int ValueAsInt {
			int get() {
				return m_value.asInt;
			}
		}

		property float ValueAsFloat {
			float get() {
				return m_value.asFloat[0];
			}
		}

		property Platform::Array<float>^ ValueAsArray {
			Platform::Array<float>^ get() {
				return m_array;
			}
		}

		property bool ValueAsBool {
			bool get() {
				return m_value.asBool;
			}
		}
	};
}

