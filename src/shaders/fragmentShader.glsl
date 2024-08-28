

#version 430 core

out vec4 FragColor;

in vec4 vertexColor;
in vec4 FragPosLightSpace;

//uniform vec3 objectColor;
//uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth  ? 0.7 : 0.0;  
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}  

void main()
{
	//FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.3);
	//FragColor = vec4(lightColor * objectColor,1.0);
    vec3 color = vertexColor.xyz * (1.0-ShadowCalculation(FragPosLightSpace));
	FragColor = vec4(color,vertexColor.w);
};