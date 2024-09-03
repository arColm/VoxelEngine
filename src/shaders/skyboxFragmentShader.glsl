

#version 430 core

in vec3 vertexPos;
in vec3 vertexColor;

out vec4 FragColor;

uniform float sunHeight;

void main() {
	FragColor = vec4(vertexColor,1.0);
}