/* The frame work will begin with four items. 
 * It will have a WinMain function to handle the entry point of the application. 
 * It will also have a system class that encapsulates the entire application that will be called from within the WinMain function. 
 * Inside the system class we will have a input class for handling user input and a graphics class for handling the DirectX graphics code. 
 * Here is a diagram of the framework setup:
 *
 *                 WinMain
 *                    |
 *               EngineClass
 *                    |
 *         InputClass   GraphicsClass
 */   


#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{

	bool result;
	///////////////////////////System object//////////////////////////////////////
	byhj::Engine* pEngine;
	pEngine = new byhj::Engine;
	if(!pEngine)
	{
		return 0;
	}

	///////////////////////////Init and Run System///////////////////////////
	result = pEngine->Init();
	if(result)
	{
		pEngine->Run();
	}

	/////////////////////Shutdown and release the system object/////////////////
	pEngine->Shutdown();
	delete pEngine;
	pEngine = 0;

	return 0;
}