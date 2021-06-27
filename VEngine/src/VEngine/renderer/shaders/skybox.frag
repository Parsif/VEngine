#version 430 core

out vec4 fr_color;
in vec3 vs_tex_coords;

uniform samplerCube skybox;

void main()
{
	fr_color = texture(skybox, vs_tex_coords);
}