#version 450 core

const float PI = 3.14159265359;
const vec2 poisson_disk[4] = vec2[](
      vec2(-0.94201624, -0.39906216),
      vec2(0.94558609, -0.76890725),
      vec2(-0.094184101, -0.92938870),
      vec2(0.34495938, 0.29387760)
);
const uint MAX_LIGHTS = 4;

layout (location = 0) out vec4 fr_color;
layout (location = 1) out vec4 fr_bright_color;

in VertexOutput
{
	vec2 tex_coord;
	vec3 normal;
	vec3 frag_pos;
    vec4 frag_pos_light_space[MAX_LIGHTS];
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

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
    float light_radius;
    samplerCube shadow_map;
};

struct SphereAreaLight
{
    vec3 position;
    vec3 color;
    float intensity;
    float light_radius;
    float source_radius;
};

struct TubeAreaLight
{
    vec3 position1;
    vec3 position2;
    vec3 color;
    float intensity;
    float light_radius;
    float source_radius;
};

uniform Material u_material;
uniform DirLight u_dirlights[MAX_LIGHTS];
uniform PointLight u_point_lights[MAX_LIGHTS];
uniform SphereAreaLight u_sphere_area_lights[MAX_LIGHTS];
uniform TubeAreaLight u_tube_area_lights[MAX_LIGHTS];


uniform vec3 u_view_pos;

uniform float u_far_plane; 

uniform samplerCube u_irradiance_map;

uniform int u_number_of_dir_lights;
uniform int u_number_of_point_lights;
uniform int u_number_of_sphere_area_lights;
uniform int u_number_of_tube_area_lights;

uniform float u_bloom_threshold;
uniform float u_bloom_intensity;

float random(vec4 seed4);

float distribution_GGX(vec3 N, vec3 H, float roughness);
float geometry_schlick_GGX(float NdotV, float roughness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  fresnel_schlick(float cosTheta, vec3 F0);
vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness);

float calc_shadow(vec3 normal, vec3 surface_to_light, sampler2D shadow_map, vec4 frag_pos_light_space);
float calc_omni_shadow(vec3 normal, vec3 surface_to_light, samplerCube shadow_map);


void main()
{
    vec3 albedo     = u_material.has_albedo ? texture(u_material.albedo_map, vs_output.tex_coord).rgb : vec3(0.5f);
    float metallic  = u_material.has_metallic ? texture(u_material.metallic_map, vs_output.tex_coord).r : 0.0f;
    float roughness = u_material.has_roughness ? texture(u_material.roughness_map, vs_output.tex_coord).r : 0.0f;
    
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

    //dirlights
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

    //point lights
    for(int i = 0; i < u_number_of_point_lights; i++)
    {
        vec3 surface_to_light = normalize(u_point_lights[i].position - vs_output.frag_pos);
        vec3 half_way_direction = normalize(view_direction + surface_to_light);
        float distance_to_light = length(u_point_lights[i].position - vs_output.frag_pos);
        float attenuation = pow(clamp(1 - pow(distance_to_light / u_point_lights[i].light_radius, 4), 0, 1), 2) / (distance_to_light * distance_to_light + 1);
        vec3 radiance = u_point_lights[i].color * u_point_lights[i].intensity * attenuation;

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

        color *= (1 - calc_omni_shadow(normal, vs_output.frag_pos -  u_point_lights[i].position, u_point_lights[i].shadow_map));
        L0 += color;
    }

    for(int i = 0; i < u_number_of_sphere_area_lights; i++)
    {
        vec3 r = reflect(-view_direction, normal);
        vec3 surface_to_light = u_sphere_area_lights[i].position - vs_output.frag_pos;
        vec3 centerToRay = (dot(surface_to_light, r) * r) - surface_to_light;
        vec3 closestPoint = surface_to_light + centerToRay * clamp(u_sphere_area_lights[i].source_radius / length(centerToRay), 0, 1);
        surface_to_light = normalize(closestPoint);
        float distance_to_light = length(closestPoint);

        vec3 half_way_direction = normalize(view_direction + surface_to_light);
        float attenuation = pow(clamp(1 - pow(distance_to_light / u_sphere_area_lights[i].light_radius, 4), 0, 1), 2) / (distance_to_light * distance_to_light + 1);
        vec3 radiance = u_sphere_area_lights[i].color * u_sphere_area_lights[i].intensity * attenuation;
   
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
        L0 += color;
    }

    for(int i = 0; i < u_number_of_tube_area_lights; i++)
    {
        vec3 r = reflect(-view_direction, normal);
        vec3 L0 = u_tube_area_lights[i].position1 - vs_output.frag_pos;
        vec3 L1 = u_tube_area_lights[i].position2 - vs_output.frag_pos;

        float distL0 = length( L0 );
        float distL1 = length( L1 );
        float NoL0 = dot( L0, normal ) / ( 2.0 * distL0 );
        float NoL1 = dot( L1, normal ) / ( 2.0 * distL1 );
        float NoL = ( 2.0 * clamp( NoL0 + NoL1, 0, 1 )) / ( distL0 * distL1 + dot( L0, L1 ) + 2.0 );
        vec3 Ldist = L1 - L0;
        float RoLd = dot( r, Ldist);
        float distLd = length(Ldist);
        float t = ( dot( r, L0 ) * RoLd - dot( L0, Ldist) ) / ( distLd * distLd - RoLd * RoLd );

        vec3 closestPoint = L0 + Ldist * clamp(t, 0, 1);
        vec3 centerToRay = dot(closestPoint, r) * r - closestPoint;
        closestPoint = closestPoint + centerToRay * clamp(u_tube_area_lights[i].source_radius / length(centerToRay), 0, 1);
        vec3 surface_to_light = normalize(closestPoint);
        float distance_to_light = length(closestPoint);

        vec3 half_way_direction = normalize(view_direction + surface_to_light);
        float attenuation = pow(clamp(1 - pow(distance_to_light / u_tube_area_lights[i].light_radius, 4), 0, 1), 2) / (distance_to_light * distance_to_light + 1);
        vec3 radiance = u_sphere_area_lights[i].color * u_sphere_area_lights[i].intensity * attenuation;
   
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
        L0 += color;
    }


    //abmient
    vec3 kS = fresnel_schlick_roughness(max(dot(normal, view_direction), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(u_irradiance_map, normal).rgb;
    vec3 diffuse    = irradiance * albedo;
    vec3 ambient    = kD * diffuse; 
    if(u_material.has_ao)
    {
        float ao = texture(u_material.ao_map, vs_output.tex_coord).r;
        ambient = ambient * ao;
    }
    L0 += ambient;

    fr_color = vec4(L0, 1.0);

     //for bloom
    float brigthness = dot(fr_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brigthness > u_bloom_threshold)
    {
        fr_bright_color = fr_color * u_bloom_intensity;
    }
    else
    {
        fr_bright_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}   


float random(vec4 seed4)
{
   float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
   return fract(sin(dot_product) * 43758.5453);
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

vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  

float calc_shadow(vec3 normal, vec3 surface_to_light, sampler2D shadow_map, vec4 frag_pos_light_space)
{
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    if(proj_coords.z > 1)
    {
        return 1.0f;
    }

    const float current_depth = proj_coords.z;

    const float bias = max(0.05 * (1.0 - dot(normal, surface_to_light)), 0.005); 
    const vec2 TEXEL_SIZE = 1.0 / textureSize(shadow_map, 0);

    float shadow = 0.0;

    for (int i=0; i < poisson_disk.length(); i++)
    {
       vec4 seed = vec4(gl_FragCoord.xyz, i);
       int index = int(16.0*random(seed))%16;
       float pcf_depth = texture(shadow_map, proj_coords.xy + poisson_disk[index] / 700.0 ).r;
       shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;  
    }

    return shadow / 9.0;
}


float calc_omni_shadow(vec3 normal, vec3 surface_to_light, samplerCube shadow_map)
{
    const vec3 sample_offset_directions[20] = vec3[]
    (
       vec3(1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
       vec3(1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3(1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3(1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3(0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    float current_depth = length(surface_to_light);
    float shadow = 0.0;
    const float bias = max(0.05 * (1.0 - dot(normal, surface_to_light)), 0.005); 
    float view_distance = length(u_view_pos - vs_output.frag_pos);
    float disk_radius = (1.0 + (view_distance / u_far_plane)) / 25.0;  
    for(int i = 0; i < sample_offset_directions.length(); ++i)
    {
        float closest_depth = texture(shadow_map, surface_to_light + sample_offset_directions[i] * disk_radius).r;
        closest_depth *= u_far_plane;   // undo mapping [0;1]
        if(current_depth - bias > closest_depth)
            shadow += 1.0;
    }
    shadow /= float(sample_offset_directions.length());  

    return shadow;
}
