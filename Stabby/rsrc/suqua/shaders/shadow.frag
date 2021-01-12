#version 450 core

in vec2 TexCoord;

out vec4 color;

layout(binding = 0) uniform sampler1D lightingStrip;
layout(binding = 1) uniform sampler2D Image;

uniform vec2 camPos;
uniform vec2 camRes;
uniform vec2 lightPos;
uniform uint radius;
uniform uint rayCount;
uniform uint tick;

float penetration = 20;
int bands = 8;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	
    vec2 screenPos = lightPos - camPos;
    
	float dis = distance(gl_FragCoord.xy, screenPos);
	float light;
    //color = vec4(1.0, 0.0, 0.0, 1.0);
	if(dis < radius) {
        float normalDis = (dis / float(radius));
		vec2 sideLengths = vec2(gl_FragCoord.x - screenPos.x, gl_FragCoord.y - screenPos.y);
		float angle = degrees(atan(sideLengths.y, sideLengths.x));
        while(angle >= 360)
            angle -= 360;
        while(angle < 0)
            angle += 360;
        //color = vec4(vec3(angle/360), 1.0);
		//convert angle to x coord to be used in texture lookup
		float circlePos = (angle / 360);
		vec4 stripColor = texelFetch(lightingStrip, int(circlePos * rayCount), 0);
		//if its not penetrating
        float pixelStripDis = stripColor.r * float(radius) + (rand(floor(gl_FragCoord.xy + tick)) * 2);
        
        if (dis < pixelStripDis) {
			light = 1 - normalDis;
		}
        else if(dis < pixelStripDis + penetration) {
            light = (1 - normalDis) * (1.0 - (dis - pixelStripDis) / penetration);
        }
        else {
            light = 0;
        }
	} else {
		light = 0;
	}
	
    vec4 imageColor = texture(Image, TexCoord);
    
    light = float(int(light * bands)) / bands;
    
	color = vec4(imageColor.rgb * light, 1.0);
}