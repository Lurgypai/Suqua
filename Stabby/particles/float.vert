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
        uint randseed = id * uint(p.life) + p.death;
        float angle = p.angle;
        float deceleration = 0.0009;
        float dispersion = 6;
        
        angle = sin(radians((p.life))) * 70;
        angle += random(randseed) * dispersion * 2 - dispersion;
        
        data[id].vel -= deceleration;
        if(data[id].vel < 0)
            data[id].vel = 0;
        
        data[id].angle = angle;
        data[id].pos += vec2(p.vel * cos(radians(p.angle - 90)), p.vel * sin(radians(p.angle - 90)));
        data[id].life -= 1;
    }
}