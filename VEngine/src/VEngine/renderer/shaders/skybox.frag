#version 450 core

out vec4 fr_color;
in vec3 vs_tex_coords;

uniform samplerCube u_environment_map;

void main()
{
	vec3 env_color = texture(u_environment_map, vs_tex_coords).rgb;
  
    fr_color = vec4(env_color, 1.0);
}