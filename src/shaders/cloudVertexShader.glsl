
#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uv;

out vec3 vertexPos;
out vec2 uv_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = (projection * view * model* vec4(aPos,1.0));
	vertexPos = aPos;
	uv_out = uv;
};
