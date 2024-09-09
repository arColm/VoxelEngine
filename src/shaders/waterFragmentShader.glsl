

#version 430 core

out vec4 FragColor;

in vec4 vertexColor;
in vec4 FragPosLightSpace;
in vec3 normal;
in vec3 FragPos;

//uniform vec3 objectColor;
//uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform sampler2D shadowMap;
uniform float sunHeight;
uniform vec3 sunLightDirection;

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
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;  
    if(projCoords.z > 1.0)
        shadow = 1.0;
    return shadow;
}  

void main()
{
	//FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.3);
	//FragColor = vec4(lightColor * objectColor,1.0);
    
    // AMBIENT
    vec3 ambientLight = 0.1 * vec3(1.0);

    // DIFFUSE
    float diffuseLight = max(dot(normal,-sunLightDirection),0.0);

    //SPECULAR
	vec3 lowerColor = mix(vec3(0.878,0.482,0.027),vec3(1.0),clamp(sunHeight,-0.1,1.0));
    vec3 viewDir = normalize(cameraPos-FragPos);
    vec3 reflectDir = reflect(sunLightDirection,normal);
    float specularLight = pow(max(dot(viewDir,reflectDir),0.0),8);

    // FINAL LIGHT COLOR
    vec3 light = ambientLight + ((1.0-ShadowCalculation(FragPosLightSpace)) * (diffuseLight)) * lowerColor * clamp(sunHeight*5+0.5,0.0,1.0);
    //light = ambientLight+vec3(diffuseLight);
    vec3 color = vertexColor.xyz  * light;
    //color =  light;
	//FragColor = vec4(color,vertexColor.w);
    FragColor = vec4(color,vertexColor.w);
};