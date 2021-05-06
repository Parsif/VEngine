#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

out VertexOutput
{
	vec2 tex_coord;
	vec3 normal;
	vec3 frag_pos;
	vec4 frag_pos_light_space;
	mat3 TBN;
} vs_output;


uniform mat4 u_transform;
uniform mat4 u_view_projection;
uniform mat4 u_light_space_matrix;

void main()
{
	vs_output.frag_pos = (u_transform * vec4(position, 1.0) ).xyz;
	gl_Position = u_view_projection * vec4(vs_output.frag_pos, 1.0f);
	vs_output.tex_coord = tex_coord;
	vs_output.normal = normal;
	vs_output.frag_pos_light_space = u_light_space_matrix * vec4(vs_output.frag_pos, 1.0f);

	vec3 T = normalize(vec3(u_transform * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(u_transform * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);

	vec3 B = cross(N, T);

    vs_output.TBN = mat3(T, B, N);
}	