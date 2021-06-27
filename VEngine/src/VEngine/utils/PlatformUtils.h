#pragma once

#include <string>
#include <optional>

#include "core/Vtypes.h"
#include "core/Window.h"

namespace vengine
{
	class FileDialogs
	{
	public:
		[[nodiscard]] static std::optional<std::string> open_file_dialog(const char* filter, const Ref<Window> window);
		[[nodiscard]] static std::optional<std::string> save_file_dialog(const char* filter, const Ref<Window> window);
	};

	
}
