#pragma once

#include "../RenderPassDescriptor.h"
#include "renderer/Material.h"
#include "renderer/RenderCommand.h"

namespace vengine
{
	class RendererApiGL
	{
	public:
		static void init();
		static void shutdown();

		static void begin_render_pass(const RenderPassDescriptor& descriptor);
		static void end_render_pass();

		static void set_viewport(int x, int y, unsigned int width, unsigned int height);

		static void draw_elements(RenderCommand& command);
		static void draw_arrays(RenderCommand& command, unsigned int first, unsigned int count);

	private:
		void static prepare_drawing(RenderCommand& command);

		
	private:
#ifdef V_CONFIGURATION_DEBUG
		static void GLAPIENTRY
			MessageCallback(GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar* message,
				const void* userParam)
		{
			if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

			auto source_str = [source]() -> std::string {
				switch (source)
				{
				case GL_DEBUG_SOURCE_API: return "API";
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
				case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
				case GL_DEBUG_SOURCE_THIRD_PARTY:  return "THIRD PARTY";
				case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
				case GL_DEBUG_SOURCE_OTHER: return "OTHER";
				default: return "UNKNOWN";
				}
			}();

			auto type_str = [type]() {
				switch (type)
				{
				case GL_DEBUG_TYPE_ERROR: return "ERROR";
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
				case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
				case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
				case GL_DEBUG_TYPE_MARKER:  return "MARKER";
				case GL_DEBUG_TYPE_OTHER: return "OTHER";
				default: return "UNKNOWN";
				}
			}();

			auto severity_str = [severity]() {
				switch (severity) {
				case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
				case GL_DEBUG_SEVERITY_LOW: return "LOW";
				case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
				case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
				default: return "UNKNOWN";
				}
			}();
			LOG_ERROR(source_str + ", " + type_str + ", " + severity_str + ", " + std::to_string(id) + ", " + message);
		}
#endif

	};


	
	
}



