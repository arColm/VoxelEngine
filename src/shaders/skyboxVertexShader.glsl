
#version 430 core

layout (location = 0) in vec3 aPos;

out vec3 vertexColor;
out vec3 vertexPos;

uniform vec3 cameraPos;
uniform mat4 view;
uniform mat4 projection;

uniform float sunHeight;


float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

vec3 usmoothstep(vec3 edge0, vec3 edge1, vec3 x) {
	vec3 t = (x - edge0) / (edge1 - edge0);
    return t * t * (3.0 - 2.0 * t);
}

float usmoothstep(float edge0, float edge1, float x) {
	float t = (x - edge0) / (edge1 - edge0);
    return t * t * (3.0 - 2.0 * t);
}

vec3 starColor() {
	float n = noise(aPos);

    //n = pow(n,10);
    return vec3(n);
}

void main() {
	
	gl_Position = (projection * view * vec4(5*aPos+cameraPos,1.0)).xyww;
    vertexPos = 5*normalize(aPos);

}