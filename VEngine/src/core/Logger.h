#pragma once


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
			MessageSeverity severity;
		};
	
	public:
		template<class... Args>
		static void log(const std::string& message, const MessageSeverity severity, Args... args)
		{
			
			s_messages.push_back(Message{message, severity});
		}

		[[nodiscard]] static auto& get_messages() { return s_messages; }
		
	private:
		inline static std::vector<Message> s_messages = std::vector<Message>();
	};
}


