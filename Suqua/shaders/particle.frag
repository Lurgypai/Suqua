#version 450 core
out vec4 FragColor;

void main () {
    vec3 color = vec3(1.0, 1.0, 1.0);
	FragColor = vec4(color, 1.0);
}