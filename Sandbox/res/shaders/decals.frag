#version 330 core
layout (location = 0) out vec4 gEmissive;   // metallic in alpha
layout (location = 1) out vec4 gNormal;     // roughness in alpha
layout (location = 2) out vec4 baseColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 model;
uniform float screenWidth;
uniform float screenHeight;
uniform vec3 targetPlaneSurfaceNormal;
uniform int writeRoughnessMetallic;

void main()
{    
    //Position on the screen
    //float2 screenPos = positionCS.xy / positionCS.w;
    
    //Convert into a texture coordinate
   // vec2 depthCoords = vec2((1 + gl_FragCoord.x) / 2 + (0.5 / screenWidth), (1 - gl_FragCoord.y) / 2 + (0.5 / screenHeight));

    //Sample a value from the depth buffer
    //vec4 sampledDepth =  texture(depthTexture, vec2(depthCoords.s, depthCoords.t));



    // Get the Fragment Z position (from the depth buffer)
    vec2 depthCoords = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
    float z = texture(depthTexture, vec2(depthCoords.s, depthCoords.t)).x * 2.0f - 1.0f;
    vec4 clipSpacePosition = vec4(vec2(depthCoords.s, depthCoords.t) * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;

    // Get the Fragment XYZ position (perspective division, via it's depth value)
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;
    vec3 WorldPos = worldSpacePosition.xyz;
	
	//vec4 objectPosition = mul(WorldPos, inverse(model));
	vec4 objectPosition = inverse(model) * vec4(WorldPos, 1.0);


    if (abs(objectPosition.x) > 0.5)
        discard;
    if (abs(objectPosition.y) > 0.5)
        discard;
   // if (abs(objectPosition.z) > 0.5)
    //    discard;

    //
    // Clipping
    //vec3 orientation = normalize(vec3(model[0].y, model[1].y, model[2].y));
    //vec3 orientation = normalize(model[1].xyz);
    //float threshold = -0.95241298041;
    vec3 normal = texture(normalTexture, vec2(depthCoords.s, depthCoords.t)).rgb;
    
   // if (dot(normal, targetPlaneSurfaceNormal)  < 0.6) 
   //    discard;//- threshold < 0) discard;

	// Add 0.5 to get texture coordinates.
	vec2 decalTexCoord = objectPosition.xy + 0.5;
    baseColor = texture(diffuseTexture, decalTexCoord);


if (writeRoughnessMetallic == 1) {
    if (texture(diffuseTexture, decalTexCoord).a > 0.5)
        gEmissive.a = 1;
     //   gNormal.a = 1 * texture(diffuseTexture, decalTexCoord).a;
    }







 //   baseColor.rgb = orientation;

  //baseColor.rgb = targetPlaneSurfaceNormal;

//  if (dot(normal, targetPlaneSurfaceNormal) < 0.5)  
//    baseColor.rgb = vec3(1);
}


	//  Reject anything outside.
	//clip(0.5 - abs(objectPosition.xyz)); YOU COMMENTED THIS OUT RECKLESSLY