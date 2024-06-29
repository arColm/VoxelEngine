

#version 430 core

out vec4 FragColor;

in vec3 vertexColor;

//uniform vec3 objectColor;
//uniform vec3 lightColor;

void main()
{
	//FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.3);
	//FragColor = vec4(lightColor * objectColor,1.0);
	FragColor = vec4(1.0);
};