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
        uint randseed = id * uint(p.life);
        float angle = p.angle;
        float rotateSpeed = 1.2;
        float deceleration = 0.02;
        
        angle = sin(radians((p.life + 2) * 4)) * 70;
        
        data[id].vel -= random(randseed) * deceleration;
        if(data[id].vel < 0)
            data[id].vel = 0;
        
        data[id].angle = angle;
        data[id].pos += vec2(p.vel * cos(radians(p.angle - 70)), p.vel * sin(radians(p.angle - 70)));
        data[id].life -= 1;
    }
}