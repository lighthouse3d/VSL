/** ----------------------------------------------------------
 * \class VSResModelLib
 *
 * Lighthouse3D
 *
 * VSResModelLib - Very Simple Resource Model Library
 *
 * \version 0.2.0
 *		Added cubemap textures
 *		Added the possibility to reuse textures	
 *		Loader gets tangent and bitangent attributes
 *
 * version 0.1.0
 * Initial Release
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

#include "vsResModelLib.h"



VSResModelLib::VSResModelLib():pScene(0), pUseAdjacency(true)
{
	mMyMeshes.reserve(10);
	pMyMeshesAux.reserve(10);
}


VSResModelLib::~VSResModelLib() {

	for (unsigned int i = 0; i < mMyMeshes.size(); ++i) {
			
		glDeleteVertexArrays(1,&(mMyMeshes[i].vao));

#ifdef _VSL_TEXTURE_WITH_DEVIL

		for (int t = 0; t < MAX_TEXTURES; ++t ) {
			glDeleteTextures(1,&(mMyMeshes[i].texUnits[t]));
		}
#endif
		glDeleteBuffers(1,&(mMyMeshes[i].uniformBlockIndex));
	}
	mMyMeshes.clear();
}


void
VSResModelLib::clone(VSResourceLib *res) {

	if (res == NULL)
		return;

	VSResModelLib *r = (VSResModelLib *)res;

	this->mMyMeshes = r->mMyMeshes;
}




bool
VSResModelLib::load(std::string filename) {

	Assimp::Importer importer;

	//check if file exists
	std::ifstream fin(filename.c_str());
	if(!fin.fail()) 
		fin.close();
	else {
		VSLOG(sLogError, "Unable to open file %s", 
							filename.c_str());
		return false;
	}

//	pScene = aiImportFile(filename.c_str(),aiProcessPreset_TargetRealtime_MaxQuality);
	pScene = importer.ReadFile( filename, 
					aiProcessPreset_TargetRealtime_Quality);

	// If the import failed, report it
	if( !pScene) {
		VSLOG(sLogError, "Failed to import %s", 
					filename.c_str());
		return false;
	}
	// Get prefix for texture loading
	size_t index = filename.find_last_of("/\\");
	std::string prefix = filename.substr(0, index+1);

#ifdef _VSL_TEXTURE_WITH_DEVIL

	pTextureIdMap.clear();
	bool result = loadTextures(pScene, prefix);

#endif

	genVAOsAndUniformBuffer(pScene);

	// determine bounding box
	aiVector3t<float> min, max;
//	aiVector3D min, max;
	min.x = min.y = min.z =  1e10f;
	max.x = max.y = max.z = -1e10f;

	mVSML->loadIdentity(VSMathLib::AUX0);
	get_bounding_box_for_node(pScene->mRootNode,&min,&max);

	//bb[0][0] = min.x;
	//bb[0][1] = min.y;
	//bb[0][2] = min.z;

	//bb[1][0] = max.x;
	//bb[1][1] = max.y;
	//bb[1][2] = max.z;


	bbInit = true;

	float tmp;
	tmp = max.x - min.x;
	tmp = max.y - min.y > tmp? max.y - min.y:tmp;
	tmp = max.z - min.z > tmp? max.z - min.z:tmp;
	
	mScaleToUnitCube = 2.0f / tmp;

	bb[0][0] = -1.0f * (max.x - min.x) * mScaleToUnitCube/2;
	bb[0][1] = -1.0f * (max.y - min.y) * mScaleToUnitCube/2;
	bb[0][2] = -1.0f * (max.z - min.z) * mScaleToUnitCube/2;

	bb[1][0] = 1.0f * (max.x - min.x) * mScaleToUnitCube/2;
	bb[1][1] = 1.0f * (max.y - min.y) * mScaleToUnitCube/2;
	bb[1][2] = 1.0f * (max.z - min.z) * mScaleToUnitCube/2;

	mCenter[0] = min.x + (max.x - min.x) * 0.5f;
	mCenter[1] = min.y + (max.y - min.y) * 0.5f;
	mCenter[2] = min.z + (max.z - min.z) * 0.5f;

	mVSML->loadIdentity(VSMathLib::AUX0);
	mVSML->scale(VSMathLib::AUX0, mScaleToUnitCube, mScaleToUnitCube, mScaleToUnitCube);
	mVSML->translate(VSMathLib::AUX0, -mCenter[0], -mCenter[1], -mCenter[2]);

	for (unsigned int i = 0; i < mMyMeshes.size(); ++i) {

		mVSML->pushMatrix(mVSML->AUX0);

		mVSML->multMatrix(mVSML->AUX0, mMyMeshes[i].transform);
		memcpy(mMyMeshes[i].transform, mVSML->get(mVSML->AUX0), sizeof(float)*16);

		mVSML->popMatrix(mVSML->AUX0);
	}

#ifdef _VSL_TEXTURE_WITH_DEVIL
	// clear texture map
	pTextureIdMap.clear();
	return result;
#else
	return true;
#endif
}


void 
VSResModelLib::render () {

	// we are only going to use texture unit 0

	mVSML->pushMatrix(VSMathLib::MODEL);
	//mVSML->scale(mScaleToUnitCube, mScaleToUnitCube, mScaleToUnitCube);
	//mVSML->translate(-mCenter[0], -mCenter[1], -mCenter[2]);
	for (unsigned int i = 0; i < mMyMeshes.size(); ++i) {
	
		mVSML->pushMatrix(VSMathLib::MODEL);
		mVSML->multMatrix(VSMathLib::MODEL, 
								mMyMeshes[i].transform);
		// send matrices to shaders
		mVSML->matricesToGL();

		// set material
		setMaterial(mMyMeshes[i].mat);

#ifdef _VSL_TEXTURE_WITH_DEVIL

		// bind texture
		for (unsigned int j = 0; j < VSResourceLib::MAX_TEXTURES; ++j) {
			if (mMyMeshes[i].texUnits[j] != 0) {
				glActiveTexture(GL_TEXTURE0 + j);
				glBindTexture(mMyMeshes[i].texTypes[j], 
						mMyMeshes[i].texUnits[j]);
			}
		}
#endif
		// bind VAO
		glBindVertexArray(mMyMeshes[i].vao);
		glDrawElements(mMyMeshes[i].type, 
			mMyMeshes[i].numIndices, GL_UNSIGNED_INT, 0);

#ifdef _VSL_TEXTURE_WITH_DEVIL
		for (unsigned int j = 0; j < VSResourceLib::MAX_TEXTURES; ++j) {
			if (mMyMeshes[i].texUnits[j] != 0) {
				glActiveTexture(GL_TEXTURE0 + j);
				glBindTexture(mMyMeshes[i].texTypes[j], 0);
			}
		}
#endif
		mVSML->popMatrix(VSMathLib::MODEL);
	}
	glBindVertexArray(0);
	mVSML->popMatrix(VSMathLib::MODEL);
}

#ifdef _VSL_TEXTURE_WITH_DEVIL

// Load model textures
bool 
VSResModelLib::loadTextures(const aiScene* scene, 
							std::string prefix)
{
	VSLOG(sLogInfo, "Loading Textures from %s", 
						prefix.c_str());
	
	/* scan scene's materials for textures */
	for (unsigned int m=0; m<scene->mNumMaterials; ++m)
	{
		int texIndex = 0;
		aiString path;	// filename

		aiReturn texFound = 
			scene->mMaterials[m]->
			GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		
		while (texFound == AI_SUCCESS) {
			//fill map with textures, OpenGL image ids set to 0
			pTextureIdMap[path.data] = 0; 
			// more textures?
			texIndex++;
			texFound = 
				scene->mMaterials[m]->
				GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		}
	}

	int numTextures = pTextureIdMap.size();

	/* get iterator */
	std::map<std::string, GLuint>::iterator itr = 
							pTextureIdMap.begin();

	for (int i= 0; itr != pTextureIdMap.end(); ++i, ++itr)
	{
		// get filename
		std::string filename = (*itr).first;  
		filename = prefix + filename;
		// save texture id for filename in map
		(*itr).second = loadRGBATexture(filename, true,true);	  
		VSLOG(sLogInfo, "Texture %s loaded with name %d", 
			filename.c_str(), (int)(*itr).second);
	}


	return true;
}

#endif

void 
VSResModelLib::genVAOsAndUniformBuffer(const struct aiScene *sc) {

	struct MyMesh aMesh;
	struct Material aMat; 
	GLuint buffer;
	int totalTris = 0, totalVerts = 0;
	unsigned int *adjFaceArray;
	
	VSLOG(sLogInfo, "Number of Meshes: %d",sc->mNumMeshes);
	// For each mesh
	for (unsigned int n = 0; n < sc->mNumMeshes; ++n)
	{
		const struct aiMesh* mesh = sc->mMeshes[n];

		if (mesh->mPrimitiveTypes != 4) {
			aMesh.numIndices = 0;
			pMyMeshesAux.push_back(aMesh);
			continue;
		}

		VSLOG(sLogInfo, "Mesh[%d] Triangles %d",n, 
							mesh->mNumFaces);
		totalTris += mesh->mNumFaces;
		// create array with faces
		// have to convert from Assimp format to array
		unsigned int *faceArray;
		faceArray = (unsigned int *)malloc(
				sizeof(unsigned int) * mesh->mNumFaces * 3);

		aMesh.indexes = faceArray;

		unsigned int faceIndex = 0;

		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];

			memcpy(&faceArray[faceIndex], face->mIndices,
								3 * sizeof(unsigned int));
			faceIndex += 3;
		}

		if (pUseAdjacency) {
			// Create the half edge structure
			std::map<std::pair<unsigned int,unsigned int>, struct HalfEdge *> myEdges;
			struct HalfEdge *edge;

			// fill it up with edges. twin info will be added latter
			edge = (struct HalfEdge *)malloc(sizeof(struct HalfEdge) * mesh->mNumFaces * 3);
			for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		
				edge[i*3].vertex = faceArray[i*3+1];
				edge[i*3+1].vertex = faceArray[i*3+2];
				edge[i*3+2].vertex = faceArray[i*3];

				edge[i*3].next = &edge[i*3+1];
				edge[i*3+1].next = &edge[i*3+2];
				edge[i*3+2].next = &edge[i*3];

				edge[i*3].twin   = NULL;
				edge[i*3+1].twin = NULL;
				edge[i*3+2].twin = NULL;

				myEdges[std::pair<unsigned int,unsigned int>(faceArray[i*3+2],faceArray[i*3])] = &edge[i*3];
				myEdges[std::pair<unsigned int,unsigned int>(faceArray[i*3],faceArray[i*3+1])] = &edge[i*3+1];
				myEdges[std::pair<unsigned int,unsigned int>(faceArray[i*3+1],faceArray[i*3+2])] = &edge[i*3+2];
			}

			// add twin info
			std::map<std::pair<unsigned int,unsigned int>, struct HalfEdge *>::iterator iter;
			std::pair<unsigned int,unsigned int> edgeIndex, twinIndex;

			iter = myEdges.begin();

			for (; iter != myEdges.end(); ++iter) {
		 
				edgeIndex = iter->first;
				twinIndex = std::pair<unsigned int, unsigned int>(edgeIndex.second, edgeIndex.first);

				if (myEdges.count(twinIndex))
					iter->second->twin = myEdges[twinIndex];
				else
					iter->second->twin = NULL;
			}
		
			adjFaceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 6);
			for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		
				// NOTE: twin may be null
				adjFaceArray[i*6]   = edge[3*i + 0].next->vertex;
				adjFaceArray[i*6+1] = edge[3*i + 0].twin?edge[3*i + 0].twin->vertex:edge[3*i + 0].next->vertex;

				adjFaceArray[i*6+2] = edge[3*i + 1].next->vertex;
				adjFaceArray[i*6+3] = edge[3*i + 1].twin?edge[3*i + 1].twin->vertex:edge[3*i + 1].next->vertex;

				adjFaceArray[i*6+4] = edge[3*i + 2].next->vertex;
				adjFaceArray[i*6+5] = edge[3*i + 2].twin?edge[3*i + 2].twin->vertex:edge[3*i + 2].next->vertex;

			}
		}
		if (pUseAdjacency) {
			//printf("\n");
			//for (unsigned int i = 0; i < mesh->mNumFaces * 3; ++i)
			//	printf("%d ", faceArray[i]);
			//printf("\n");
			//for (unsigned int i = 0; i < mesh->mNumFaces * 6; ++i)
			//	printf("%d ", adjFaceArray[i]);
			//printf("\n");
			aMesh.numIndices = sc->mMeshes[n]->mNumFaces * 6;
		}
		else
			aMesh.numIndices = sc->mMeshes[n]->mNumFaces * 6;



		// generate Vertex Array for mesh
		glGenVertexArrays(1,&(aMesh.vao));
		glBindVertexArray(aMesh.vao);

		// buffer for faces
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

		if (pUseAdjacency) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
						sizeof(unsigned int) * mesh->mNumFaces * 6,
						adjFaceArray, GL_STATIC_DRAW);
			free(adjFaceArray);
		}
		else
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
						sizeof(unsigned int) * mesh->mNumFaces * 3,
						faceArray, GL_STATIC_DRAW);

//		free(faceArray);

		// buffer for vertex positions
		if (mesh->HasPositions()) {

			aMesh.positions = (float *) malloc(sizeof(float) * 3 * mesh->mNumVertices);
			memcpy(aMesh.positions, mesh->mVertices, sizeof(float) * 3 * mesh->mNumVertices);

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float)*3*mesh->mNumVertices, 
				mesh->mVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(
					VSShaderLib::VERTEX_COORD_ATTRIB);
			glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB,
					3, GL_FLOAT, 0, 0, 0);
			totalVerts += mesh->mNumVertices;
		}

		// buffer for vertex normals
		if (mesh->HasNormals()) {

			aMesh.normals = (float *) malloc(sizeof(float) * 3 * mesh->mNumVertices);
			memcpy(aMesh.normals, mesh->mNormals, sizeof(float) * 3 * mesh->mNumVertices);

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float)*3*mesh->mNumVertices, mesh->mNormals, 
				GL_STATIC_DRAW);
			glEnableVertexAttribArray(VSShaderLib::NORMAL_ATTRIB);
			glVertexAttribPointer(VSShaderLib::NORMAL_ATTRIB, 
				3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex tangents
		if (mesh->HasTangentsAndBitangents()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float)*3*mesh->mNumVertices, mesh->mTangents, 
				GL_STATIC_DRAW);
			glEnableVertexAttribArray(VSShaderLib::TANGENT_ATTRIB);
			glVertexAttribPointer(VSShaderLib::TANGENT_ATTRIB, 
				3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex bitangents
		if (mesh->HasTangentsAndBitangents()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float)*3*mesh->mNumVertices, mesh->mBitangents, 
				GL_STATIC_DRAW);
			glEnableVertexAttribArray(VSShaderLib::BITANGENT_ATTRIB);
			glVertexAttribPointer(VSShaderLib::BITANGENT_ATTRIB, 
				3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex texture coordinates
		if (mesh->HasTextureCoords(0)) {
			float *texCoords = (float *)malloc(
						sizeof(float)*2*mesh->mNumVertices);
			for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {

				texCoords[k*2]   = mesh->mTextureCoords[0][k].x;
				texCoords[k*2+1] = mesh->mTextureCoords[0][k].y; 
				
			}
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float)*2*mesh->mNumVertices, texCoords, 
				GL_STATIC_DRAW);
			glEnableVertexAttribArray(
				VSShaderLib::TEXTURE_COORD_ATTRIB);
			glVertexAttribPointer(
				VSShaderLib::TEXTURE_COORD_ATTRIB, 2, 
				GL_FLOAT, 0, 0, 0);
			free(texCoords);
		}

		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	
		// create material uniform buffer
		struct aiMaterial *mtl = 
			sc->mMaterials[mesh->mMaterialIndex];
			
		aiString texPath;	//contains filename of texture

#ifdef _VSL_TEXTURE_WITH_DEVIL
		for (int j = 0; j < VSResourceLib::MAX_TEXTURES; ++j)
			aMesh.texUnits[j] = 0;

		if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 
											0, &texPath)){
				//bind texture
				aMesh.texUnits[0] = 
								pTextureIdMap[texPath.data];
				aMesh.texTypes[0] = GL_TEXTURE_2D;
				aMat.texCount = 1;
			}
		else {
			aMesh.texUnits[0] = 0;
			aMat.texCount = 0;
		}
#endif
		float c[4];
		set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
		aiColor4D diffuse;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, 
							AI_MATKEY_COLOR_DIFFUSE, &diffuse))
			color4_to_float4(&diffuse, c);
		memcpy(aMat.diffuse, c, sizeof(c));

		set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
		aiColor4D ambient;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, 
							AI_MATKEY_COLOR_AMBIENT, &ambient))
			color4_to_float4(&ambient, c);
		memcpy(aMat.ambient, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D specular;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, 
							AI_MATKEY_COLOR_SPECULAR, &specular))
			color4_to_float4(&specular, c);
		memcpy(aMat.specular, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D emission;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, 
							AI_MATKEY_COLOR_EMISSIVE, &emission))
			color4_to_float4(&emission, c);
		memcpy(aMat.emissive, c, sizeof(c));

		float shininess = 0.0;
		unsigned int max;
		aiGetMaterialFloatArray(mtl, 
							AI_MATKEY_SHININESS, &shininess, &max);
		aMat.shininess = shininess;

		aMesh.mat = aMat;

		pMyMeshesAux.push_back(aMesh);
	}

	mVSML->loadIdentity(VSMathLib::AUX0);
	recursive_walk_for_matrices(sc, sc->mRootNode);

	pMyMeshesAux.clear();

	VSLOG(sLogInfo, "Total Meshes: %d  | Vertices: %d | Faces: %d",
					sc->mNumMeshes, totalVerts, totalTris);
}


#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

void 
VSResModelLib::get_bounding_box_for_node (const aiNode* nd, 
	aiVector3D* min, 
	aiVector3D* max)
	
{
	unsigned int n = 0;

	mVSML->pushMatrix(VSMathLib::AUX0);

	if (nd->mNumMeshes) {

		// Get node transformation matrix
		aiMatrix4x4 m = nd->mTransformation;

		// OpenGL matrices are column major
		m.Transpose();

		// apply node transformation
		float aux[16];
		memcpy(aux,&m,sizeof(float) * 16);
		mVSML->multMatrix(VSMathLib::AUX0, aux);


		for (; n < nd->mNumMeshes; ++n) {
			const struct aiMesh* mesh = 
							pScene->mMeshes[nd->mMeshes[n]];
			for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {

				aiVector3D tmp = mesh->mVertices[t];
				float a[4], res[4];

				a[0] = tmp.x; 
				a[1] = tmp.y; 
				a[2] = tmp.z; 
				a[3] = 1.0f;

				mVSML->multMatrixPoint(VSMathLib::AUX0, a, res);
			
				min->x = aisgl_min(min->x,res[0]);
				min->y = aisgl_min(min->y,res[1]);
				min->z = aisgl_min(min->z,res[2]);

				max->x = aisgl_max(max->x,res[0]);
				max->y = aisgl_max(max->y,res[1]);
				max->z = aisgl_max(max->z,res[2]);
			}
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n],min,max);
	}

	mVSML->popMatrix(VSMathLib::AUX0);
}


void 
VSResModelLib::recursive_walk_for_matrices(
			const struct aiScene *sc, 
			const struct aiNode* nd) {

	mVSML->pushMatrix(VSMathLib::AUX0);
	if (nd->mNumMeshes)
	{
		// Get node transformation matrix
		aiMatrix4x4 m = nd->mTransformation;
		// OpenGL matrices are column major
		m.Transpose();
		// save model matrix and apply node transformation
		float aux[16];
		memcpy(aux,&m,sizeof(float) * 16);
		mVSML->multMatrix(VSMathLib::AUX0, aux);
	
		// get matrices for all meshes assigned to this node
		for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {

			if(sc->mMeshes[nd->mMeshes[n]]->mPrimitiveTypes == 4) {

				struct MyMesh aMesh;
				memcpy(&aMesh, &(pMyMeshesAux[nd->mMeshes[n]]), 
											sizeof (aMesh));
				memcpy(aMesh.transform,mVSML->get(VSMathLib::AUX0), 
											sizeof(float)*16);
				if (pUseAdjacency)
					aMesh.type = GL_TRIANGLES_ADJACENCY;
				else
					aMesh.type = GL_TRIANGLES;
				mMyMeshes.push_back(aMesh);
			}
		}
	}
	// recurse for all children
	for (unsigned int n=0; n < nd->mNumChildren; ++n){
		recursive_walk_for_matrices(sc, nd->mChildren[n]);
	}
	mVSML->popMatrix(VSMathLib::AUX0);
}



void 
VSResModelLib::setColor(VSResourceLib::MaterialSemantics m, float *values) {

	if (m == TEX_COUNT)
		return;

	for (unsigned int i = 0; i < mMyMeshes.size(); ++i) {

		switch(m) {
			case SHININESS:
				mMyMeshes[i].mat.shininess = *values;
				break;
			case DIFFUSE:
				memcpy(mMyMeshes[i].mat.diffuse, values, sizeof(float)*4);
				break;
			case AMBIENT:
				memcpy(mMyMeshes[i].mat.ambient, values, sizeof(float)*4);
				break;
			case SPECULAR:
				memcpy(mMyMeshes[i].mat.specular, values, sizeof(float)*4);
				break;
			case EMISSIVE:
				memcpy(mMyMeshes[i].mat.emissive, values, sizeof(float)*4);
				break;
		}
	}
}


void 
VSResModelLib::setColor(unsigned int mesh, VSResourceLib::MaterialSemantics m, float *values) {

	if (mesh >= mMyMeshes.size())
		return;

	if (m == TEX_COUNT)
		return;

	switch(m) {
		case SHININESS:
			mMyMeshes[mesh].mat.shininess = *values;
			break;
		case DIFFUSE:
			memcpy(mMyMeshes[mesh].mat.diffuse, values, sizeof(float)*4);
			break;
		case AMBIENT:

			memcpy(mMyMeshes[mesh].mat.ambient, values, sizeof(float)*4);
			break;
		case SPECULAR:
			memcpy(mMyMeshes[mesh].mat.specular, values, sizeof(float)*4);
			break;
		case EMISSIVE:
			memcpy(mMyMeshes[mesh].mat.emissive, values, sizeof(float)*4);
			break;
	}
}


void
VSResModelLib::setMaterialColor(MaterialColors m) {

	for (unsigned int i = 0; i < mMyMeshes.size(); ++i) {

		memcpy(mMyMeshes[i].mat.ambient, &(VSResourceLib::Colors[m][0]), 3*sizeof(float));
		memcpy(mMyMeshes[i].mat.diffuse, &(VSResourceLib::Colors[m][3]), 3*sizeof(float));
		memcpy(mMyMeshes[i].mat.specular, &(VSResourceLib::Colors[m][6]), 3*sizeof(float));
		mMyMeshes[i].mat.shininess = VSResourceLib::Colors[m][9];
	}	
}

#ifdef _VSL_TEXTURE_WITH_DEVIL

void 
VSResModelLib::setTexture(unsigned int unit, unsigned int textureID, GLenum textureType) {

	for (unsigned int i = 0; i < mMyMeshes.size(); ++i) {
		mMyMeshes[i].texUnits[unit] = textureID;
		mMyMeshes[i].texTypes[unit] = textureType;
		mMyMeshes[i].mat.texCount = 1;
	}
}


void 
VSResModelLib::addTexture(unsigned int unit, std::string filename) {

	int textID = loadRGBATexture(filename, true);
	for (unsigned int i = 0; i < mMyMeshes.size(); ++i) {
		mMyMeshes[i].texUnits[unit] = textID;
		mMyMeshes[i].texTypes[unit] = GL_TEXTURE_2D;
		mMyMeshes[i].mat.texCount = 1;
	}
}


void 
VSResModelLib::addCubeMapTexture(unsigned int unit, std::string posX, std::string negX, 
									std::string posY, std::string negY, 
									std::string posZ, std::string negZ) {

	int textID = loadCubeMapTexture(posX, negX, posY, negY, posZ, negZ);
	for (unsigned int i = 0; i < mMyMeshes.size(); ++i) {
		mMyMeshes[i].texUnits[unit] = textID;
		mMyMeshes[i].texTypes[unit] = GL_TEXTURE_CUBE_MAP;
		mMyMeshes[i].mat.texCount = 1;
	}
}

#endif

// Auxiliary functions to convert Assimp data to float arays
void 
VSResModelLib::set_float4(float f[4], 
					float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}


// Auxiliary functions to convert Assimp data to float arays
void 
VSResModelLib::color4_to_float4(const aiColor4D *c, 
					float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}
