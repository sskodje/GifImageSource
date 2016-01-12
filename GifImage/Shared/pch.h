#pragma once

#include <collection.h>
#include <ppltasks.h>

#include <wrl.h> 
#include <wrl\client.h> 

#include <dxgi.h> 
#include <dxgi1_2.h> 
#include <d2d1_1.h> 
#include <d3d11_2.h> 

#include "windows.ui.xaml.media.dxinterop.h"

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw Platform::Exception::CreateException(hr);
		}
	}
}