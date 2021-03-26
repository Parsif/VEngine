#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coords;

layout(location = 2) in ivec4 a_bone_indices; 
layout(location = 3) in vec4 bone_weights;

out vec2 vs_tex_coords;

uniform mat4 u_transform;
uniform mat4 u_view_projection;

const int MAX_BONES = 100;
uniform mat4 u_bone_transforms[MAX_BONES];

void main()
{
	mat4 bone_transform = u_bone_transforms[a_bone_indices[0]] * bone_weights[0];
    bone_transform += u_bone_transforms[a_bone_indices[1]] * bone_weights[1];
    bone_transform += u_bone_transforms[a_bone_indices[2]] * bone_weights[2];
    bone_transform += u_bone_transforms[a_bone_indices[3]] * bone_weights[3];

	vec4 local_position = bone_transform * vec4(position, 1.0);

	gl_Position = u_view_projection * u_transform * local_position;
	vs_tex_coords = tex_coords;
}	