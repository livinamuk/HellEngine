#version 330 core

in vec2 TexCoords;
out vec3 FragColor;

uniform float screenWidth;
uniform float screenHeight;
uniform sampler2D inputTexture;

void main()
{             
    //vec2 pixelOffset = vec2(1.0f/screenWidth, 1.0f/screenHeight);
    //FragColor = vec3(1.0);
    //FragColor += texture(inputTexture, vec2(TexCoords.x - 4.0f * pixelOffset.x, TexCoords.y)).xyz;
    //FragColor += texture(inputTexture, vec2(TexCoords.x + 4.0f * pixelOffset.x, TexCoords.y)).xyz;
    //FragColor += texture(inputTexture, vec2(TexCoords.x, TexCoords.y - 4.0f * pixelOffset.y)).xyz;
    //FragColor += texture(inputTexture, vec2(TexCoords.x, TexCoords.y + 4.0f * pixelOffset.y)).xyz;
    //FragColor *= 0.25f;

	FragColor = texture(inputTexture, vec2(TexCoords.x * 4, TexCoords.y * 4)).xyz;
}