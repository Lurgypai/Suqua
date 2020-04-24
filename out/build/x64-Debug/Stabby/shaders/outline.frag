#version 450 core

uniform vec3 color = vec3(1.0, 1.0, 1.0);

layout(binding = 0) uniform sampler2D outlineBuffer;

in vec2 TexCoord;

out vec4 FragColor;

void main () {
    vec4 mapColor = texture(outlineBuffer, TexCoord);
    if(mapColor.r > 0) {
            FragColor = vec4(0, 0, 0, 0);
            return;
    }
        
    vec2 bufferRes = textureSize(outlineBuffer, 0);
    vec2 pixelPos = (TexCoord * vec2(bufferRes));
    
    for(int i = 0; i != 3; ++i) {
        for(int j = 0; j != 3; ++j) {
            vec2 targetPixelPos = vec2(pixelPos.x + i - 1, pixelPos.y + j - 1);
            vec4 targetPixelColor = texture(outlineBuffer, targetPixelPos / bufferRes);
            if(targetPixelColor.r > 0) {
                FragColor = vec4(color, 1);
                return;
            }
        }
    }
    
    
    FragColor = vec4(0, 0, 0, 0);
}