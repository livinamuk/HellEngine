// vertex shader
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

//out vec2 TexCoords;
uniform mat4 gWVP;

void main()
{
	gl_Position = gWVP * vec4(position, 1.0);
}
 