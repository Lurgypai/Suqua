#version 450 core

uniform vec2 camPos = vec2(0.0, 0.0);
uniform vec2 camRes;
uniform vec2 zoom = vec2(1.0, 1.0);

uniform vec2 windowRes;

uniform int start = 0;
uniform int size = 0;

struct Particle
{
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
    
    vec2 windowPos = vec2(-50.0, -50.0);
    if(part.life > 0) {
        vec2 screenSize = camRes;
        
        if(screenSize.x > windowRes.x)
            screenSize.x = windowRes.x;
            
        if(screenSize.y > windowRes.y)
            screenSize.y = windowRes.y;
        
        //position
        vec2 pixelPos = (part.pos - camPos);
        vec2 camWindowPos = pixelPos / (screenSize / zoom);
        camWindowPos.y = (-camWindowPos.y) + 1;
        //scale camera into the window
        windowPos = camWindowPos * (screenSize / windowRes);
        windowPos = 2 * (windowPos) - 1;
	}
	gl_Position = vec4(windowPos, 0.0, 1.0);
}