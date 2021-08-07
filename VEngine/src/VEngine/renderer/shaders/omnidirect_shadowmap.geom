#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

out vec4 gs_frag_pos; // FragPos from GS (output per emitvertex)

struct Uniforms
{
    mat4 light_space_transforms;
};
uniform Uniforms uniforms[6];


void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle vertex
        {
            gs_frag_pos = gl_in[i].gl_Position;
            gl_Position = uniforms[face].light_space_transforms * gs_frag_pos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}