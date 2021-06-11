#pragma once

#include <imgui.h>

namespace vengine
{
	class ConsolePanel;
	
	class Logger
	{
	public:
		enum class MessageSeverity
		{
			INFO, WARNING, ERROR
		};
	
	public:
		static void info(const std::string& message)
		{
			log(message, MessageSeverity::INFO);
		}

		static void warning(const std::string& message)
		{
			log(message, MessageSeverity::WARNING);
		}

		static void error(const std::string& message)
		{
			log(message, MessageSeverity::ERROR);
		}
		
		[[nodiscard]] static auto& get_imgui_text_buffer() { return s_imgui_text_buffer; }
	
	private:
		static void log(const std::string& message, const MessageSeverity severity)
		{
			//TODO: change date format and check str_time size
			auto now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char str_time[26] = {};
			ctime_s(str_time, 26, &now_time);
			
			s_imgui_text_buffer.append(message.data(), message.data() + message.size() + 1);
		}

		static void clear()
		{
			s_imgui_text_buffer.clear();
		}
	
	private:
		inline static ImGuiTextBuffer s_imgui_text_buffer = ImGuiTextBuffer{};

		friend ConsolePanel;
	};
}


