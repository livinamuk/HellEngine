#version 330 core
//layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out float gMousePick;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform bool PBR;
uniform bool hasNormalMap;
uniform bool worldBasedTexCoords;
uniform bool worldBasedTexFloorCoords;
uniform bool X_Aligned;

uniform highp float MousePickColor;

uniform sampler2D diffuseTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform sampler2D normalMap;
 
uniform float roughness;	// when no material present
uniform float metallic; 	// when no material present


void main()
{    
	// Tex Coords
	vec2 finalTexCoords = TexCoords;
	
	if (worldBasedTexCoords && X_Aligned)
		finalTexCoords.x = FragPos.x;
	else if (worldBasedTexCoords && !X_Aligned)
		finalTexCoords.x = FragPos.z;

	// floor tex coords
	if (worldBasedTexFloorCoords)
	{
	finalTexCoords.x = FragPos.x;
	finalTexCoords.y = FragPos.z;
	}		
	

    //gPosition = FragPos;
	gAlbedoSpec.rgb = texture(diffuseTexture, finalTexCoords).rgb;
	gMousePick = MousePickColor;


	// Normals
	if (hasNormalMap) {
		gNormal.rgb = texture(normalMap, finalTexCoords).rgb;
		gNormal.rgb = normalize(gNormal.rgb * 2.0 - 1.0);
		gNormal.rgb = normalize(TBN * gNormal.rgb);
	}
	else
		gNormal.rgb = normalize(Normal);


	// PBR
	if (PBR) {
		gAlbedoSpec.a = texture(metallicTexture, finalTexCoords).r;    
		gNormal.a = texture(roughnessTexture, finalTexCoords).r;
	}
	else {
		gAlbedoSpec.a = metallic;
		gNormal.a = roughness;
	}
}