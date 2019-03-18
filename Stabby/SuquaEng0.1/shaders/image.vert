#version 430 core


uniform vec2 camPos = vec2(0.0, 0.0);
uniform vec2 camRes;
uniform vec2 zoom = vec2(1.0, 1.0);

struct ImgData
{
	vec2 objPos;	//
	vec2 imgRes;
	vec2 imgOffset;
	vec2 objRes;
	vec2 origin;
	vec2 scale;
	float angle;
};

layout (std140, binding = 0) buffer ImgDataBuffer {
	ImgData data[];
};

uniform vec2 windowRes;

out vec2 FragCoord;

void main() {
	ImgData dat = data[ gl_VertexID / 6];
	
	vec2 screenSize = camRes;
	
	if(screenSize.x > windowRes.x)
		screenSize.x = windowRes.x;
		
	if(screenSize.y > windowRes.y)
		screenSize.y = windowRes.y;
		
	screenSize /= zoom;
	
	int id = abs((gl_VertexID % 6) - 3);
	vec2 pos = vec2(id % 2, id / 2);
	vec2 imgPos = pos;
	
	//move 1, 1 to surround origin
	imgPos -= 0.5;
	imgPos *= dat.objRes;
	imgPos += (abs(dat.objRes) / 2.0);
	
	imgPos += dat.imgOffset;
	imgPos /= dat.imgRes;
	FragCoord = imgPos;
	
	vec2 pixelPos = (pos * abs(dat.objRes));
	
	//scale about origin
	pixelPos = ((pixelPos - dat.origin) * dat.scale) + dat.origin;
	
	//rotate about origin
	float magn = distance(dat.origin, pixelPos);
	vec2 diff = pixelPos - dat.origin;
	float newAngle = atan(diff.y, diff.x) + radians(dat.angle);
	pixelPos = vec2(magn * cos(newAngle), magn * sin(newAngle));
	pixelPos += dat.origin;
	
	//position
	pixelPos += (dat.objPos - camPos);
	vec2 windowPos = pixelPos / screenSize;
	windowPos.y = (-windowPos.y) + 1;
	windowPos = 2 * (windowPos) - 1;
	
	gl_Position = vec4(windowPos, 0.0, 1.0);
}