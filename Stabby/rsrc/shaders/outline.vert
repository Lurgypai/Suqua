#version 450 core

out vec2 TexCoord;

void main () {
	vec2 pos = vec2(gl_VertexID % 2, gl_VertexID / 2);
	TexCoord = pos;
	gl_Position = vec4( (2.0 * pos) - 1, 0.0, 1.0);
}