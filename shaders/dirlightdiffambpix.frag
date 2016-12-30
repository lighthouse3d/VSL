#version 330

layout (std140) uniform Material {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

layout (std140) uniform Matrices {
	mat4 projViewModelMatrix;
	mat4 viewMatrix;
	mat3 normalMatrix;
};

uniform	sampler2D texUnit;
uniform vec3 lightDir;

in vec3 Normal;
in vec2 TexCoord;

out vec4 outputF;

void main()
{
	vec4 color, emission,amb;
	float intensity;
	vec3 ld, n;
	
	ld = normalize(vec3(viewMatrix * vec4(lightDir, 0.0)));
	n = normalize(Normal);	
	intensity = max(dot(ld,n),0.0);
	
	if (texCount == 0) {
		color = diffuse;
		amb = ambient;
		emission = emissive;
	}
	else {
		color = texture(texUnit, TexCoord) * (diffuse);
		amb = color * ambient;
		emission = texture(texUnit, TexCoord) * emissive;
	}
	outputF = (color * intensity) + amb + emission;
}


