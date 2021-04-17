#pragma once

#include <imgui.h>

namespace vengine
{
	class Logger
	{
	public:
		enum class MessageSeverity
		{
			INFO, WARNING, ERROR
		};
		
		struct Message
		{
			std::string text;
			ImVec4 severity_color;
		};
	
	public:
		static void log(const std::string& message, const MessageSeverity severity)
		{
			//TODO: change date format and check str_time size
			auto now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char str_time[26] = {};
			ctime_s(str_time, 26, &now_time);
			ImVec4 message_color;
			switch (severity)
			{
			case MessageSeverity::INFO:
				message_color = ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f };
				break;
			case MessageSeverity::WARNING:
				message_color = ImVec4{ 1.0f, 1.0f, 0.0f, 1.0f };
				break;
			case MessageSeverity::ERROR:
				message_color = ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f };
				break;
			}
			s_messages.push_back(Message{ str_time + message, message_color });
		}

		[[nodiscard]] static auto& get_messages() { return s_messages; }
		
	private:
		inline static std::vector<Message> s_messages = std::vector<Message>();
	};
}


