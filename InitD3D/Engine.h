#ifndef ENGINE_H
#define ENGINE_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "Input.h"
#include "Graphics.h"

namespace byhj
{
	class Engine
	{
	public:
		Engine();
		Engine(const Engine&);
		~Engine();

		//Member Functions
		bool Init();
		void Shutdown();
		void Run();

		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		bool Render();
		void InitWindow(int screenWidth, int screenHeight);
		void ShutdownWindow();

	private:
		LPCTSTR AppName;
		HINSTANCE hInstance;
		HWND hWnd;

		//Input class and Render Graphics class interface
		Input *pInput;
		Graphics *pGraphics;
	};

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static Engine *pAppHandle = 0;
}

#endif