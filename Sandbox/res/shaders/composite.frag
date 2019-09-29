#version 400 core

layout (location = 1) out vec3 Composite;

in vec2 TexCoords;

uniform sampler2D albedoTexture;
uniform sampler2D accumulatedLighting;

uniform float contrast;

void main()
{
	// Get pixel information
	vec3 albedo = pow(texture(albedoTexture, TexCoords).rgb, vec3(2.2));
	vec3 lighting = texture(accumulatedLighting, TexCoords).rgb;
	vec3 ambient = vec3(0.001) * albedo;
	Composite = ambient + lighting;
}