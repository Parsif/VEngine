#version 450 core

in vec3 vs_near_point;
in vec3 vs_far_point;
in mat4 vs_view_projection;

out vec4 fr_color;


vec4 grid(vec3 frag_pos_3d, float scale)
{
    vec2 coord = frag_pos_3d.xz * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(frag_pos_3d.x > -0.1 * minimumx && frag_pos_3d.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(frag_pos_3d.z > -0.1 * minimumz && frag_pos_3d.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}


float compute_depth(vec3 position)
{
    vec4 clip_space_pos = vs_view_projection * vec4(position, 1.0);
    float far = gl_DepthRange.far;
	float near = gl_DepthRange.near;
    return (((far-near) * clip_space_pos.z / clip_space_pos.w) + near + far) / 2.0;
}

// TODO: y == 0 no image
void main() 
{
    float t = -vs_near_point.y / (vs_far_point.y - vs_near_point.y);
    vec3 frag_pos = vs_near_point + t * (vs_far_point - vs_near_point);
    gl_FragDepth = compute_depth(frag_pos);

    fr_color = (grid(frag_pos, 1)) * ((t > 0) ? 1 : 0);
}