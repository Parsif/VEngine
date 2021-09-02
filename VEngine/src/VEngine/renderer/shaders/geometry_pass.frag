#version 450 core

layout(location = 0) out vec3 fr_position;
layout(location = 1) out vec3 fr_albedo;
layout(location = 2) out vec3 fr_normal;
layout(location = 3) out vec3 fr_metallic_roughness_ao;

in VertexOutput
{
	vec2 tex_coord;
	vec3 normal;
	vec3 frag_pos;
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

uniform Material u_material;

void main()
{
    fr_albedo = u_material.has_albedo ? texture(u_material.albedo_map, vs_output.tex_coord).rgb : vec3(0.5f);
    fr_metallic_roughness_ao.r  = u_material.has_metallic ? texture(u_material.metallic_map, vs_output.tex_coord).r : 0.0f;
    fr_metallic_roughness_ao.g = u_material.has_roughness ? texture(u_material.roughness_map, vs_output.tex_coord).r : 0.0f;
    fr_metallic_roughness_ao.b = u_material.has_ao ? texture(u_material.roughness_map, vs_output.tex_coord).r : 1.0f;    
    
    vec3 normal = vs_output.normal;
    if(u_material.has_normal_map)
    {
        normal = texture(u_material.normal_map, vs_output.tex_coord).rgb;
        normal = normal * 2.0 - 1.0;
        normal = normalize(vs_output.TBN * normal);
    }

    fr_normal = normal;
    fr_position = vs_output.frag_pos;
}