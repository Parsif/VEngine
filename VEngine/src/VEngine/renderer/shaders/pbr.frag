#version 430 core

const float PI = 3.14159265359;
const uint MAX_DIR_LIGHTS = 4;

out vec4 fr_color;

in VertexOutput
{
	vec2 tex_coord;
	vec3 normal;
	vec3 frag_pos;
    vec4 frag_pos_light_space[MAX_DIR_LIGHTS];
    mat3 TBN;
} vs_output;

struct Material
{
    sampler2D albedo_map;
    sampler2D metallic_map;
    sampler2D roughness_map;
    sampler2D ao_map;
    sampler2D normal_map;

    bool has_albedo;
    bool has_metallic;
    bool has_roughness;
    bool has_ao;
    bool has_normal_map;
};

struct DirLight
{
    vec3 position;
    vec3 color;
    float intensity;
    mat4 light_space_matrix;
    sampler2D shadow_map;
};

uniform Material u_material;
uniform DirLight u_dirlights[MAX_DIR_LIGHTS];
uniform int u_number_of_dir_lights;

uniform vec3 u_view_pos;

float distribution_GGX(vec3 N, vec3 H, float roughness);
float geometry_schlick_GGX(float NdotV, float roughness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  fresnel_schlick(float cosTheta, vec3 F0);

float calc_shadow(vec3 normal, vec3 light_dir, sampler2D shadow_map, vec4 frag_pos_light_space);

void main()
{
    vec3 albedo     = u_material.has_albedo ? texture(u_material.albedo_map, vs_output.tex_coord).rgb : vec3(0.5f);
    float metallic  = u_material.has_metallic ? texture(u_material.metallic_map, vs_output.tex_coord).r : 0.0f;
    float roughness = u_material.has_roughness ? texture(u_material.roughness_map, vs_output.tex_coord).r : 0.0f;

    const float gamma = 2.2;
    albedo = pow(albedo, vec3(gamma));
    
    vec3 normal = vs_output.normal;
    if(u_material.has_normal_map)
    {
        normal = texture(u_material.normal_map, vs_output.tex_coord).rgb;
        normal = normal * 2.0 - 1.0;
        normal = normalize(vs_output.TBN * normal);
    }
    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 view_direction = normalize(u_view_pos - vs_output.frag_pos);

    vec3 L0 = vec3(0.0);
    for(int i = 0; i < u_number_of_dir_lights; i++)
    {
        vec3 surface_to_light = normalize(u_dirlights[i].position);
        vec3 half_way_direction = normalize(view_direction + surface_to_light);
        vec3 radiance = u_dirlights[i].color * u_dirlights[i].intensity;

        // Cook-Torrance BRDF
        float NDF = distribution_GGX(normal, half_way_direction, roughness);        
        float G   = geometry_smith(normal, view_direction, surface_to_light, roughness);    
        vec3  F   = fresnel_schlick(max(dot(half_way_direction, view_direction), 0.0), F0);       

        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(normal, view_direction), 0.0) * max(dot(normal, surface_to_light), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(normal, surface_to_light), 0.0);                
        vec3 color = (kD * albedo / PI + specular) * radiance * NdotL; 
        //shadows
        color *= (1 - calc_shadow(normal, surface_to_light, u_dirlights[i].shadow_map, vs_output.frag_pos_light_space[i]));
        L0 += color;
    }
    //abmient
    if(u_material.has_ao)
    {
        float ao = texture(u_material.ao_map, vs_output.tex_coord).r;
        vec3 ambient = vec3(0.03) * albedo * ao;
        L0 += ambient;
    }
    
    //hdr
    L0 = L0 / (L0 + vec3(1.0));
    // apply gamma correction
    L0 = pow(L0, vec3(1.0 / gamma));
    fr_color = vec4(L0, 1.0);
}   

float distribution_GGX(vec3 N, vec3 H, float roughness)
{
     float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float geometry_schlick_GGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometry_schlick_GGX(NdotV, roughness);
    float ggx1  = geometry_schlick_GGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  

float calc_shadow(vec3 normal, vec3 light_dir, sampler2D shadow_map, vec4 frag_pos_light_space)
{
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    const float closest_depth = texture(shadow_map, proj_coords.xy).r;
    const float current_depth = proj_coords.z;

    const float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005); 
    const vec2 TEXEL_SIZE = 1.0 / textureSize(shadow_map, 0);


    float shadow = 0.0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * TEXEL_SIZE).r;
            shadow += current_depth - bias > closest_depth ? 1.0 : 0;
        }
    }

    return shadow / 9.0;
}