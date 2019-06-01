#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in ivec4 aBoneID;
layout (location = 6) in vec4 aBoneWeight;
layout (location = 7) in mat4 instancedModelMatrix;

uniform mat4 model;
uniform bool animated;
uniform bool instanced;
uniform mat4 jointTransforms[50];	// hardcoded maximum bones 50

void main()
{
vec4 worldPos;
	
	// Instancing?
	mat4 modelMatrix = model;
	if (instanced)
		modelMatrix = instancedModelMatrix;

	gl_Position = modelMatrix * vec4(aPos, 1.0);
	
	
	// Animated
	if (animated)
	{
		vec4 totalLocalPos = vec4(0.0);
		vec4 totalNormal = vec4(0.0);
		mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

		for(int i=0;i<4;i++) 
		{
			mat4 jointTransform = jointTransforms[aBoneID[i]];
			vec4 posePosition = jointTransform * vec4(aPos, 1.0);
			totalLocalPos += posePosition * aBoneWeight[i];		
		}
		gl_Position = modelMatrix * totalLocalPos;
	}
}