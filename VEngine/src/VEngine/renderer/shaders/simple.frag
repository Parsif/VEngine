#version 450 core

out vec4 fr_color;

in vec2 vs_tex_coords;
uniform sampler2D u_image;

void main()
{
	fr_color = texture(u_image, vs_tex_coords);
	  
}