#version 450 core

uniform ivec2 camPos;
uniform float zoom;

layout(binding = 0) uniform sampler2D Image;
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
        
        vec2 prevPos = data[id].pos;
        vec2 newPos = prevPos + vec2(p.vel * cos(radians(p.angle - 90)), p.vel * sin(radians(p.angle - 90)));
        vec2 res = vec2(textureSize(Image, 0));
        
        //position
        vec2 pixelPos = (newPos - camPos);
        vec2 fragCoord = pixelPos / (res / zoom);
        fragCoord.y = (-fragCoord.y) + 1;
        
        vec4 color = texture(Image, fragCoord);
        if(color.r == 0.0)
            data[id].pos = newPos;
        
        data[id].life -= 1;
    }
}