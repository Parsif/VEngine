#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;

out VertexOutput
{
	vec2 tex_coord;
} vs_output;


void main()
{
	gl_Position = vec4(position, 1.0);
	vs_output.tex_coord = tex_coord;	
}	