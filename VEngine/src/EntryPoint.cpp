#include "precheader.h"

#include "Application.h"

#ifdef V_PLATFORM_WINDOWS

int main(int argc, char* argv[])
{
	VEngine::Application application;
	application.start();
	application.run();
	return 0;
}

#endif
