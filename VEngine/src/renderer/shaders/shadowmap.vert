#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 u_light_space_matrix;
uniform mat4 u_tranform;

void main()
{
    gl_Position = u_light_space_matrix * u_tranform * vec4(position, 1.0);
}  