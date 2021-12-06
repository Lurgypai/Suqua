#version 450 core

in vec2 TexCoord;

out vec4 color;

layout(binding = 0) uniform sampler2D ShadowMap;
layout(binding = 1) uniform sampler2D SourceImage;


void main() {
	color = texture(ShadowMap, TexCoord) * texture(SourceImage, TexCoord);
}