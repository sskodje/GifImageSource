#pragma once

namespace GifImage
{
	public ref class Vector4F sealed
	{
		float m_x;
		float m_y;
		float m_z;
		float m_w;
	public:
		Vector4F(float x, float y,float z, float w) :m_x(x), m_y(y),m_z(z),m_w(w)
		{
		}

		property float X {
			float get() {
				return m_x;
			}
			void set(float value) {
				m_x = value;
			}
		}

		property float Y {
			float get() {
				return m_y;
			}
			void set(float value) {
				m_y = value;
			}
		}

		property float Z {
			float get() {
				return m_z;
			}
			void set(float value) {
				m_z = value;
			}
		}

		property float W {
			float get() {
				return m_w;
			}
			void set(float value) {
				m_w = value;
			}
		}
	};
}

