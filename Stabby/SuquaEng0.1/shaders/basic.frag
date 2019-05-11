#version 450 core

layout(binding = 0) uniform sampler2D Image;

in vec2 TexCoord;

out vec4 FragColor;

void main () {
	FragColor = texture(Image, TexCoord);
}