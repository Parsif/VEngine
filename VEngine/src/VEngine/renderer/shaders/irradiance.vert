#version 430 core
layout (location = 0) in vec3 position;

out vec3 vs_pos;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    vs_pos = position;  
    gl_Position =  u_projection * u_view * vec4(position, 1.0);
}