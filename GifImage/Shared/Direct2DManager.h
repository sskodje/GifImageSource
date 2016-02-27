#pragma once


namespace GifImage
{
	class Direct2DManager
	{
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

		void CreateDeviceResources();


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

		inline Microsoft::WRL::ComPtr<IDXGIDevice> GetDXGIDevice()
		{
			return m_dxgiDevice;
		}
		inline Microsoft::WRL::ComPtr<ID3D11Device> GetD3DDevice()
		{
			return m_d3dDevice;
		}
		void ClearDeviceResource();
		void Recreate();
	};
}
