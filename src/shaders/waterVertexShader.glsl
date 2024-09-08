
#version 430 core

const float fogDistanceSquared = 60000.0;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;

//out vec2 TexCoord;
out vec4 vertexColor;
out vec4 FragPosLightSpace;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform float sunHeight;
uniform float time;

uniform vec3 cameraPos;
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
// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}


float layeredNoise(vec3 p, int octaves) {
    float n = 0;
    float amplitude = 1.0;
    float persistence = 1.0;
    float maxAmplitude = 0.0;
    for(int i=0;i<octaves;i++) {
        maxAmplitude += amplitude;
        n+= noise(p*persistence) * amplitude;
        amplitude *= 0.3;
        persistence *= 2;
    }
    return n /maxAmplitude;
}

float layeredNoise(vec2 p, int octaves) {
    float n = 0;
    float amplitude = 1.0;
    float persistence = 1.0;
    float maxAmplitude = 0.0;

    for(int i=0;i<octaves;i++) {
        maxAmplitude += amplitude;
        n+= noise(p*persistence) * amplitude;
        //amplitude *= 0.3;
        amplitude *= 0.2;
        persistence *= 2;
    }
    return n /maxAmplitude;
}
vec3 fogColor(vec3 position) {
	vec2 distanceVector = aPos.xz - cameraPos.xz;
	vec3 color = vec3(clamp(sunHeight * 2.5,-1.0,1.0));
	return  vec3(dot(distanceVector,distanceVector)/fogDistanceSquared) * color;
}

void main()
{
	//vec3 wave = aNormal.y>=1 ?
    //    clamp( ((sin(aPos.x+aPos.z + time)-0.2)*aNormal),vec3(0,-layeredNoise(aPos.xz +vec2(time),5),0),vec3(0,0,0))
    //    : vec3(0);
    float wave = aNormal.y>=1 ?
        -layeredNoise(aPos.xz +vec2(time),5)
        : 0;
	vec3 pos = aPos + vec3(0,wave,0);
	gl_Position = projection * view * vec4(pos,1.0);
	vertexColor = aColor + vec4(fogColor(pos),0.0);
    FragPosLightSpace = lightSpaceMatrix * vec4(pos, 1.0);
	normal = vec3(wave,1-2*wave,-wave);

	//vertexColor = vec4(aPos, 1.0);
	//TexCoord = aTexCoord;
};



