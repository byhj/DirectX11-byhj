#include "Graphics.h"

namespace byhj
{

	Graphics::Graphics()
		:pD3D(0)
	{
	}


	Graphics::Graphics(const Graphics& rhs)
	{
	}


	Graphics::~Graphics()
	{
	}


	bool Graphics::Init(int screenWidth, int screenHeight, HWND hwnd)
	{
		bool result;

		// Create the Direct3D object.
		pD3D = new byhj::D3D;
		if(!pD3D)
		{
			return false;
		}

		// Initialize the Direct3D object.
		result = pD3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
		if(!result)
		{
			MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
			return false;
		}

		return true;
	}


	void Graphics::Shutdown()
	{
		if(pD3D)
		{
			pD3D->Shutdown();
			delete pD3D;
			pD3D = 0;
		}

		return;
	}

	bool Graphics::Render()
	{	// Render the graphics scene.
		bool result;
		result = RenderGraphics();
		if(!result)
		{
			return false;
		}

		return true;
	}

	bool Graphics::RenderGraphics()
	{
		pD3D->BeginScene(0.5f, 0.0f, 0.5f, 1.0f);
		pD3D->EndScene();
		return true;
	}

} //Namespace