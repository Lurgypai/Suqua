#version 430 core


uniform vec2 camPos = vec2(0.0, 0.0);
uniform vec2 camRes;
uniform vec2 zoom = vec2(1.0, 1.0);
uniform float depth;

uniform vec2 windowRes;

layout(location = 0) in vec2 pos;

void main() {
    
	//position
	vec2 pixelPos = (pos - camPos);
	vec2 windowPos = pixelPos / (camRes / zoom);
	windowPos.y = (-windowPos.y) + 1;
	windowPos = 2 * (windowPos) - 1;
	
	gl_Position = vec4(windowPos, depth, 1.0);
}