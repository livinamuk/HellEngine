#version 400 core
layout (location = 4) out vec4 FragColor;

uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D emissiveTexture;
uniform samplerCubeArray shadowMaps;

uniform float lightStrengths[16];
uniform vec3 lightPositions[16];
uniform vec3 lightColors[16];
uniform float lightRadius[16];
uniform float lightAttenuationConstant[16];
uniform float lightAttenuationLinear[16];
uniform float lightAttenuationExp[16];
uniform int lightIndex;

uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;
uniform float bias;

uniform float lowerXlimit;
uniform float upperXlimit;
uniform float lowerZlimit;
uniform float upperZlimit;

const float PI = 3.14159265359;

uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;

uniform float screenWidth;
uniform float screenHeight;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos, vec3 lightPos, int light)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 50.0;
    for(int i = 0; i < samples; ++i)
    {
        //float closestDepth = texture(shadowMaps, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        float closestDepth = texture(shadowMaps, vec4(fragToLight + gridSamplingDisk[i] * diskRadius, light)).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
       
        
    return shadow;
}


// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}


// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


void main()
{
    // Calculate texture co-ordinate
    vec2 gScreenSize = vec2(screenWidth, screenHeight);
    vec2 TexCoord = gl_FragCoord.xy / gScreenSize;

	
    // return early for emissive fragment
//	vec3 emissive = texture(emissiveTexture, vec2(TexCoord.s, TexCoord.t)).rgb;
//	if ((emissive.x > 0) || (emissive.y > 0) || (emissive.z > 0))
//		return;

    // Get the Fragment Z position (from the depth buffer)
    float z = texture(depthTexture, vec2(TexCoord.s, TexCoord.t)).x * 2.0f - 1.0f;
    vec4 clipSpacePosition = vec4(vec2(TexCoord.s, TexCoord.t) * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;

    // Get the Fragment XYZ position (perspective division, via it's depth value)
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;
    vec3 WorldPos = worldSpacePosition.xyz;

	// is it shadow? then BAIL
	int i = lightIndex;
	float shadow = ShadowCalculation(WorldPos, lightPositions[i], i);
	if (shadow > 0.75)
		return;
	
	// reflectance equation
	vec3 Lo = vec3(0.0);

	// This if statement will discard any fragments outside walls without doors, cause no light can ever get passed that.
	//if ((WorldPos.x >= lowerXlimit) && (WorldPos.x <= upperXlimit) && (WorldPos.z >= lowerZlimit) && (WorldPos.z <= upperZlimit))
	//if (true)
	{
		// Only calculate fragments in between the floor and ceiling. THIS COULD BE POINTLESS? I'm pretty sure it is.
		//if ((WorldPos.y >= -0.01) && (WorldPos.y <= 2.41f)) 
		{
			vec3 albedo     = pow(texture(albedoTexture, TexCoord).rgb, vec3(2.2));
			float metallic  = texture(emissiveTexture, TexCoord).a;
			//float metallic  = texture(albedoTexture, TexCoord).a;
			float roughness = texture(normalTexture, TexCoord).a;

			vec3 N = normalize(texture(normalTexture, TexCoord).rgb) ;//* 2 -1  ; //getNormalFromMap();
			vec3 V = normalize(viewPos - WorldPos);    
			vec3 F0 = vec3(0.04); 
			F0 = mix(F0, albedo, metallic);

			// calculate per-light radiance
			vec3 L = normalize(lightPositions[i] - WorldPos);
			vec3 H = normalize(V + L);
			float distance = length(lightPositions[i] - WorldPos);

			if (distance < lightRadius[i])
			{
				float attenuation = 1.0 / (lightAttenuationConstant[i] + lightAttenuationLinear[i] * distance + lightAttenuationExp[i] * distance * distance);
				attenuation *= lightStrengths[i];

				vec3 radiance = lightColors[i] * attenuation;
				float NDF = DistributionGGX(N, H, roughness);   
				float G   = GeometrySmith(N, V, L, roughness);      
				vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
				vec3 nominator    = NDF * G * F; 
				float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
				vec3 specular = nominator / max(denominator, 0.001);
				vec3 kS = F;
				vec3 kD = vec3(1.0) - kS;
				kD *= 1.0 - metallic;      
				float NdotL = max(dot(N, L), 0.0);
				Lo += ((kD * albedo / PI + specular) * radiance * NdotL) * (1 - shadow);
			}
			FragColor = vec4(Lo, 1.0);
		}
	//	FragColor.rgb = texture(albedoTexture, vec2(TexCoord.s, TexCoord.t)).rgb;
	}
}


 //vec3 getshadowtc(vec3 dir, vec4 shadowparams, vec2 shadowoffset)
  //  {
  //      vec3 adir = abs(dir);
   //     float m = max(adir.x, adir.y);
   //     vec2 mparams = shadowparams.xy / max(adir.z, m);
   //     vec4 proj;
   //     if(adir.x > adir.y) proj = vec4(dir.zyx, 0.0); else proj = vec4(dir.xzy, 1.0);
    //    if(adir.z > m) proj = vec4(dir, 2.0);
    //    return vec3(proj.xy * mparams.x + vec2(proj.w, step(proj.z, 0.0)) * shadowparams.z + shadowoffset, mparams.y + shadowparams.w);
   // }