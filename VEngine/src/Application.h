#pragma once

namespace VEngine
{
	class Application
	{
	public:
		static bool start();
		static void run();

	private:
		static bool s_is_running;
	};
}



