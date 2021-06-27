#include "precheader.h"

#include "Application.h"

#ifdef V_PLATFORM_WINDOWS

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	vengine::Application application;
	application.init();
	application.run();
	application.shutdown();

	return 0;
}

#endif
