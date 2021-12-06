#version 450 core
in vec4 Color;
out vec4 FragColor;

void main () {
	FragColor = Color;
    if(FragColor.a == 0)
        discard;
}