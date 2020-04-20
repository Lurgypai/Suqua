#version 430 core


uniform vec2 camPos = vec2(0.0, 0.0);
uniform vec2 camRes;
uniform vec2 zoom = vec2(1.0, 1.0);

uniform vec2 windowRes;

layout(location = 0) in vec2 pos;

void main() {
	vec2 screenSize = camRes;
	
	if(screenSize.x > windowRes.x)
		screenSize.x = windowRes.x;
		
	if(screenSize.y > windowRes.y)
		screenSize.y = windowRes.y;
    
	//position
	vec2 pixelPos = (pos - camPos);
	vec2 camWindowPos = pixelPos / (screenSize / zoom);
	camWindowPos.y = (-camWindowPos.y) + 1;
    //scale camera into the window
    vec2 windowPos = camWindowPos * (screenSize / windowRes);
	windowPos = 2 * (windowPos) - 1;
	
	gl_Position = vec4(windowPos, 0.0, 1.0);
}