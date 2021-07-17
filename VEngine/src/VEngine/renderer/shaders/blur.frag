#version 450 core

out vec4 fr_color;

in vec2 vs_tex_coords;

uniform sampler2D u_image;
uniform bool u_horizontal;

void main()
{
	float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); 

	vec2 tex_offset = 1.0 / textureSize(u_image, 0);
	vec3 result = texture(u_image, vs_tex_coords).rgb * weights[0];
	if(u_horizontal)
	{
		for(int i = 1; i < 5; i++)
		{
			result += texture(u_image, vs_tex_coords + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
			result += texture(u_image, vs_tex_coords - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
		}
	}
	else 
	{
		for(int i = 1; i < 5; i++)
        {
            result += texture(u_image, vs_tex_coords + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
            result += texture(u_image, vs_tex_coords - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
        }
	}

	fr_color = vec4(result, 1.0);
}