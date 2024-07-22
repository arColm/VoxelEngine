
#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
//layout (location = 1) in vec2 aTexCoord;

//out vec2 TexCoord;
out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vertexColor = aColor;
	gl_Position = projection * view * model * vec4(aPos,1.0);
	//TexCoord = aTexCoord;
};

uint lowbias32(uint x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}