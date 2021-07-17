#version 450 core

out vec4 fr_color;
in vec2 vs_tex_coords;

struct Material
{
    sampler2D diffuse0;
    sampler2D specular0;
    float shininess;
};

uniform Material u_material;

void main()
{
	fr_color = texture(u_material.diffuse0, vs_tex_coords);
}   