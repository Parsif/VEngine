#version 430 core
layout(location = 0) in vec3 tex_coords;

out vec3 vs_tex_coords;

uniform mat4 u_view_projection;

void main()
{
	vec4 pos = u_view_projection * vec4(tex_coords, 1.0f);
	gl_Position = pos.xyww;
	vs_tex_coords = tex_coords;
}	