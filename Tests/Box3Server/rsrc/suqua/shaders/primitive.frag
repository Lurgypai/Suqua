#version 450 core

uniform vec4 Color;

out vec4 FragColor;

void main() {
	FragColor = Color;
    if(FragColor.a == 0)
        discard;
}