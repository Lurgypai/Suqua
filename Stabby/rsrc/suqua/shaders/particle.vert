#version 450 core

uniform vec2 camPos = vec2(0.0, 0.0);
uniform vec2 camRes;
uniform vec2 zoom = vec2(1.0, 1.0);

uniform vec2 windowRes;

uniform int start = 0;
uniform int size = 0;

out vec4 Color;

struct Particle
{
    vec4 color;
	vec2 pos;
    float angle;
    float vel;
    int life;
    int death;
};

layout (std140, binding = 0) buffer ParticleDataBuffer {
	Particle data[];
};

void main () {
    int id = (start + gl_VertexID) % size;
    Particle part = data[id];
    
    //outside of window if dead
    vec2 windowPos = vec2(-50, -50);
    if(part.life > 0) {
        Color = part.color;
    
        //position
        vec2 pixelPos = (part.pos - camPos);
        windowPos = pixelPos / (camRes / zoom);
        windowPos.y = (-windowPos.y) + 1;
        windowPos = 2 * (windowPos) - 1;
	}
	gl_Position = vec4(windowPos, -0.1, 1.0);
}