#pragma once

namespace GifImage
{
	public ref class Vector2 sealed
	{
		float m_x;
		float m_y;

	public:
		Vector2(float x, float y):m_x(x), m_y(y)
		{
		}

		property float X {
			float get() {
				return m_x;
			}
			void set(float value){
				m_x = value;
			}
		}

		property float Y {
			float get() {
				return m_y;
			}
			void set(float value){
				m_y = value;
			}
		}
	};
}

