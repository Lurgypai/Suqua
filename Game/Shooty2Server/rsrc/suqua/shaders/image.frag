#version 430 core

layout(binding = 0) uniform sampler2D Img;

in vec2 FragCoord;

out vec4 FragColor;

void main() {
	FragColor = texture(Img, FragCoord);
    if(FragColor.a == 0)
        discard;
}