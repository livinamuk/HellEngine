#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D blur0;
uniform sampler2D blur1;
uniform sampler2D blur2;
uniform sampler2D blur3;
uniform bool bloom;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb; 
	
	vec3 blur = texture(blur0, TexCoords).rgb;
	blur += texture(blur1, TexCoords).rgb;
	blur += texture(blur2, TexCoords).rgb;
	blur += texture(blur3, TexCoords).rgb;

    vec3 bloomColor = blur;
    if(bloom)
        hdrColor += bloomColor; // additive blending

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}