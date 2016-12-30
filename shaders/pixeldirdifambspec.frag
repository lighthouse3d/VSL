#version 330

layout (std140) uniform Material {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

layout (std140) uniform Lights {
	vec3 l_dir;	   // camera space
};

uniform	sampler2D texUnit;

in Data {
	vec3 normal;
	vec2 texCoord;
	vec4 eye;
} DataIn;

out vec4 colorOut;

void main() {

	// set the specular term to black
	vec4 spec = vec4(0.0);

	// normalize both input vectors
	vec3 n = normalize(DataIn.normal);
	vec3 e = normalize(vec3(DataIn.eye));

	float intensity = max(dot(n,l_dir), 0.0);

	// if the vertex is lit compute the specular color
	if (intensity > 0.0) {
		// compute the half vector
		vec3 h = normalize(l_dir + e);	
		// compute the specular term into spec
		float intSpec = max(dot(h,n), 0.0);
		spec = specular * pow(intSpec,shininess);
	}
	vec4 diff = diffuse;
	if (texCount != 0)
		diff *= texture(texUnit, DataIn.texCoord) ;

	colorOut = max(intensity *  diff  + spec, diff*0.25) + emissive;
}