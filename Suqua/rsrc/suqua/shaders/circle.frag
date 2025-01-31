#version 430 core

in vec2 ScreenPos;
in vec2 ScreenAxes;

uniform vec4 Color;

out vec4 FragColor;

void main() {
    float alpha = 0;
    if(length(ScreenPos - gl_FragCoord.xy) < ScreenAxes.x) {
        alpha = Color.a;
    }
	FragColor = vec4(Color.rgb, alpha);
}