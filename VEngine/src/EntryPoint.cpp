#include "Application.h"

#ifdef V_PLATFORM_WINDOWS
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	VEngine::Application::start();
	VEngine::Application::run();
}

#endif
