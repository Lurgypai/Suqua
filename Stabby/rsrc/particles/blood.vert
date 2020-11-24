#version 450 core

float width = 640.0;
float height = 360.0;

uniform ivec2 camPos;
uniform float zoom;

layout(binding = 0) uniform sampler2D Image;
layout( local_size_x = 1000) in;

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

float random(uint x){
    return fract(sin(x) * 43758.5453);
}

void main () {
    uint id = gl_GlobalInvocationID.x;
    Particle p = data[id];
    if(p.life > 0) {
        uint randseed = id * uint(p.life);
        float angle = p.angle;
        float vel = p.vel;
        float gravity = .07;
        float airResistance = 0.0095;
        
        vec2 velComps = vec2(p.vel * cos(radians(angle)), p.vel * sin(radians(angle)));
        velComps.y += gravity;
        if(velComps.x >= airResistance)
            velComps.x -= airResistance;
        else if(velComps.x <= -airResistance)
            velComps.x += airResistance;
        else
            velComps.x = 0;
            
        vel = sqrt((velComps.x * velComps.x) + (velComps.y * velComps.y));
        if(velComps.x != 0)
            angle = degrees(atan(velComps.y, velComps.x));
        else if(velComps.y >= 0)
            angle = 90;
        else if(velComps.y < 0)
            angle = 270;
            
        data[id].vel = vel;
        data[id].angle = angle;
        
        vec2 newPos = data[id].pos + vec2(vel * cos(radians(angle)), vel * sin(radians(angle)));
        vec2 res = vec2(textureSize(Image, 0));
        
        //position
        vec2 pixelPos = (newPos - camPos);
        vec2 fragCoord = pixelPos / (res / zoom);
        fragCoord.y = (-fragCoord.y) + 1;
        
        vec4 color = texture(Image, fragCoord);
        if(color.r == 0.0)
            data[id].pos = newPos;
        else
            data[id].vel = 0;
            
        data[id].life -= 1;
    }
}