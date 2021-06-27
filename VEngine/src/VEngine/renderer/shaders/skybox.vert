#version 430 core
layout(location = 0) in vec3 tex_coords;

out vec3 vs_tex_coords;

void main()
{
	gl_Position = vec4(tex_coords, 1.0f).xyww;
	vs_tex_coords = tex_coords;
}	