/** ----------------------------------------------------------
 * \class VSResModelLib
 *
 * Lighthouse3D
 *
 * VSResModelLib - Very Simple Resource Model Library
 *
 * \version 0.4
 *		Added Materials from teapots.c
 *		https://www.sgi.com/products/software/opengl/examples/redbook/source/teapots.c
 *
 * \version 0.3
 *		Updated the lib to use Assimp 3.0
 *
 * \version 0.2.1
 *		Added the possibility of setting colors per model and per mesh
 *
 * \version 0.2.0
 *		Added cubemap textures
 *		Added the possibility to reuse textures	
 *		Loader gets tangent and bitangent attributes
 *
 * version 0.1.0
 *		Initial Release
 *
 * This lib provides an interface for Assimp to load and render 3D models
 *  and performs simple resource managment 
 *
 * This lib requires the following classes from VSL:
 * (http://www.lighthouse3d.com/very-simple-libs)
 *
 * VSResourceLib
 * VSMathLib 
 * VSLogLib
 * VSShaderLib
 *
 * and the following third party libs:
 *
 * GLEW (http://glew.sourceforge.net/),
 * Assimp (http://assimp.sourceforge.net/)
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

 
#ifndef __VSResModelLib__
#define __VSResModelLib__

#ifdef _WIN32
#pragma comment(lib,"assimp.lib")
#endif


#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <GL/glew.h>

// Assimp include files. These three are usually needed.
//#include <assimp/assimp.hpp>	
//#include <assimp/aiPostProcess.h>
//#include <assimp/aiScene.h>

#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"


#include "vsResourceLib.h"


class VSResModelLib : public VSResourceLib{

public:

	VSResModelLib();
	~VSResModelLib();

	virtual void clone(VSResourceLib *res);
	/** implementation of the superclass abstract method
	  * \param filename the model's filename
	*/
	virtual bool load(std::string filename);
	/// implementation of the superclass abstract method
	virtual void render();
	/// set a predefined material
	void setMaterialColor(MaterialColors m);
	/// set a color component for all meshes
	void setColor(VSResourceLib::MaterialSemantics m, float *values);
	/// set a color component for a particular mesh
	void setColor(unsigned int mesh, VSResourceLib::MaterialSemantics m, float *values);

#ifdef _VSL_TEXTURE_WITH_DEVIL

	/// load and set a texture for the object
	virtual void addTexture(unsigned int unit, std::string filename);
	/// load and set a cubemap texture for a object
	virtual void addCubeMapTexture(unsigned int unit, 
									std::string posX, std::string negX, 
									std::string posY, std::string negY, 
									std::string posZ, std::string negZ);
	/// set an existing texture ID for a texture unit
	virtual void setTexture(unsigned int unit, unsigned int textureID,
							GLenum textureType = GL_TEXTURE_2D);
#endif

//protected:

	// A model can be made of many meshes. Each is stored
	// in the following structure
	struct MyMesh{
		GLuint vao;
		GLuint texUnits[MAX_TEXTURES];
		GLuint texTypes[MAX_TEXTURES];
		GLuint uniformBlockIndex;
		float transform[16];
		int numIndices;
		unsigned int type;
		struct Material mat;
		unsigned int *indexes;
		float *positions;
		float *normals;
	};

	struct HalfEdge {
		GLuint vertex;
		struct HalfEdge *next;
		struct HalfEdge *twin;
	};

	/// the mesh collection
	std::vector<struct MyMesh> mMyMeshes;
	
private:
	/// aux pre processed mesh collection
	std::vector<struct MyMesh> pMyMeshesAux;

	// the global Assimp scene object
	const aiScene* pScene;

	bool pUseAdjacency;

#ifdef _VSL_TEXTURE_WITH_DEVIL

	// images / texture
	// map image filenames to textureIds
	// pointer to texture Array
	std::map<std::string, GLuint> pTextureIdMap;	

	// AUX FUNCTIONS
	bool loadTextures(const aiScene *scene, std::string prefix);
#endif
	void genVAOsAndUniformBuffer(const aiScene *sc);
	void recursive_walk_for_matrices(const aiScene *sc, 
						const aiNode* nd);

	void get_bounding_box_for_node (const struct aiNode* nd, 
						aiVector3D* min, 
						aiVector3D* max);

	void color4_to_float4(const aiColor4D *c, float f[4]);
	void set_float4(float f[4], float a, float b, float c, float d);

};

#endif
