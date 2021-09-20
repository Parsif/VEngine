#version 450 core
out vec4 fr_color;
  
in vec2 vs_tex_coords;

uniform sampler2D u_scene;
uniform sampler2D u_bloom_blur;
uniform float u_exposure;
uniform bool u_is_bloom_enabled;

void main()
{             
    vec3 hdr_color = texture(u_scene, vs_tex_coords).rgb;      
    if(u_is_bloom_enabled)
    {
        vec3 bloom_color = texture(u_bloom_blur, vs_tex_coords).rgb;
        hdr_color += bloom_color; 
    }

    vec3 result = vec3(1.0) - exp(-hdr_color * u_exposure);
   
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));
    fr_color = vec4(result, 1.0);
} 
