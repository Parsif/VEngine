#version 430 core

out vec4 fr_color;
in vec3 vs_tex_coords;

uniform samplerCube u_environment_map;

void main()
{
	vec3 env_color = texture(u_environment_map, vs_tex_coords).rgb;
    
    env_color = env_color / (env_color + vec3(1.0));
    env_color = pow(env_color, vec3(1.0/2.2)); 
  
    fr_color = vec4(env_color, 1.0);
}