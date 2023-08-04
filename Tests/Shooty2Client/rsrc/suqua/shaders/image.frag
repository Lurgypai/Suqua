#version 430 core

layout(binding = 0) uniform sampler2D Img;

in vec2 FragCoord;
in vec4 colorOverlay;
in float a;

out vec4 FragColor;

void main() {
	FragColor = texture(Img, FragCoord);
    FragColor.rgb = mix(FragColor.rgb, colorOverlay.rgb, a);
    FragColor.a *= colorOverlay.a;
    if(FragColor.a == 0)
        discard;
}