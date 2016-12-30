/** ----------------------------------------------------------
 * \class VSResourceLib
 *
 * Lighthouse3D
 *
 * VSResourceLib - Very Simple Resource Library
 *
 * \version 0.1.1
 *		Added virtual function load cubemaps
 *		Added virtual function to set a preloaded texture
 *
 * \version 0.1.0
 *		Initial Release
 *
 * This abstract class defines an interface 
 * for loading and rendering resources (models)
 *
* This lib requires the following classes from VSL:
 * (http://www.lighthouse3d.com/very-simple-libs)
 *
 * VSMathLib 
 * VSLogLib
 * VSShaderLib
 *
 * and the following third party libs:
 *
 * GLEW (http://glew.sourceforge.net/),
 * DevIL (http://openil.sourceforge.net/) (optional)
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#include "vsResourceLib.h"

float VSResourceLib::Colors[24][10] = 
	{{0.0215f ,0.1745f ,0.0215f ,0.07568f ,0.61424f ,0.07568f ,0.633f ,0.727811f, 0.633f, 76.8f} ,
	{0.135f ,0.2225f ,0.1575f ,0.54f ,0.89f ,0.63f ,0.316228f ,0.316228f ,0.316228f , 12.8f} ,
	{0.05375f ,0.05f ,0.06625f ,0.18275f ,0.17f ,0.22525f ,0.332741f ,0.328634f ,0.346435f , 38.4f} ,
	{0.25f ,0.20725f ,0.20725f ,1.0f ,0.829f ,0.829f ,0.296648f ,0.296648f ,0.296648f , 10.2f} ,
	{0.1745f ,0.01175f ,0.01175f ,0.61424f ,0.04136f ,0.04136f ,0.727811f ,0.626959f ,0.626959f , 76.8f} ,
	{0.1f ,0.18725f ,0.1745f ,0.396f ,0.74151f ,0.69102f ,0.297254f ,0.30829f ,0.306678f , 12.8f} ,
	{0.329412f ,0.223529f ,0.027451f ,0.780392f ,0.568627f ,0.113725f ,0.992157f ,0.941176f ,0.807843f ,27.9f} ,
	{0.2125f ,0.1275f ,0.054f ,0.714f ,0.4284f ,0.18144f ,0.393548f ,0.271906f ,0.166721f , 25.6f} ,
	{0.25f ,0.25f ,0.25f ,0.4f ,0.4f ,0.4f ,0.774597f ,0.774597f ,0.774597f , 76.8f} ,
	{0.19125f ,0.0735f ,0.0225f ,0.7038f ,0.27048f ,0.0828f ,0.256777f ,0.137622f ,0.086014f , 12.8f} ,
	{0.24725f ,0.1995f ,0.0745f ,0.75164f ,0.60648f ,0.22648f ,0.628281f ,0.555802f ,0.366065f , 51.2f} ,
	{0.19225f ,0.19225f ,0.19225f ,0.50754f ,0.50754f ,0.50754f ,0.508273f ,0.508273f ,0.508273f , 51.2f} ,
	{0.0f ,0.0f ,0.0f ,0.01f ,0.01f ,0.01f ,0.50f ,0.50f ,0.50f , 32.0f} ,
	{0.0f ,0.1f ,0.06f ,0.0f ,0.50980392f ,0.50980392f ,0.50196078f ,0.50196078f ,0.50196078f , 32.0f} ,
	{0.0f ,0.0f ,0.0f ,0.1f ,0.35f ,0.1f ,0.45f ,0.55f ,0.45f ,		32.0f} ,
	{0.0f ,0.0f ,0.0f ,0.5f ,0.0f ,0.0f ,0.7f ,0.6f ,0.6f ,			32.0f} ,
	{0.0f ,0.0f ,0.0f ,0.55f ,0.55f ,0.55f ,0.70f ,0.70f ,0.70f ,	32.0f} ,
	{0.0f ,0.0f ,0.0f ,0.5f ,0.5f ,0.0f ,0.60f ,0.60f ,0.50f ,		32.0f} ,           
	{0.02f ,0.02f ,0.02f ,0.01f ,0.01f ,0.01f ,0.4f ,0.4f ,0.4f ,	10.0f} ,
	{0.0f ,0.05f ,0.05f ,0.4f ,0.5f ,0.5f ,0.04f ,0.7f ,0.7f ,		10.0f} ,
	{0.0f ,0.05f ,0.0f ,0.4f ,0.5f ,0.4f ,0.04f ,0.7f ,0.04f ,		10.0f} ,
	{0.05f ,0.0f ,0.0f ,0.5f ,0.4f ,0.4f ,0.7f ,0.04f ,0.04f ,		10.0f} ,
	{0.05f ,0.05f ,0.05f ,0.5f ,0.5f ,0.5f ,0.7f ,0.7f ,0.7f ,		10.0f} ,
	{0.05f ,0.05f ,0.0f ,0.5f ,0.5f ,0.4f ,0.7f ,0.7f ,0.04f ,		10.0f}};	

float VSResourceLib::sIdentityMatrix[16] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f };

VSLogLib VSResourceLib::sLogError, VSResourceLib::sLogInfo;
std::string VSResourceLib::sMaterialBlockName = "";


GLenum VSResourceLib::faceTarget[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};


VSResourceLib::VSResourceLib(): mScaleToUnitCube(1.0), bbVAO(0), bbInit(false)
{
	// get a pointer to VSMathLib singleton
	mVSML = VSMathLib::getInstance();

	/* initialization of DevIL */
#if (__VSL_TEXTURE_LOADING__ == 1) && !defined(__ANDROID_API__)
	ilInit(); 
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
#endif
}


VSResourceLib::~VSResourceLib() {

}


void
VSResourceLib::initBB() {

	// expand bounding box
	for(int i = 0; i < 3; ++i) {
		bb[1][i] += 0.01f;
		bb[0][i] -= 0.01f;
	}

	float vertices[8][4];
	GLuint ind[36];
	//left, bottom, far
	vertices[0][0] = bb[0][0]; vertices[0][1] = bb[0][1]; vertices[0][2] = bb[0][2]; vertices[0][3] = 1.0;
	// right, bottom, far
	vertices[1][0] = bb[1][0]; vertices[1][1] = bb[0][1]; vertices[1][2] = bb[0][2]; vertices[1][3] = 1.0;
	// right, top, far
	vertices[2][0] = bb[1][0]; vertices[2][1] = bb[1][1]; vertices[2][2] = bb[0][2]; vertices[2][3] = 1.0;
	// left, top, far
	vertices[3][0] = bb[0][0]; vertices[3][1] = bb[1][1]; vertices[3][2] = bb[0][2]; vertices[3][3] = 1.0;

	//left, bottom, near
	vertices[4][0] = bb[0][0]; vertices[4][1] = bb[0][1]; vertices[4][2] = bb[1][2]; vertices[4][3] = 1.0;
	// right, bottom, near
	vertices[5][0] = bb[1][0]; vertices[5][1] = bb[0][1]; vertices[5][2] = bb[1][2]; vertices[5][3] = 1.0;
	// right, top, near
	vertices[6][0] = bb[1][0]; vertices[6][1] = bb[1][1]; vertices[6][2] = bb[1][2]; vertices[6][3] = 1.0;
	// left, top, near
	vertices[7][0] = bb[0][0]; vertices[7][1] = bb[1][1]; vertices[7][2] = bb[1][2]; vertices[7][3] = 1.0;

	// front
	ind[0] = 4; ind[1] = 5; ind[2] = 7;
	ind[3] = 7; ind[4] = 5; ind[5] = 6;
	// right
	ind[6] = 1; ind[7] = 6; ind[8] = 5;
	ind[9] = 1; ind[10]= 2; ind[11]= 6;
	// top
	ind[12]= 6; ind[13]= 2; ind[14]= 7;
	ind[15]= 7; ind[16]= 2; ind[17]= 3;
	//left
	ind[18]= 4; ind[19]= 7; ind[20]= 0;
	ind[21]= 0; ind[22]= 7; ind[23]= 3;
	//bottom
	ind[24]= 0; ind[25]= 1; ind[26]= 4;
	ind[27]= 1; ind[28]= 5; ind[29]= 4;
	//back
	ind[30]= 0; ind[31]= 2; ind[32]= 1;
	ind[33]= 0; ind[34]= 3; ind[35]= 2;

	GLuint vb, ib;
	glGenVertexArrays(1, &bbVAO);
	glBindVertexArray(bbVAO);

	glGenBuffers(1,&vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 8, (void *)vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

	glGenBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, (void *)ind, GL_STATIC_DRAW);
}


void
VSResourceLib::renderBB() {

	if (!bbInit)
		return;


	if (!bbVAO) 
		initBB();

	mVSML->matricesToGL();
	glBindVertexArray(bbVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}



// get the scale factor used to fit the model in a unit cube
float
VSResourceLib::getScaleForUnitCube() {

	return mScaleToUnitCube;
}


// Static function to set the shader's material block name
void 
VSResourceLib::setMaterialBlockName(std::string name) {

	sMaterialBlockName = name;
}


// useful to set uniforms inside a named block
void
VSResourceLib::setMaterial(Material &aMat) {

	// use named block
	if (sMaterialBlockName != "" && mMatSemanticMap.size() == 0) {
		VSShaderLib::setBlock(sMaterialBlockName, &aMat);
	}
	// use uniforms in named block
	else if (sMaterialBlockName != "" && mMatSemanticMap.size() != 0) {

		std::map<std::string, MaterialSemantics>::iterator iter;
		for (iter = mMatSemanticMap.begin(); iter != mMatSemanticMap.end(); ++iter) {
			void *value;
			switch ((*iter).second) {
				case DIFFUSE: value = (void *)aMat.diffuse;
					break;
				case AMBIENT: value = (void *)aMat.ambient;
					break;
				case SPECULAR: value = (void *)aMat.specular;
					break;
				case EMISSIVE: value = (void *)aMat.emissive;
					break;
				case SHININESS: value = (void *)&aMat.shininess;
					break;
				case TEX_COUNT: value = (void *)&aMat.texCount;
					break;
			}
			VSShaderLib::setBlockUniform(sMaterialBlockName, 
						(*iter).first, value);
		}
	}
}


void 
VSResourceLib::setUniformSemantics(MaterialSemantics field, std::string name) {

	mMatSemanticMap[name] = field;
}


// Get loading errors
std::string 
VSResourceLib::getErrors() {

	return(sLogError.dumpToString());
}


// get model information
std::string
VSResourceLib::getInfo() {

	return(sLogInfo.dumpToString());
}

#if (__VSL_TEXTURE_LOADING__ == 1)

// helper function for derived classes
// loads an image and defines an 8-bit RGBA texture
unsigned int
VSResourceLib::loadRGBATexture(std::string filename, 
						bool mipmap, bool compress, 
						GLenum aFilter, GLenum aRepMode) {

#ifdef __ANDROID_API__
	return (unsigned int)LoadTexture(filename);
#else

	ILboolean success;
	unsigned int imageID;
	GLuint textureID = 0;

	// Load Texture Map
	ilGenImages(1, &imageID); 
	
	ilBindImage(imageID); /* Binding of DevIL image name */
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT); 
	success = ilLoadImage((ILstring)filename.c_str());

	if (!success) {
		VSLOG(sLogError, "Couldn't load texture: %s", 
							filename.c_str());
		// The operation was not sucessfull 
		// hence free image and texture 
		ilDeleteImages(1, &imageID); 
		return 0;
	}

	// add information to the log

	VSLOG(sLogInfo, "Texture Loaded: %s", filename.c_str());
	printf("Width: %d, Height %d, Bytes per Pixel %d", 
				ilGetInteger(IL_IMAGE_WIDTH),
				ilGetInteger(IL_IMAGE_HEIGHT),
				ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL));

	std::string s;
	switch(ilGetInteger(IL_IMAGE_FORMAT)) {
		case IL_COLOR_INDEX      : s =  "IL_COLOR_INDEX"; break;     
		case IL_ALPHA		     : s =  "IL_ALPHA"; break;	
		case IL_RGB              : s =  "IL_RGB"; break;     
		case IL_RGBA             : s =  "IL_RGBA"; break;     
		case IL_BGR              : s =  "IL_BGR"; break;     
		case IL_BGRA             : s =  "IL_BGRA"; break;     
		case IL_LUMINANCE        : s =  "IL_LUMINANCE"; break;     
		case  IL_LUMINANCE_ALPHA : s =  "IL_LUMINANCE_ALPHA"; break;
	}
	printf(" Format %s", s.c_str());
	
	switch(ilGetInteger(IL_IMAGE_TYPE)) {
		case IL_BYTE           : s =  "IL_BYTE"; break;     
		case IL_UNSIGNED_BYTE  : s =  "IL_UNSIGNED_BYTE"; break;	
		case IL_SHORT          : s =  "IL_SHORT"; break;     
		case IL_UNSIGNED_SHORT : s =  "IL_UNSIGNED_SHORT"; break;     
		case IL_INT            : s =  "IL_INT"; break;     
		case IL_UNSIGNED_INT   : s =  "IL_UNSIGNED_INT"; break;     
		case IL_FLOAT          : s =  "IL_FLOAT"; break;     
		case IL_DOUBLE         : s =  "IL_DOUBLE"; break;
		case IL_HALF           : s =  "IL_HALF"; break;
	}
	printf(" Data type:  %s\n", s.c_str());

	/* Convert image to RGBA */
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 
	// Set filters
	GLenum minFilter = aFilter;
	if (aFilter == GL_LINEAR && mipmap) {
		minFilter = GL_LINEAR_MIPMAP_LINEAR;
	}
	else if (aFilter == GL_NEAREST && mipmap){
		minFilter = GL_NEAREST_MIPMAP_LINEAR;
	}
	GLenum type;
	if (compress)
		type = GL_RGBA;
	else
		type = GL_COMPRESSED_RGBA;


	/* Create and load textures to OpenGL */
	glGenTextures(1, &textureID); /* Texture name generation */
	glBindTexture(GL_TEXTURE_2D, textureID); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilter); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, aRepMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, aRepMode);
	glTexImage2D(GL_TEXTURE_2D, 0, type, 
					ilGetInteger(IL_IMAGE_WIDTH),
					ilGetInteger(IL_IMAGE_HEIGHT), 
					0, GL_RGBA, GL_UNSIGNED_BYTE,
					ilGetData()); 

	// Mipmapping?
	if (mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,0);

	/* Because we have already copied image data into texture data
	we can release memory used by image. */
	ilDeleteImages(1, &imageID); 


	return textureID;
#endif
}


// helper function for derived classes
// loads an image and defines an 8-bit RGBA texture
unsigned int
VSResourceLib::loadCubeMapTexture(	std::string posX, std::string negX, 
									std::string posY, std::string negY, 
									std::string posZ, std::string negZ) {

#ifdef __ANDROID_API__
	return (unsigned int)LoadCubeMapTexture(posX, negX, posY, negY, posZ, negZ);
#else
	ILboolean success;
	unsigned int imageID;
	GLuint textureID = 0;

	std::string files[6];

	files[0] = posX;
	files[1] = negX;
	files[2] = posY;
	files[3] = negY;
	files[4] = posZ;
	files[5] = negZ;

	glGenTextures(1, &textureID); /* Texture name generation */
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); 

	// Load Textures for Cube Map

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,  GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,  GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,  GL_CLAMP_TO_EDGE);

	ilGenImages(1, &imageID); 
	ilBindImage(imageID); /* Binding of DevIL image name */

	for (int i = 0; i < 6; ++i) {
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT); 
		success = ilLoadImage((ILstring)files[i].c_str());

		if (!success) {
			VSLOG(sLogError, "Couldn't load texture: %s", 
								files[i].c_str());
			// The operation was not sucessfull 
			// hence free image and texture 
			ilDeleteImages(1, &imageID); 
			return 0;
		}
		
		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 

		/* Create and load textures to OpenGL */
		glTexImage2D(faceTarget[i], 0, GL_RGBA, 
						ilGetInteger(IL_IMAGE_WIDTH),
						ilGetInteger(IL_IMAGE_HEIGHT), 
						0, GL_RGBA, GL_UNSIGNED_BYTE,
						ilGetData()); 

		VSLOG(sLogInfo, "Texture Loaded: %s", files[i].c_str());
	}

	VSLOG(sLogInfo, "Cube Map Loaded Successfully");

	glBindTexture(GL_TEXTURE_CUBE_MAP,0);

	/* Because we have already copied image data into texture data
	we can release memory used by image. */
	ilDeleteImages(1, &imageID); 

	// add information to the log

	return textureID;

#endif
}

#endif