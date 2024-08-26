
#version 430 core

const float fogDistanceSquared = 60000.0;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
//layout (location = 1) in vec2 aTexCoord;

//out vec2 TexCoord;
out vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPos;

vec3 fogColor(vec3 position) {
	vec2 distanceVector = aPos.xz - cameraPos.xz;
	return  vec3(dot(distanceVector,distanceVector)/fogDistanceSquared);
}

void main()
{
	gl_Position = projection * view * vec4(aPos,1.0);
	vertexColor = aColor + vec4(fogColor(aPos),0.0);

	//vertexColor = vec4(aPos, 1.0);
	//TexCoord = aTexCoord;
};

