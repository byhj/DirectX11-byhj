#include "Engine.h"
#include <iostream>

namespace byhj
{
	Engine::Engine()
		:pInput(0),
		 pGraphics(0)
	{

	}

	Engine::Engine(const Engine &rhs)
	{

	}

	Engine::~Engine()
	{

	}


	///////////////Init the Engine Class(Input + Graphics)/////////////
	bool Engine::Init()
	{
		int screenWidth = 0, screenHeight = 0;
		InitWindow(screenWidth, screenHeight);
		bool result;

		///////////////Create Input Class Object/////////////////
		pInput = new Input;
		if (!pInput)
		{
			std::cerr << "Can not create Input Class" << std::endl;
			return false;
		}
		pInput->Init();

		///////////////Create Graphics Class Object////////////////
		pGraphics = new Graphics;
		if (!pGraphics)
		{
			std::cerr << "Can not create Graphics Class" << std::endl;
			return false;
		}
		result = pGraphics->Init(screenWidth, screenHeight, hWnd);
		if(!result)
		{
			return false;
		}

        return true;
	}

	/////////Real Time Rending, Every loop we check the messages form User//////////
	void Engine::Run()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));
		bool result;
		bool done  = false;

		//Run Until Enter ESC or Render Fail
		while (!done)
		{
			if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
		        result = Render();
				if (!result)
				{
					std::cerr << "Render Graphics Fail" << std::endl;
					done = true;
				}
			}
		}
	}
	//////////////Render The Graphics//////////////////////
	bool Engine::Render()
	{
		bool result;
		if (pInput->IsKeyDown(VK_ESCAPE) )
		{
			return false;
		}

		result = pGraphics->Render();
		if (!result)
		{
			return false;
		}

		return true;
	}

	////////////////////Init Window //////////////////////////////
	void Engine::InitWindow(int &screenWidth, int &screenHeight)
	{
		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;
		int posX, posY;

		// Get an external pointer to this object.
		pAppHandle = this;
		hInstance = GetModuleHandle(NULL);

		AppName = L"DirectX11-Engine";

		// Setup the windows class with default settings.
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm       = wc.hIcon;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = AppName;
		wc.cbSize        = sizeof(WNDCLASSEX);

		// Register the window class.
		RegisterClassEx(&wc);

		// Determine the resolution of the clients desktop screen.
		screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if(FULL_SCREEN)
		{
			// If full screen set the screen to maximum size of the users desktop and 32bit.
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;			
			dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Change the display settings to full screen.
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			// Set the position of the window to the top left corner.
			posX = posY = 0;
		}	
		else
		{
			// If windowed then set it to 1000x800 resolution.
			screenWidth  = 1000;
			screenHeight = 800;

			// Place the window in the middle of the screen.
			posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
		}

		hWnd = CreateWindowEx(	
			NULL,	           
			AppName, 
			AppName,
			WS_OVERLAPPEDWINDOW,	
			posX, 
			posY, 
			screenWidth, 
			screenHeight,
			NULL,
			NULL,
			hInstance,	
			NULL
			);

		// Bring the window up on the screen and set it as main focus.
		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);

		return;
	}
	/////////////////////Shutdonw Engine////////////////////////
	void Engine::Shutdown()
	{
		//Release Graphics Object
		if (pGraphics)
		{
			pGraphics->Shutdown();
			delete pGraphics;
			pGraphics = 0;
		}

		//Release Input Object
		if (pInput)
		{
			delete pInput;
			pInput = 0;
		}

		ShutdownWindow();
	}

	///////////////////Shutdown Window///////////////////////////////
	void Engine::ShutdownWindow()
	{
		// Fix the display settings if leaving full screen mode.
		if (FULL_SCREEN)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		DestroyWindow(hWnd);
		hWnd = NULL;

		UnregisterClass(AppName, hInstance);
		hInstance = NULL;
		pAppHandle = NULL;

		return;
	}

	////////////////////////////CALLBACK Message////////////////////////////
	LRESULT CALLBACK Engine::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
			{
				pInput->KeyDown( (unsigned int)wParam );
				return 0;
			}
		case WM_KEYUP:
			{
				pInput->KeyUp( (unsigned int)wParam );
				return 0;
			}

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	///////////////////////////Window Message Callback Function/////////////
	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
		case WM_CLOSE:
			{
				PostQuitMessage(0);
				return 0;
			}
		default:
			return pAppHandle->MessageHandler(hWnd, uMsg, wParam, lParam);
		}
	}


}//Namespace

