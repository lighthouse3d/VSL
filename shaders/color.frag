#version 330

layout (std140) uniform Material {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform sampler2D texUnit;

in Data {
	vec4 texCoord;
} DataIn;

out vec4 outputF;

void main()
{
	if (texCount > 0)
		outputF = emissive * texture(texUnit, DataIn.texCoord.xy);
	else	
		outputF = emissive;
} 
