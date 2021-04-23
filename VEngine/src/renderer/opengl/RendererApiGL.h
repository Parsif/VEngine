#pragma once

#include "core/Vtypes.h"

#include "Buffer.h"
#include "FrameBufferGL.h"
#include "VertexArray.h"

#include "../RenderPassDescriptor.h"
#include "renderer/Material.h"
#include "renderer/RenderCommand.h"

namespace vengine
{
	class RendererApiGL
	{
	public:
		void init();
		void shutdown();

		void begin_render_pass(const RenderPassDescriptor& descriptor);
		void end_render_pass() const;

		void set_viewport(int x, int y, unsigned int width, unsigned int height);

		void draw_elements(RenderCommand& triangle_command) const;

	private:
		void prepare_drawing(RenderCommand& command) const;

		
	private:
		struct Viewport
		{
			int x = 0;
			int y = 0;
			unsigned int width = 0;
			unsigned int height = 0;
		};
		
		Ref<VertexBuffer> m_vertex_buffer;
		Ref<IndexBuffer> m_index_buffer;
		Ref<VertexArray> m_vertex_array;
		//TODO: CHANGE DEFAULT INIT OF BUFFER LAYOUT
		BufferLayout m_buffer_layout{};
		Material m_material{};
		
		Viewport m_viewport;

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
			Logger::log(source_str + ", " + type_str + ", " + severity_str + ", " + std::to_string(id) + ", " + message, Logger::MessageSeverity::ERROR);
		}
#endif

	};


	
	
}



