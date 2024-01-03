#version 430 core


uniform vec2 camPos = vec2(0, 0);
uniform ivec2 camRes;
uniform vec2 zoom = vec2(1.0, 1.0);
uniform bool flip_vertically = true;

struct ImgData
{
    vec4 colorOverlay;
	vec2 objPos;
	ivec2 imgRes;
	ivec2 imgOffset;
	ivec2 objRes;
	vec2 origin;
	vec2 scale;
	float angle;
    float depth;
    float a;
};

layout (std140, binding = 0) buffer ImgDataBuffer {
	ImgData data[];
};

uniform vec2 windowRes;

out vec2 FragCoord;
out vec4 colorOverlay;
out float a;

void main() {
	ImgData dat = data[ gl_VertexID / 6];
    a = dat.a;
    colorOverlay = dat.colorOverlay;
	
	int id = abs((gl_VertexID % 6) - 3);
    
	vec2 pos = vec2(id % 2, id / 2);
    vec2 h = vec2(0.5);
	vec2 imgPos = pos - h;

	vec2 objRes = vec2(dat.objRes);
	//move -0.5, 0.5 to surround origin
	imgPos *= sign(objRes);
    imgPos += h;
    
    imgPos *= abs(objRes);
	imgPos += dat.imgOffset;
    // imgPos += 0.1; //remove this line to re-enable rounding bug
	FragCoord = vec2(imgPos) / dat.imgRes;
	
	vec2 pixelPos = pos * abs(objRes);
	
	//scale about origin
	pixelPos = ((pixelPos - dat.origin) * dat.scale) + dat.origin;
	
	//rotate about origin
	float magn = distance(dat.origin, pixelPos);
	vec2 diff = pixelPos - dat.origin;
	float newAngle = atan(diff.y, diff.x) + radians(dat.angle);
	pixelPos = vec2(magn * cos(newAngle), magn * sin(newAngle));
	pixelPos += dat.origin;
    
	//position
	pixelPos += (dat.objPos - round(camPos));
    
	vec2 windowPos = pixelPos / (vec2(camRes) / zoom);
    if(flip_vertically)
        windowPos.y = (-windowPos.y) + 1;
	windowPos = 2 * (windowPos) - 1;
	
	gl_Position = vec4(windowPos, dat.depth, 1.0);
}
