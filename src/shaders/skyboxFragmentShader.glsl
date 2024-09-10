

#version 430 core

in vec3 vertexPos;

out vec4 FragColor;

uniform float sunHeight;
uniform vec3 fogColor;

// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}
float hash(float p) { p = fract(p * 0.011); p *= p + 7.5; p *= p + p; return fract(p); }
//float hash(vec2 p) {vec3 p3 = fract(vec3(p.xyx) * 0.13); p3 += dot(p3, p3.yzx + 3.333); return fract((p3.x + p3.y) * p3.z); }

// 3D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);
 
    // For performance, compute the base input to a 1D hash from the integer part of the argument and the 
    // incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

vec3 starColor() {
	float n = noise(vertexPos*10);
    //n = n*2;
    n = pow(n,50);
    return vec3(n);
}

void main() {
	vec3 lowerColor = fogColor;
	vec3 mainColor = mix(vec3(0.067,0.0588,0.211),vec3(0.196,0.8,0.914),vec3(clamp(sunHeight*2,-0.2,1.0)));
	

    mainColor += mix(starColor(),vec3(0),vec3(clamp(sunHeight,-1,0)+1));

	//vec3 aggregateLowerColor = mix(mainColor,lowerColor,clamp(sunHeight+0.4,0.0,1.0));
    // ABOVE IMPLEMENTATION BEFORE FOG
    vec3 aggregateLowerColor = lowerColor;
	
	FragColor = vec4(mix(aggregateLowerColor,mainColor,vec3(clamp(vertexPos.y*3,0.0,1.0))),1.0);
    //FragColor = vec4(starColor(),1.0);
}