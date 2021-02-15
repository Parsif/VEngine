#version 430 core
layout(location = 0) in vec3 pos;

uniform mat4 u_transform;
uniform mat4 u_view_projection;

void main()
{
	gl_Position = u_view_projection * u_transform * vec4(pos, 1.0f);
}