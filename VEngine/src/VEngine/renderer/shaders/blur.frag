#version 450 core

out vec4 fr_color;

in vec2 vs_tex_coords;

uniform sampler2D u_image;
uniform bool u_horizontal;

void main()
{
	float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
	float weights[3] = float[](0.2270270270, 0.3162162162, 0.0702702703); 

	vec2 tex_offset = 1.0 / textureSize(u_image, 0);
	vec3 result = texture(u_image, vs_tex_coords).rgb * weights[0];
	if(u_horizontal)
	{
		for(int i = 1; i < weights.length(); i++)
		{
			result += texture(u_image, vs_tex_coords + vec2(tex_offset.x * offset[i], 0.0)).rgb * weights[i];
			result += texture(u_image, vs_tex_coords - vec2(tex_offset.x * offset[i], 0.0)).rgb * weights[i];
		}
	}
	else 
	{
		for(int i = 1; i < weights.length(); i++)
        {
            result += texture(u_image, vs_tex_coords + vec2(0.0, tex_offset.y * offset[i])).rgb * weights[i];
            result += texture(u_image, vs_tex_coords - vec2(0.0, tex_offset.y * offset[i])).rgb * weights[i];
        }
	}

	fr_color = vec4(result, 1.0);
}
