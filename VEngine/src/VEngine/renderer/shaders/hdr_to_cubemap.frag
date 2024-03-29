#version 450 core

out vec4 frag_color;
in vec3 vs_pos;

uniform sampler2D u_equirectangular_map;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(vs_pos)); // make sure to normalize localPos
    vec3 color = texture(u_equirectangular_map, uv).rgb;
    
    frag_color = vec4(color, 1.0);
}