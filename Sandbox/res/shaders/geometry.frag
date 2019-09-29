#version 330 core
layout (location = 0) out vec4 gEmissive;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out float gMousePick;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform vec3 emissiveColor;
uniform bool hasEmissiveMap;
uniform int TEXTURE_FLAG;
uniform float TEXTURE_SCALE;

uniform highp float MousePickColor;


//uniform sampler2DArray materialTextures;
uniform sampler2D diffuseTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform sampler2D normalMap;
uniform sampler2D emissiveMap;

uniform vec2 texOffset;

void main()
{    
	
	// TEXTURE_FLAG
	// 0: model's co-ordinates
	// 1: world based X aligned
	// 2: world based Z aligned
	// 3: floor based
	// 4: floor based rotated

	// Tex Coords
	vec2 finalTexCoords = TexCoords;
	
	// Walls
	if (TEXTURE_FLAG == 1)
		finalTexCoords.x = FragPos.x + texOffset.x;		// X aligned
	if (TEXTURE_FLAG == 2)
		finalTexCoords.x = FragPos.z + texOffset.y;		// Z aligned

	// Floor
	if (TEXTURE_FLAG == 3)								
		finalTexCoords = vec2(FragPos.x, FragPos.z);		// Regular
	if (TEXTURE_FLAG == 4)								
		finalTexCoords = vec2(FragPos.z, FragPos.x);		// Rotate 90 degrees

	finalTexCoords *= TEXTURE_SCALE;

	// Base colour
	vec4 baseColor = texture(diffuseTexture, finalTexCoords);
	gAlbedoSpec = baseColor;

	// Mouse pick
	gMousePick = MousePickColor;
 
	// Normals
	gNormal.rgb = texture(normalMap, finalTexCoords).rgb;
	gNormal.rgb = normalize(gNormal.rgb * 2.0 - 1.0);
	gNormal.rgb = normalize(TBN * gNormal.rgb);

	// Metallic
	gEmissive.a = texture(metallicTexture, finalTexCoords).r; 

	// Roughness
	gNormal.a = texture(roughnessTexture, finalTexCoords).r;

	// Emissive
	if (hasEmissiveMap)
	{
		vec3 emissiveAmount = texture(emissiveMap, finalTexCoords).rgb;
		gEmissive.rgb = emissiveAmount * emissiveColor;
	}
	else
		gEmissive.rgb = vec3(0);
}