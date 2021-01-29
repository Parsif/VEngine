#pragma once


namespace vengine
{
	class RenderCommand
	{
	public:
		enum class Type
		{
			MESH_COMMAND,
			TRIANGLES_COMMAND,
		};

		[[nodiscard]] virtual Type get_type() const = 0;
		
		virtual ~RenderCommand() = default;
	};
}


