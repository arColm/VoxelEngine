
#version 430 core

layout (location = 0) in vec3 aPos;

out vec3 vertexColor;

uniform vec3 cameraPos;
uniform mat4 view;
uniform mat4 projection;

uniform float sunHeight;

vec3 usmoothstep(vec3 edge0, vec3 edge1, vec3 x) {
	vec3 t = (x - edge0) / (edge1 - edge0);
    return t * t * (3.0 - 2.0 * t);
}

float usmoothstep(float edge0, float edge1, float x) {
	float t = (x - edge0) / (edge1 - edge0);
    return t * t * (3.0 - 2.0 * t);
}
void main() {
	float height = normalize(aPos).y;
	vec3 normalizedPos = normalize(aPos);
	vec3 lowerColor = mix(vec3(0.878,0.482,0.027),vec3(2.0),clamp(sunHeight,-0.1,1.0));
	vec3 mainColor = mix(vec3(0.067,0.0588,0.211),vec3(0.196,0.8,0.914),vec3(clamp(sunHeight*2,-0.2,1.0)));

	vec3 aggregateLowerColor = mix(mainColor,lowerColor,clamp(sunHeight+0.3,0.0,1.0));
	
	gl_Position = (projection * view * vec4(aPos+cameraPos,1.0)).xyww;
	vertexColor = mix(aggregateLowerColor,mainColor,vec3(clamp(aPos.y*3,0.0,1.0)));
}