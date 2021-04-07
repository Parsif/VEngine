#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec3 normal;

out vec2 vs_tex_coord;
out vec3 vs_normal;
out vec3 vs_frag_pos;

uniform mat4 u_transform;
uniform mat4 u_view_projection;

void main()
{
	vs_frag_pos = (u_transform * vec4(position, 1.0) ).xyz;
	gl_Position = u_view_projection * vec4(vs_frag_pos, 1.0f);
	vs_tex_coord = tex_coord;
	vs_normal = normal;
}	