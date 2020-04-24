#version 450 core

layout( local_size_x = 1000) in;

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

float random(uint x){
    return fract(sin(x) * 43758.5453);
}

void main () {
    uint id = gl_GlobalInvocationID.x;
    Particle p = data[id];
    if(p.life > 0) {
        float angle = p.angle;
        
        angle = sin(radians(p.life)) * 60 + 90 + random(id) * 5;
        
        data[id].angle = angle;
        data[id].pos += vec2(p.vel * cos(radians(p.angle - 90)), p.vel * sin(radians(p.angle - 90)));
        data[id].life--;
    }
}