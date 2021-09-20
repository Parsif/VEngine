#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;

out vec2 vs_tex_coords;

void main()
{
    vs_tex_coords = tex_coords;
    gl_Position = vec4(position, 1.0);
}