#pragma once
#include "pch.h"
namespace GifImage {
	public ref class EffectGuid sealed
	{
	public:

		EffectGuid(unsigned int a,
			unsigned short b,
			unsigned short c,
			unsigned char d,
			unsigned char e,
			unsigned char f,
			unsigned char g,
			unsigned char h,
			unsigned char i,
			unsigned char j,
			unsigned char k):a(a),b(b),c(c),d(d),e(e),f(f),g(g),h(h),i(i),j(j),k(k)
		{

		}

virtual		~EffectGuid()
		{
		}

		Platform::Guid ToGuid() {

			return Platform::Guid(a, b, c, d, e, f, g, h, i, j, k);
		}

	private:
		unsigned int a;
		unsigned short b;
		unsigned short c;
		unsigned char d;
		unsigned char e;
		unsigned char f;
		unsigned char g;
		unsigned char h;
		unsigned char i;
		unsigned char j;
		unsigned char k;

	};
}