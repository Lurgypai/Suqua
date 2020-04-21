#version 430 core

uniform vec3 color;
uniform bool anti_alias;

in vec2 FragCoord;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D textImg;

void main() {
    if(!anti_alias) {
        FragColor = vec4(color, ceil(texture(textImg, FragCoord).r));
    }
    else {
        FragColor = vec4(color, texture(textImg, FragCoord).r);
    }
}