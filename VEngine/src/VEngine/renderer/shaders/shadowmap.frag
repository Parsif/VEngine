#version 450 core

out float frag_depth;

void main()
{     
	frag_depth = gl_FragCoord.z;
}  