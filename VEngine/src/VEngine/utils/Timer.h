#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace vengine
{
	struct Time
	{
		long int duration;
		std::string name;
	};
	
	class Timer
	{
	public:
		Timer(const std::string& name)
		{
			m_time.name = name;
			m_start_timepoint = std::chrono::high_resolution_clock::now();
		}
		
		~Timer()
		{
			const auto end_timepoint = std::chrono::high_resolution_clock::now();
			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_start_timepoint).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch().count();
			m_time.duration = (end - start) * 0.001f;
			
			s_times.push_back(m_time);
		}

		static void clear()
		{
			s_times.clear();
		}

		[[nodiscard]] static auto& get_times() { return s_times; };
	private:
		Time m_time;
		std::chrono::steady_clock::time_point m_start_timepoint;
		inline static std::vector<Time> s_times = std::vector<Time>();
	};
}
