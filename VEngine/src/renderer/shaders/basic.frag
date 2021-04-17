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
    sampler2D specular;
};

struct DirLight {
    vec3 direction;
    vec3 color;
};

uniform Material u_material;
uniform DirLight u_dirlight;
uniform vec3 u_view_pos;
uniform sampler2D u_shadow_map;


vec3 calc_directional_light();
float calc_shadow();

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
    vec3 light_direction = normalize(-u_dirlight.direction);

    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = u_dirlight.color * diffuse_factor * diffuse_texture_color;

    //specular
    vec3 view_dir = normalize(u_view_pos - vs_output.frag_pos);
    vec3 reflect_dir = reflect(-light_direction, normal);
    float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = u_dirlight.color * specular_factor * texture(u_material.specular, vs_output.tex_coord).rgb;

    float shadow = calc_shadow();
    return ambient + (diffuse + specular) * (1 - shadow);
}

float calc_shadow()
{
    vec3 proj_coords = vs_output.frag_pos_light_space.xyz / vs_output.frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    float closest_depth = texture(u_shadow_map, proj_coords.xy).r;

    float current_depth = proj_coords.z;
    float shadow = current_depth > closest_depth ? 1.0 : 0;
    return shadow;
}