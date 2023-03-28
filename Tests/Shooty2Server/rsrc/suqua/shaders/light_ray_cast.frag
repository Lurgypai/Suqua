 #version 430 core

in vec2 TexCoord;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D OccludeMap;


uniform vec2 camPos;
uniform vec2 camRes;
uniform vec2 lightPos;
uniform uint radius;
uniform uint rayCount;

void main() {
	//use x coord to calculate an angle, and from there cast a ray check for obstructions. run a check 
	vec3 color = vec3(1.0);
	
	float angle = radians(360 * (gl_FragCoord.x / float(rayCount)));
	vec2 slope = vec2(cos(angle), sin(angle));
	
	vec2 screenPos = vec2(lightPos.x - camPos.x, camRes.y - (lightPos.y - camPos.y));
	vec2 pos = screenPos;
    
	while (distance(pos, screenPos) < radius) {
		pos += slope;
		vec4 mapColor = texelFetch(OccludeMap, ivec2(pos), 0);
		
        if(mapColor.xyz == 1.0) {
            color = vec3(float(distance(screenPos, pos)) / float(radius));
            break;
        }
	}
	
	FragColor = vec4(color, 1.0);
    
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

//use a framebuffer with an occlusion map drawn into it