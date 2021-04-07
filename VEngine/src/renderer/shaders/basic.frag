#version 430 core

out vec4 fr_color;

in vec2 vs_tex_coord;
in vec3 vs_normal;
in vec3 vs_frag_pos;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 color;

    float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;
};

uniform Material u_material;
uniform DirLight u_dirlight;
uniform vec3 u_view_pos;

vec3 calc_directional_light();

void main()
{
	fr_color = vec4(calc_directional_light(), 1.0);

    // apply gamma correction
    const float gamma = 2.2;
    fr_color = vec4(pow(fr_color.rgb, vec3(1.0/ gamma)), 1.0);
}   

vec3 calc_directional_light()
{
    //ambient
    vec3 ambient = u_dirlight.ambient_intensity * texture(u_material.diffuse, vs_tex_coord).rgb;

    //diffuse
     vec3 normal = normalize(vs_normal);
     vec3 light_direction = normalize(-u_dirlight.direction);

     float diffuse_factor = max(dot(normal, light_direction), 0.0);
     vec3 diffuse = u_dirlight.color * u_dirlight.diffuse_intensity * diffuse_factor * texture(u_material.diffuse, vs_tex_coord).rgb;

    //specular
    vec3 view_dir = normalize(u_view_pos - vs_frag_pos);
    vec3 reflect_dir = reflect(-light_direction, normal);
    float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = u_dirlight.color * u_dirlight.specular_intensity * specular_factor * texture(u_material.specular, vs_tex_coord).rgb;
    return ambient + diffuse + specular;
}