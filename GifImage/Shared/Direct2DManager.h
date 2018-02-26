#pragma once


namespace GifImage
{
	class Direct2DManager
	{
	public:
		// Private constructor
		Direct2DManager(UINT windowID);
		virtual ~Direct2DManager();
		// Instance
		static Direct2DManager* GetInstance(int id);
		static void ReturnInstance(int id);
		static void ReserveInstance(int id);
		// Properties

		inline Microsoft::WRL::ComPtr<ID2D1Device> GetD2DDevice()
		{
			return m_d2dDevice;
		}

		inline Microsoft::WRL::ComPtr<ID2D1DeviceContext> GetD2DContext()
		{
			return m_d2dContext;
		}
		inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetD3DContext()
		{
			return m_d3dContext;
		}
		inline Microsoft::WRL::ComPtr<IDXGIDevice> GetDXGIDevice()
		{
			return m_dxgiDevice;
		}
		inline Microsoft::WRL::ComPtr<ID3D11Device> GetD3DDevice()
		{
			return m_d3dDevice;
		}
		inline Microsoft::WRL::ComPtr<IWICImagingFactory> GetIWICFactory()
		{
			return m_pIWICFactory;
		}
		void ClearDeviceResource();
		void Recreate();

	private:
		// Instance
		static std::vector<Direct2DManager*>                m_instances;

		UINT m_clientCount;
		UINT m_windowID;
		// D2D members
		Microsoft::WRL::ComPtr<IDXGIDevice>                    m_dxgiDevice;
		Microsoft::WRL::ComPtr<ID2D1Device>                    m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext>             m_d2dContext;
		//D3D members
		Microsoft::WRL::ComPtr<ID3D11Device>				   m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>             m_d3dContext;
		//WIC members
		Microsoft::WRL::ComPtr<IWICImagingFactory> m_pIWICFactory;

		void CreateDeviceResources();
	};
}
