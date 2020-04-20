#version 450 core

layout(binding = 0) uniform sampler2D Image;
layout(binding = 1) uniform sampler2D colorPallete;

in vec2 TexCoord;

out vec4 FragColor;

uniform vec2 palleteSize = vec2(6, 2);

void main () {
    vec4 startingColor = texture(Image, TexCoord);
    //should be encoded into the rg;
    vec2 colorPos = startingColor.rg;
	FragColor = texture(colorPallete, colorPos);
}