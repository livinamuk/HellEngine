#version 400 core
//out vec4 FragColor;

layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
//uniform samplerCube depthMap;
uniform samplerCubeArray depthMap;

uniform vec3 lightPositions[2];
uniform vec3 viewPos;

uniform float far_plane;


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
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 50.0;
    for(int i = 0; i < samples; ++i)
    {
        //float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        float closestDepth = texture(depthMap, vec4(fragToLight + gridSamplingDisk[i] * diskRadius, light)).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
       
        
    return shadow;
}

void main()
{           
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);
	vec3 fragPos = fs_in.FragPos;
	
	vec3 lighting = vec3(0); // accumulative
	vec3 ambient = 0.2 * color;
	vec3 lightColor = vec3(0.3);
	

	for (int i = 0; i<2; i++)
	{
		vec3 lightPos = lightPositions[i];
	    vec3 lightDir = normalize(lightPos - fragPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = diff * lightColor;
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = 0.0;
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
		vec3 specular = spec * lightColor;   
	
		// calculate shadow
		float shadow = ShadowCalculation(fs_in.FragPos, lightPos, i);                      
		lighting += (ambient + (1.0 - shadow) * (diffuse + specular));    
	}
    FragColor = vec4(lighting * color, 1.0);
}