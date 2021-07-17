#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

const uint MAX_DIR_LIGHTS = 4;

out VertexOutput
{
	vec2 tex_coord;
	vec3 normal;
	vec3 frag_pos;
    vec4 frag_pos_light_space[MAX_DIR_LIGHTS];
	mat3 TBN;
} vs_output;


struct DirLight
{
    vec3 position;
    vec3 color;
    float intensity;
	mat4 light_space_matrix;
    sampler2D shadow_map;
};

uniform int u_dir_light_count;

uniform mat4 u_transform;
uniform mat4 u_view_projection;
uniform DirLight u_dirlights[MAX_DIR_LIGHTS];
uniform int u_number_of_dir_lights;

void main()
{
	vs_output.frag_pos = (u_transform * vec4(position, 1.0) ).xyz;
	gl_Position = u_view_projection * vec4(vs_output.frag_pos, 1.0f);
	vs_output.tex_coord = tex_coord;
	vs_output.normal = normal;
	
	for(int i = 0; i < u_number_of_dir_lights; i++)
	{
		vs_output.frag_pos_light_space[i] = u_dirlights[i].light_space_matrix * vec4(vs_output.frag_pos, 1.0f);
	}


	vec3 T = normalize(vec3(u_transform * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(u_transform * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);

	vec3 B = cross(N, T);

    vs_output.TBN = mat3(T, B, N);
}	