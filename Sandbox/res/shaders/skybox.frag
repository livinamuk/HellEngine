#version 400 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCubeArray cubeMap;
//uniform samplerCube cubeMap;

void main()
{    
  //  FragColor = texture(cubeMap, TexCoords);
    FragColor = texture(cubeMap, vec4(TexCoords, 2));
}