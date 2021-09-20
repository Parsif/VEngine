#version 450 core

#define SMAA_REPROJECTION_WEIGHT_SCALE 30.0

out vec4 fr_color;
in vec2 vs_tex_coords;

uniform sampler2D u_current_frame;
uniform sampler2D u_prev_frame;
uniform sampler2D u_velocity_buffer;

void main()
{
	vec2 velocity = texture(u_velocity_buffer, vs_tex_coords).rg;
	vec4 current = texture(u_current_frame, vs_tex_coords);
	vec4 previous = texture(u_prev_frame, vs_tex_coords - velocity);
	float delta = abs(current.a * current.a - previous.a * previous.a) / 5.0;
    float weight = 0.5 * clamp(1.0 - sqrt(delta) * SMAA_REPROJECTION_WEIGHT_SCALE, 0, 1);

    // Blend the pixels according to the calculated weight:
	fr_color = mix(current, previous, weight);
}