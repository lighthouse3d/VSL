/** ----------------------------------------------------------
 * \class VSResourceLib
 *
 * Lighthouse3D
 *
 * VSResourceLib - Very Simple Resource Library
 *
 * \version 0.1.4
 *		Added Materials from teapots.c
 *		https://www.sgi.com/products/software/opengl/examples/redbook/source/teapots.c
 *
 * \version 0.1.3
 *		setting the material block name is now a static function
 *
 * \version 0.1.2
 *		The library can be made independent of DevIL (without texturing capabilities though)
 *
 * \version 0.1.1
 *		Added virtual function load cubemaps
 *		Added virtual function to set a preloaded texture
 *
 * version 0.1.0
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

#ifndef __VSRL__
#define __VSRL__

#include "vslConfig.h"

#include <string>
#include <vector>
#include <map>
#include <fstream>


#ifdef __ANDROID_API__
#include <GLES3/gl3.h>
#if (__VSL_TEXTURE_LOADING__ == 1)
#include <textureLoader.h>
#endif
#else
#include <GL/glew.h>
#endif


// remove this define to skip adding the devil lib to the project
#if (__VSL_TEXTURE_LOADING__ == 1) && !defined(__ANDROID_API__)
#include <IL/il.h>
#endif



// Include other VSL
// vsMathLib is required for rendering and matrix manipulation
#include "vsMathLib.h"
// vsLogLib is required for logging errors and model info
#include "vsLogLib.h"
// VSShaderLib is required to enable and set the 
// semantic of the vertex arrays
#include "vsShaderLib.h"


class VSResourceLib {

public:

	/// helper structure for derived classes
	struct Material{

		float diffuse[4];
		float ambient[4];
		float specular[4];
		float emissive[4];
		float shininess;
		int texCount;
	};

	/// material semantics
	enum MaterialSemantics {

		DIFFUSE,
		AMBIENT,
		SPECULAR,
		EMISSIVE,
		SHININESS,
		TEX_COUNT
	} MaterialComponent;

	enum MaterialColors {
		EMERALD,
		JADE,
		OBSIDIAN,
		PEARL,
		RUBY,
		TURQUOISE,
		BRASS,
		BRONZE,
		CHROME,
		COPPER,
		GOLD,
		SILVER,
		BLACK_PLASTIC,
		CYAN_PLASTIC,
		GREEN_PLASTIC,
		RED_PLASTIC,
		WHITE_PLASTIC,
		YELLOW_PLASTIC,
		BLACK_RUBBER,
		CYAN_RUBBER,
		GREEN_RUBBER,
		RED_RUBBER,
		WHITE_RUBBER,
		YELLOW_RUBBER
	} MaterialColor;

	static float Colors[24][10]; 

	VSResourceLib();
	~VSResourceLib();

	///// load the resource
	//virtual bool load(std::string filename) = 0;

	/// render the resource
	virtual void render() = 0;

#if (__VSL_TEXTURE_LOADING__ == 1)
	/// virtual function to be implemented in derived classes
	/// assigns a texture (from an image) to a unit
	virtual void addTexture(unsigned int unit, std::string filename) {};
	virtual void addCubeMapTexture(unsigned int unit, 
									std::string posX, std::string negX, 
									std::string posY, std::string negY, 
									std::string posZ, std::string negZ) {};
	virtual void setTexture(unsigned int unit, unsigned int textureID,
							GLenum textureType = GL_TEXTURE_2D) {};

	static unsigned int loadRGBATexture(std::string filename, bool mipmap = true,
										bool compress = false,
										GLenum aFilter = GL_LINEAR, GLenum aRepMode = GL_REPEAT);
	static unsigned int loadCubeMapTexture(	std::string posX, std::string negX,
											   std::string posY, std::string negY,
											   std::string posZ, std::string negZ);
#endif
	/// const to ease future upgrades
	static const int MAX_TEXTURES = 8;
	/// Sets the shader's material block name
	static void setMaterialBlockName(std::string);


	/// set the semantics of the uniforms inside the named block
	void setUniformSemantics(MaterialSemantics field, std::string);

	/// returns errors found when loading the resource
	/// it is up to the derived classes to add meaningfull
	/// information
	virtual std::string getErrors();
	/// returns information about the loaded resource
	/// it is up to the derived classes to add meaningfull
	/// information
	virtual std::string getInfo();
	/// returns the scale factor to display the model in a unit cube
	/// note: may not apply to all subclasses
	float getScaleForUnitCube();

	static GLenum faceTarget[6];

	virtual void renderBB();

protected:

	static VSLogLib sLogError, sLogInfo;
	/// shader's material block name
	static std::string sMaterialBlockName;
	static float sIdentityMatrix[16];

	/// set the material uniforms
	void setMaterial(Material &aMat);


	std::map<std::string, MaterialSemantics> mMatSemanticMap;


	/// keep a pointer to VSMathLib singleton
	VSMathLib *mVSML;
	/// Logs for errors and model Information

	/// center of the model
	float mCenter[3];	
	/// scale factor for the model to fill a unit cube (-1, 1)
	float mScaleToUnitCube;

	void initBB();

	GLuint bbVAO;
	bool bbInit;
	float bb[2][3];
};

#endif
