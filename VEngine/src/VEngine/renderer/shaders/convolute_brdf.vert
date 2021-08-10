#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;

out vec3 vs_pos;
out vec2 vs_tex_coord;

void main()
{
    gl_Position =  vec4(position, 1.0);
    vs_tex_coord = tex_coords;
}