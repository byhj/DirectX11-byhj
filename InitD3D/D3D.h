#ifndef D3D_H
#define D3D_H

#pragma  comment(lib, "dxgi.lib")
#pragma  comment(lib, "d3d11.lib")
#pragma  comment(lib, "d3dx11.lib")
#pragma  comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>

namespace byhj
{ 
	class D3D
	{
	public:
		D3D();
		D3D(const D3D &);
		~D3D();

		bool Init(int, int, bool, HWND, bool, float, float);
		void Shutdown();
		void BeginScene(float, float, float, float);
		void EndScene();

		ID3D11Device * GetDevice();
		ID3D11DeviceContext *GetDeviceContext();

		void GetProj(D3DXMATRIX &);
		void GetWorld(D3DXMATRIX &);
		void GetView(D3DXMATRIX &);

		IDXGISwapChain           * pSwapChain;
		ID3D11Device             * pD3D11Device;
		ID3D11DeviceContext      * pD3D11DeviceContext;
		ID3D11RenderTargetView   * pRenderTargetView;
		ID3D11Texture2D          * pDepthStencilBuffer;
		ID3D11DepthStencilState  * pDepthStencilState;
		ID3D11DepthStencilView   * pDepthStencilView;
		ID3D11RasterizerState    * pRasterState;

		D3DXMATRIX Proj;
		D3DXMATRIX World;
		D3DXMATRIX View;
	};
}
#endif // !D3D_H
