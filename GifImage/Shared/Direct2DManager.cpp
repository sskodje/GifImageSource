#include "pch.h"
#include "Direct2DManager.h"
using namespace D2D1;
using namespace GifImage;
using namespace Microsoft::WRL;
using namespace Windows::Foundation::Collections;
using namespace Platform::Collections;

std::vector<Direct2DManager*> Direct2DManager::m_instances;
Direct2DManager::Direct2DManager(UINT windowID)
{
	CreateDeviceResources();
	m_windowID = windowID;
}
Direct2DManager::~Direct2DManager()
{
	ClearDeviceResource();
	OutputDebugString(L"Destructed Direct2DManager instance\r\n");
}
void Direct2DManager::ClearDeviceResource()
{
	m_d2dContext = nullptr;
	m_d2dDevice = nullptr;
	m_dxgiDevice = nullptr;
	m_d3dDevice = nullptr;
	m_d3dContext = nullptr;
}

void Direct2DManager::CreateDeviceResources()
{
	// This flag adds support for surfaces with a different color channel ordering 
	// than the API default. It is required for compatibility with Direct2D. 
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
#if WINDOWS_DLL
	 //If the project is in a debug build, enable debugging via SDK Layers. 
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif 

	// This array defines the set of DirectX hardware feature levels this app will support. 
	// Note the ordering should be preserved. 
	// Don't forget to declare your application's minimum required feature level in its 
	// description.  All applications are assumed to support 9.1 unless otherwise stated. 
	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	//ComPtr<ID3D11Device> device;

	// Create the Direct3D 11 API device object. 
	DX::ThrowIfFailed(
		D3D11CreateDevice(
			nullptr,                        // Specify nullptr to use the default adapter. 
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags,                  // Set debug and Direct2D compatibility flags. 
			featureLevels,                  // List of feature levels this app can support. 
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,              // Always set this to D3D11_SDK_VERSION for Windows Store apps. 
			&m_d3dDevice,                   // Returns the Direct3D device created. 
			nullptr,
			&m_d3dContext
			)
		);

	DX::ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&m_pIWICFactory));

	// Get the Direct3D 11.1 API device. 
	DX::ThrowIfFailed(
		m_d3dDevice.As(&m_dxgiDevice));


	// Create the Direct2D device object and a corresponding context. 
	DX::ThrowIfFailed(
		D2D1CreateDevice(
			m_dxgiDevice.Get(),
			nullptr,
			&m_d2dDevice));

	DX::ThrowIfFailed(
		m_d2dDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&m_d2dContext));
}

void Direct2DManager::Recreate()
{
	ClearDeviceResource();
	CreateDeviceResources();
}

Direct2DManager* Direct2DManager::GetInstance(int id)
{
	Direct2DManager* instance = nullptr;
	for (int i = 0; i < m_instances.size(); i++)
	{
		if (m_instances.at(i)->m_windowID == id)
		{
			instance = m_instances.at(i);
			return instance;
		}
	}
	if (instance == nullptr)
	{
		auto instance = new Direct2DManager(id);
		m_instances.push_back(instance);
		return instance;
	}
	return nullptr;
}

void Direct2DManager::ReserveInstance(int id)
{
	for (int i = 0; i < m_instances.size(); i++)
	{
		auto instance = m_instances.at(i);
		if (instance != nullptr && instance->m_windowID == id)
		{
			instance->m_clientCount++;
		}
	}
}

void Direct2DManager::ReturnInstance(int id)
{
	for (int i = 0; i < m_instances.size(); i++)
	{
		auto instance = m_instances.at(i);
		if (instance != nullptr && instance->m_windowID == id)
		{
			if (instance->m_clientCount > 0)
				instance->m_clientCount--;

			if (instance->m_clientCount == 0)
			{

				instance->ClearDeviceResource();
				m_instances[i] = nullptr;
				m_instances.erase(m_instances.begin() + i);
				delete instance;
			}
		}
	}
}