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
        float rotateSpeed = 1.5;
        
        if(angle > 180 + rotateSpeed) {
            angle -= rotateSpeed;
        } else if (angle < 180 - rotateSpeed) {
            angle += rotateSpeed;
        } else {
            angle = 180;
        }
        
        while(angle < 0)
            angle += 360;
            
        while(angle >= 360)
            angle -= 360;
        
        data[id].angle = angle;
        data[id].pos += vec2(p.vel * cos(radians(p.angle - 90)), p.vel * sin(radians(p.angle - 90)));
        data[id].life--;
    }
}