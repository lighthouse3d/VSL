#version 330

layout (std140) uniform Matrices {
	mat4 projViewModelMatrix;
	mat4 viewMatrix;
	mat3 normalMatrix;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 TexCoord;
out vec3 Normal;

void main()
{
	Normal = normalize(normalMatrix * normal);	
	TexCoord = vec2(texCoord);
	gl_Position = projViewModelMatrix * vec4(position,1.0);

}
