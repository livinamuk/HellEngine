#version 400 core
in vec4 FragPos;

uniform int lightIndex;
uniform vec3 lightPositions[16];
uniform float far_plane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPositions[lightIndex]);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}