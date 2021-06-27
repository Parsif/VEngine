#pragma once
#include <imgui.h>

#define LOG_INFO(message) Logger::info(message, __FILE__, __LINE__);
#define LOG_WARNING(message) Logger::warning(message, __FILE__, __LINE__);
#define LOG_ERROR(message) Logger::error(message, __FILE__, __LINE__);


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
		static void info(const std::string& message, const std::string& filename, int line_number)
		{
			std::string result = "File " + filename.substr(filename.find_last_of("/\\") + 1) + " line: " + std::to_string(line_number) + " : " + message + '\n';
			s_info_text_buffer.append(result.c_str());
		}

		static void warning(const std::string& message, const std::string& filename, int line_number)
		{
			std::string result = "File " + filename.substr(filename.find_last_of("/\\") + 1) + " line: " + std::to_string(line_number) + " : " + message + '\n';
			s_warning_text_buffer.append(result.c_str());
		}

		static void error(const std::string& message, const std::string& filename, int line_number)
		{
			std::string result = "File " + filename.substr(filename.find_last_of("/\\") + 1) + " line: " + std::to_string(line_number) + " : " + message + '\n';
			s_error_text_buffer.append(result.c_str());
		}
		
		[[nodiscard]] static auto& get_info_text_buffer() { return s_info_text_buffer; }
		[[nodiscard]] static auto& get_warning_text_buffer() { return s_warning_text_buffer; }
		[[nodiscard]] static auto& get_error_text_buffer() { return s_error_text_buffer; }
	
	private:
		static void clear()
		{
			s_info_text_buffer.clear();
			s_warning_text_buffer.clear();
			s_error_text_buffer.clear();
		}
	
	private:
		inline static ImGuiTextBuffer s_info_text_buffer = ImGuiTextBuffer{};
		inline static ImGuiTextBuffer s_warning_text_buffer = ImGuiTextBuffer{};
		inline static ImGuiTextBuffer s_error_text_buffer = ImGuiTextBuffer{};

		friend ConsolePanel;
	};
}


