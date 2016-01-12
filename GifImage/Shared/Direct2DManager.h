#pragma once

using namespace Microsoft::WRL;
using namespace Windows::Foundation::Collections;
using namespace Platform::Collections;

namespace GifImage
{
	class Direct2DManager
	{
	private:
		// Instance
		static Direct2DManager*                m_instance;

		static UINT s_clientCount;

		// D2D members
		ComPtr<IDXGIDevice>                    m_dxgiDevice;
		ComPtr<ID2D1Device>                    m_d2dDevice;
		ComPtr<ID2D1DeviceContext>             m_d2dContext;
		//D3D members
		ComPtr<ID3D11Device>				   m_d3dDevice;

		void CreateDeviceResources();


	public:
		// Private constructor
		Direct2DManager(void);
		virtual ~Direct2DManager();
		// Instance
		static Direct2DManager* GetInstance();
		static void ReturnInstance();
		static void ReserveInstance();
		// Properties

		inline ComPtr<ID2D1Device> GetD2DDevice()
		{
			return m_d2dDevice;
		}

		inline ComPtr<ID2D1DeviceContext> GetD2DContext()
		{
			return m_d2dContext;
		}

		inline ComPtr<IDXGIDevice> GetDXGIDevice()
		{
			return m_dxgiDevice;
		}
		inline ComPtr<ID3D11Device> GetD3DDevice()
		{
			return m_d3dDevice;
		}
		void ClearDeviceResource();
		void Recreate();
	};
}
