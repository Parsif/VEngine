#version 450 core
out vec4 fr_color;
  
in vec2 vs_tex_coords;

struct DirLight
{
    vec3 position;
    vec3 color;
    mat4 light_space_matrix;
    sampler2D shadow_map;
};

const uint MAX_LIGHTS = 4;
const float PI = 3.14159265359;

uniform sampler2D u_scene;

uniform DirLight u_dirlights[MAX_LIGHTS];
uniform vec3 u_view_pos;
uniform sampler2D u_position_map;

vec3 volumetric_light(DirLight dir_light);

float compute_scattering(float lightDotView)
{
    float G_SCATTERING = 0.5f;
    float result = 1.0f - G_SCATTERING * G_SCATTERING;
    result /= (4.0f * PI * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) *  lightDotView, 1.5f));
    return result;
}


void main()
{   
    vec4 scene = texture(u_scene, vs_tex_coords);
    vec3 accum_fog = volumetric_light(u_dirlights[0]);

    fr_color = scene * vec4(accum_fog, 1.0);
}

vec3 volumetric_light(DirLight dir_light)
{
    const float NB_STEPTH = 100;
    const float ditherPattern[4][4] = {{ 0.0f, 0.5f, 0.125f, 0.625f},
                                       { 0.75f, 0.22f, 0.875f, 0.375f},
                                       { 0.1875f, 0.6875f, 0.0625f, 0.5625},
                                       { 0.9375f, 0.4375f, 0.8125f, 0.3125}};

    vec3 world_pos = texture(u_position_map, vs_tex_coords).rgb;
    vec3 start_position = u_view_pos;
    vec3 ray_vector = world_pos - u_view_pos;
    float ray_length = length(ray_vector);
    vec3 ray_direction = normalize(ray_vector);
    float step_length = ray_length / NB_STEPTH;
    vec3 step1 = ray_direction * step_length;

    vec3 current_position = start_position;

    vec3 accum_fog = vec3(0.0);
    vec3 surface_to_light = normalize(dir_light.position);
    for(int i = 0; i < NB_STEPTH; i++)
    {
        vec4 world_in_shadow_map_space = u_dirlights[0].light_space_matrix * vec4(current_position, 1.0);
        world_in_shadow_map_space /= world_in_shadow_map_space.w;
        world_in_shadow_map_space = world_in_shadow_map_space * 0.5 + 0.5;
        float shadow_map_value = texture(dir_light.shadow_map, world_in_shadow_map_space.xy).r;

        if (shadow_map_value > world_in_shadow_map_space.z)
        {
            accum_fog += compute_scattering(dot(ray_direction, surface_to_light)) * dir_light.color;
        }
        current_position += step1;
    }
    
    accum_fog /= NB_STEPTH;
    return accum_fog;
}