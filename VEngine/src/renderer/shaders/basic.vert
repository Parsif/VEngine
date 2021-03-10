#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coords;

out vec2 vs_tex_coords;

uniform mat4 u_transform;
uniform mat4 u_view_projection;

void main()
{
	gl_Position = u_view_projection * u_transform * vec4(position, 1.0f);
	vs_tex_coords = tex_coords;
}	