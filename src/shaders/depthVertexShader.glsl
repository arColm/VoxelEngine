
#version 430 core

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;

//out vec2 TexCoord;

//uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
	gl_Position = lightSpaceMatrix * vec4(aPos,1.0);
};
