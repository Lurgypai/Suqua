#version 430 core

uniform vec2 camPos = vec2(0, 0);
uniform ivec2 camRes;
uniform vec2 zoom = vec2(1.0, 1.0);
uniform bool flip_vertically = true;
uniform float depth;

uniform vec2 Position;
uniform vec4 Color;
uniform float Radius;

out vec2 ScreenPos;
out vec2 ScreenAxes;

void main () {
	vec2 vertexCorner = vec2(gl_VertexID % 2, gl_VertexID / 2);
    vec2 worldPos = Position - vec2(Radius);
    worldPos = worldPos + ((vec2(Radius) * 2) * vertexCorner);
    
    vec2 pixelPos = worldPos - camPos;
    
	vec2 windowPos = pixelPos / (vec2(camRes) / zoom);
	windowPos.y = (-windowPos.y) + 1;
	windowPos = 2 * (windowPos) - 1;
    
    ScreenAxes = vec2(Radius) / zoom;
    ScreenPos = pixelPos;
    
	gl_Position = vec4(windowPos, depth, 1.0);
}