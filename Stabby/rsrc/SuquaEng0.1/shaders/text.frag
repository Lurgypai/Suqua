#version 430 core

in vec2 FragCoord;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D textImg;

void main() {
	FragColor = vec4(vec3(1.0), texture(textImg, FragCoord).r);
}