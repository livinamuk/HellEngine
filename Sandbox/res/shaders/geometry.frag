#version 330 core
layout (location = 0) out vec4 gEmissive;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gAO;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform vec3 emissiveColor;
uniform bool hasEmissiveMap;
uniform int TEXTURE_FLAG;
uniform vec2 TEXTURE_SCALE;

uniform highp float MousePickColor;


//uniform sampler2DArray materialTextures;
uniform sampler2D diffuseTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform sampler2D normalMap;
uniform sampler2D emissiveMap;
uniform sampler2D AO_Texture;

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
	
	// Floor
	if (TEXTURE_FLAG == 3)								
		finalTexCoords = vec2(FragPos.x, FragPos.z);		// Regular
	if (TEXTURE_FLAG == 4)								
		finalTexCoords = vec2(FragPos.z, FragPos.x);		// Rotate 90 degrees
		
	finalTexCoords.x *= TEXTURE_SCALE.x;
	finalTexCoords.y *= TEXTURE_SCALE.y;

	// AO
	gAO.rgb = texture(AO_Texture, finalTexCoords).rgb;

	// Base colour
	vec4 baseColor = texture(diffuseTexture, finalTexCoords);
	gAlbedoSpec = baseColor;

	// Normals
	gNormal.rgb = texture(normalMap, finalTexCoords).rgb;
	gNormal.rgb = normalize(gNormal.rgb * 2.0 - 1.0);
	gNormal.rgb = normalize(TBN * gNormal.rgb);

	// Metallic
	gEmissive.a = texture(metallicTexture, finalTexCoords).r; 

	// Roughness
	gNormal.a = texture(roughnessTexture, finalTexCoords).r;

	// Emissive
	//if (hasEmissiveMap)
	//{
		vec3 emissiveAmount = texture(emissiveMap, finalTexCoords).rgb;
		gEmissive.rgb = emissiveAmount * emissiveColor;
	//}
	//else
	//	gEmissive.rgb = vec3(0);
}