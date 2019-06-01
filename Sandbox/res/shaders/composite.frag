#version 400 core

layout (location = 1) out vec3 FragColor;

in  vec2 TexCoords;

uniform sampler2D albedoTexture;
uniform sampler2D accumulatedLighting;

uniform float brightness;
uniform float contrast;

void main()
{
	vec3 albedo = pow(texture(albedoTexture, TexCoords).rgb, vec3(2.2));
	vec3 lighting = texture(accumulatedLighting, TexCoords).rgb;
	
	vec3 ambient = vec3(0.01) * albedo;
	vec3 color = ambient + lighting;

	color = color / (color + vec3(1.0));	// HDR tonemapping    
	color = pow(color, vec3(1.0/2.2));		// gamma correct
	color.rgb = ((color.rgb - 0.5f) * max(contrast, 0)) + 0.5f;		// Apply contrast
	color.rgb += brightness;										// Apply brightness
	
	FragColor = color;
}