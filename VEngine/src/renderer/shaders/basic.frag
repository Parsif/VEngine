#version 430 core

out vec4 fr_color;

in VertexOutput
{
	vec2 tex_coord;
	vec3 normal;
	vec3 frag_pos;
    vec4 frag_pos_light_space;
} vs_output;

struct Material
{
    sampler2D diffuse;
};

struct DirLight
{
    vec3 position;
    vec3 color;
};

uniform Material u_material;
uniform DirLight u_dirlight;
uniform vec3 u_view_pos;
uniform sampler2D u_shadow_map;


vec3 calc_directional_light();
float calc_shadow(vec3 normal, vec3 light_dir);

void main()
{
	fr_color = vec4(calc_directional_light(), 1.0);

    // apply gamma correction
    const float gamma = 2.2;
    fr_color = vec4(pow(fr_color.rgb, vec3(1.0 / gamma)), 1.0);
}   

vec3 calc_directional_light()
{
    //ambient
    vec3 diffuse_texture_color = texture(u_material.diffuse, vs_output.tex_coord).rgb;
    vec3 ambient = 0.15 * diffuse_texture_color;

    //diffuse
    vec3 normal = normalize(vs_output.normal);
    vec3 light_direction = normalize(u_dirlight.position - vs_output.frag_pos);

    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = u_dirlight.color * diffuse_factor * diffuse_texture_color;

    //specular
    vec3 view_direction = normalize(u_view_pos - vs_output.frag_pos);
    vec3 half_way_direction = normalize(view_direction + light_direction);
    float specular_factor = pow(max(dot(view_direction, half_way_direction), 0.0), 32);
    vec3 specular = u_dirlight.color * specular_factor * diffuse_texture_color;

    return ambient + (diffuse + specular) * (1 - calc_shadow(normal, light_direction));
}

float calc_shadow(vec3 normal, vec3 light_dir)
{
    vec3 proj_coords = vs_output.frag_pos_light_space.xyz / vs_output.frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    const float closest_depth = texture(u_shadow_map, proj_coords.xy).r;
    const float current_depth = proj_coords.z;

    const float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005); 
    const vec2 TEXEL_SIZE = 1.0 / textureSize(u_shadow_map, 0);


    float shadow = 0.0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(u_shadow_map, proj_coords.xy + vec2(x, y) * TEXEL_SIZE).r;
            shadow += current_depth - bias > closest_depth ? 1.0 : 0;
        }
    }

    return shadow / 9.0;
}