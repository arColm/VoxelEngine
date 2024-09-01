
#version 430 core

const float fogDistanceSquared = 60000.0;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
//layout (location = 1) in vec2 aTexCoord;

//out vec2 TexCoord;
out vec4 vertexColor;
out vec4 FragPosLightSpace;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform float sunHeight;

uniform vec3 cameraPos;

vec3 fogColor(vec3 position) {
	vec2 distanceVector = aPos.xz - cameraPos.xz;
	vec3 color = vec3(clamp(sunHeight * 2.5,-1.0,1.0));
	return  vec3(dot(distanceVector,distanceVector)/fogDistanceSquared) * color;
}

void main()
{
	gl_Position = projection * view * vec4(aPos,1.0);
	vertexColor = aColor + vec4(fogColor(aPos),0.0);
    FragPosLightSpace = lightSpaceMatrix * vec4(aPos, 1.0);
	normal = aNormal;

	//vertexColor = vec4(aPos, 1.0);
	//TexCoord = aTexCoord;
};



