#include "precheader.h"

#include "Application.h"

#ifdef V_PLATFORM_WINDOWS

int main(int argc, char* argv[])
{
	vengine::Application application;
	application.run();
	return 0;
}

#endif
