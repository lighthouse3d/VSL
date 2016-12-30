/** ----------------------------------------------------------
 * \class VSModelLib
 *
 * Lighthouse3D
 *
 * VSModelLib - Very Simple Resource Model Library
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

#include "vslConfig.h"

#include <string>
#include <vector>
#include <map>
#include <fstream>

#ifdef __ANDROID_API__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#if (__VSL_MODEL_LOADING__ == 1)
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#endif

#include "vsResourceLib.h"


class VSModelLib : public VSResourceLib{

public:

	enum {
		NORMAL = 1,
		TANGENT = 2,
		BITANGENT = 4,
		TEXCOORD = 8
	} Mode;

	VSModelLib();
	~VSModelLib();

#if defined(__ANDROID_API__) && (__VSL_MODEL_LOADING__ == 1)
	static Assimp::Importer *s_Importer;
	static void SetImporter(Assimp::Importer *imp);
#endif

	/** defines what buffers to generate
	* \param mode Bitwise OR of masks that indicate the buffers to be generated (see enum Mode)
	*/
	void setGenerationMode(int mode);

	/** get the current set of buffers to generate (see enum Mode)
	*/
	int getGenerationMode();

#if (__VSL_MODEL_LOADING__ == 1)
	virtual bool load(std::string filename);
#endif

	/// implementation of the superclass abstract method
	virtual void render();
	/// set a predefined material
	void setMaterialColor(MaterialColors m);
	/// set a color component for all meshes
	void setColor(VSResourceLib::MaterialSemantics m, float r, float g, float b, float a);
	/// set a color component for all meshes
	void setColor(VSResourceLib::MaterialSemantics m, float *values);
	/// set a color component for a particular mesh
	void setColor(unsigned int mesh, VSResourceLib::MaterialSemantics m, float *values);

#if (__VSL_TEXTURE_LOADING__ == 1)

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

public:

	// A model can be made of many meshes. Each is stored
	// in the following structure
	class MyMesh {

	public:
		GLuint vao, vboPos, vboNormal, vboTexCoord, vboTangent, vboBitangent, vboIndices;
		GLuint texUnits[MAX_TEXTURES];
		GLuint texTypes[MAX_TEXTURES];
		GLuint uniformBlockIndex;
		float transform[16];
		int numIndices;
		bool hasIndices;
		unsigned int type;
		struct Material mat;

		MyMesh() {
			vao = 0; vboPos = 0; vboNormal = 0; vboTexCoord = 0; vboTangent = 0; vboBitangent = 0; vboIndices = 0;
			numIndices = 0;
			hasIndices = false;
			type = GL_TRIANGLES;
			float cD[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			memcpy(mat.diffuse, cD, sizeof(float) * 4);
			float cA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			memcpy(mat.ambient, cA, sizeof(float) * 4);
			float cS[4] = { 0.0f, 0.0f, 0.0f ,1.0f };
			memcpy(mat.specular, cS, sizeof(float) * 4);
			float cE[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			memcpy(mat.emissive, cE, sizeof(float) * 4);

			mat.shininess = 128.0;
			mat.texCount = 0;
		}

	};

	struct HalfEdge {
		GLuint vertex;
		struct HalfEdge *next;
		struct HalfEdge *twin;
	};

	/// the mesh collection
	std::vector<MyMesh> mMyMeshes;

	void addMeshes(const VSModelLib &model);

protected:
	void buildVAO(MyMesh &m, size_t nump, float *p, float *n, float *tc, float *tang, float *bitan, size_t numInd, unsigned int *indices);
	int mFlagMode;


private:
	/// aux pre processed mesh collection
	std::vector<MyMesh> mMyMeshesAux;

#if (__VSL_MODEL_LOADING__ == 1)
	// the global Assimp scene object
	const aiScene* mScene;
#endif

	bool pUseAdjacency;

#if (__VSL_TEXTURE_LOADING__ == 1)

	// images / texture
	// map image filenames to textureIds
	// pointer to texture Array
	std::map<std::string, GLuint> mTextureIdMap;

	// Load models from file
	bool loadTextures(const aiScene *scene, std::string prefix);
#endif

#if (__VSL_MODEL_LOADING__ == 1)
	void genVAOsAndUniformBuffer(const aiScene *sc);
	void recursive_walk_for_matrices(const aiScene *sc,
						const aiNode* nd);

	void get_bounding_box_for_node (const struct aiNode* nd,
						aiVector3D* min,
						aiVector3D* max);

	void color4_to_float4(const aiColor4D *c, float f[4]);
#endif
	void set_float4(float f[4], float a, float b, float c, float d);

};

#endif
