#version 430 core
layout(location = 0) in vec3 pos;

out vec3 vs_near_point;
out vec3 vs_far_point;
out mat4 vs_view_projection;

uniform mat4 u_view;
uniform mat4 u_projection;


vec3 unproject_point(float x, float y, float z, mat4 view, mat4 projection)
{
    mat4 view_inv = inverse(view);
    mat4 projection_inv = inverse(projection);
    vec4 unprojected_point = view_inv * projection_inv * vec4(x, y, z, 1.0);
    return unprojected_point.xyz / unprojected_point.w;
}

void main()
{
    vs_near_point = unproject_point(pos.x, pos.y, 0.0, u_view, u_projection);
    vs_far_point = unproject_point(pos.x, pos.y, 1.0, u_view, u_projection);
    vs_view_projection = u_projection * u_view;
    gl_Position = vec4(pos, 1.0);
}