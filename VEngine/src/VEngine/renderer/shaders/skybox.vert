#version 430 core
layout(location = 0) in vec3 position;

out vec3 vs_tex_coords;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	vs_tex_coords = position;

    mat4 rot_view = mat4(mat3(u_view)); // remove translation from the view matrix
    vec4 clip_pos = u_projection * rot_view * vec4(position, 1.0);
	gl_Position = clip_pos.xyww;
}	