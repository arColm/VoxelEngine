

#version 430 core

in vec3 vertexPos;
in vec2 uv_out;

out vec4 FragColor;

uniform float time;

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

float distanceToCenterSquared(vec2 pos) {
    return 4*dot((uv_out-vec2(0.5,0.5)) * (uv_out-vec2(0.5,0.5)),vec2(1,1));
}

float layeredNoise(vec2 p, int octaves) {
    float n = 0;
    float amplitude = 1.0;
    float persistence = 1.0;
    float maxAmplitude = 0.0;
    
    float distanceToCenterSq = distanceToCenterSquared(p)+0.1;

    for(int i=0;i<octaves;i++) {
        maxAmplitude += amplitude;
        n+= noise(p*persistence) * amplitude;
        //amplitude *= 0.3;
        amplitude *= distanceToCenterSq;
        persistence *= 2;
    }
    return n /maxAmplitude;
}

void main() {
    float n = layeredNoise(uv_out*16 +vec2(time),5);
    float opacity = 4*(0.25-dot((uv_out-vec2(0.5,0.5)) * (uv_out-vec2(0.5,0.5)),vec2(1,1)));
	FragColor = vec4(n) * vec4(1,1,1,opacity);
}