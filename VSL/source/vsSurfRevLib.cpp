/** ----------------------------------------------------------
 * \class VSSurfRevLib
 *
 * Lighthouse3D
 *
 * VSSurfRevLib - Very Simple Surfaces of Revolution Model Library
 *
 * This lib provides an interface to create models
 * based on surfaces of revolution 
 *
 * This lib requires the following classes from VSL:
 * (http://www.lighthouse3d.com/very-simple-libs)
 *
 * VSModelLib
 * VSResourceLib
 * VSMathLib 
 * VSLogLib
 * VSShaderLib
 *
 * and the following third party libs:
 *
 * GLEW (http://glew.sourceforge.net/),
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#include "vsSurfRevLib.h"

#include <stdlib.h>
#include <vector>
#include <cmath>
#include <cfloat>

VSSurfRevLib::VSSurfRevLib() {
}


VSSurfRevLib::~VSSurfRevLib() {
}


void 
VSSurfRevLib::createSphere(float radius, int divisions) {

	std::vector<float> p;
	circularProfile(p, -3.14159f/2.0f, 3.14159f/2.0f, radius, divisions);
	computeVAO(divisions+1, &(p[2]), &(p[0]), divisions*2, 0.0f);
}


void 
VSSurfRevLib::createTorus(float innerRadius, float outerRadius, int rings, int sides) {

	float tubeRadius = (outerRadius - innerRadius) * 0.5f;
	std::vector<float> p;
	circularProfile(p, -3.14159f, 3.14159f, tubeRadius, sides, innerRadius + tubeRadius);
	computeVAO(sides+1, &(p[2]), &(p[0]), rings, 0.0f);
}


void 
VSSurfRevLib::createCylinder(float height, float radius, int sides, int stacks) {

	std::vector<float> p;

	p.push_back(-radius); p.push_back(-height*0.5f);
	p.push_back(0.0f); p.push_back(-height*0.5f);

	float deltaH = height / stacks;
	for (int i = 0; i < stacks+1; ++i) {
		p.push_back(radius); p.push_back(-height*0.5f + i * deltaH);
	}

	p.push_back(0.0f); p.push_back(height*0.5f);
	p.push_back(-radius); p.push_back(height*0.5f);

	computeVAO(stacks+3, &(p[2]), (float *)&(p[0]), sides, 0.0f);
}


void 
VSSurfRevLib::createCone(float height, float baseRadius, int sides) {

	float v[2];
	v[0] = -baseRadius;
	v[1] = height;

	std::vector<float> p;

	p.push_back(-baseRadius);	p.push_back(0.0f);
	p.push_back(0.0f);			p.push_back(0.0f);
	p.push_back(baseRadius);	p.push_back(0.0f);
	int max = (int)(1 + height/ (baseRadius*2*3.14159 / sides)) ;
	for (int i = 0; i < max; ++i) {
	
		p.push_back(baseRadius - i * baseRadius  / max); p.push_back( i * height  / max);
	}
	p.push_back(0.0f);			p.push_back(height);
	p.push_back(-baseRadius);	p.push_back(height * 2.0f);

	computeVAO((int)((p.size()-4)/2), &(p[2]), &(p[0]), sides, 0.0f);
}


void
VSSurfRevLib::createPawn() {

		float p2[] = {0.0f, 0.0f, 
					  0.98f, 0.0f, 
					  0.98f, 0.01f,
					  0.99f, 0.02f,
					  1.0f , 0.02f,
					  1.0f , 0.3f,
					  0.99f, 0.31f,
					  0.98f, 0.32f,
					  0.93f, 0.32f,

					  0.95f, 0.38f,
					  0.965f, 0.44f,
					  0.97f, 0.48f,
					  0.965f, 0.52f,
					  0.95f, 0.56f,
					  0.89f, 0.62f,
					  0.83f, 0.66f,
					  0.75f, 0.70f,
					  0.66f, 0.735f,
					  0.65f, 0.74f,

					  
					  0.611f, 0.83f,
					  0.5f,   0.83f,

					  0.35f,  2.0f,

					  0.58f,  2.075f,
					  0.610f, 2.10f,
					  0.6225f, 2.1125f,
					  0.625f, 2.125f,
					  0.6225f, 2.1375f,
					  0.610f, 2.15f,
					  0.58f,  2.175f,

					  0.32f, 2.27f,
					  0.46f, 2.38f,
					  0.56f, 2.514f,
					  0.628f, 2.67f,
					  0.65f, 2.84f,

					  0.628f, 3.01f,
					  0.56f, 3.164f,
					  0.46f, 3.3f,
					  0.32f, 3.40f,
					  0.168f, 3.467f,
					  0.0f, 3.486f
					 };
		create(p2, 40, 30, 1, 0.6f);
}


void 
VSSurfRevLib::create (float *p, int numP, int sides, int closed, float smoothCos) {

	if (numP < 2)
		return;

	int i; 
	int totalPoints;
	// if closed the first point is replicated at the end
	// two extra points are added for normal computation
	if (closed)
		totalPoints = numP + 3;
	else
		totalPoints = numP + 2;

	std::vector<float> points;
	points.resize(totalPoints * 2);

	for (i = 2; i < (numP + 1) * 2; i++) {
		points[i] = p[i - 2];
	}
	if (closed) {
		points[(totalPoints - 2) * 2] = points[2];
		points[(totalPoints - 2) * 2 + 1] = points[3];
	}

	// distinguishes between closed curves and open curves
	// for normal computation
	int numPoints = numP + 2;

	if (closed) {

		points[0] = p[(numP - 1) * 2];
		points[1] = p[(numP - 1) * 2 + 1];

		points[(numPoints ) * 2] = p[2];
		points[(numPoints ) * 2 + 1] = p[3];
	}
	else {

		points[0] = points[2] + (points[2] - points[4]);
		points[1] = points[3] + (points[3] - points[5]);

		points[(numPoints - 1) * 2] = points[(numPoints - 2) * 2] +
			(points[(numPoints - 2) * 2] - points[(numPoints - 3) * 2]);
		points[(numPoints - 1) * 2 + 1] = points[(numPoints - 2) * 2 + 1] +
			(points[(numPoints - 2) * 2 + 1] - points[(numPoints - 3) * 2 + 1]);
	}

	computeVAO(totalPoints-2, &(points[2]), &(points[0]), sides, smoothCos);
}


void 
VSSurfRevLib::computeVAO(int numP, float *p, float *points, int sides, float smoothCos) {
	// Compute and store vertices

	int numSides = sides;
	int numPoints = numP + 2;

	std::vector<float> vertex, normal, textco, tangent, bitangent;
	vertex.resize(numP * 2 * 4 * (numSides + 1));
	normal.resize(numP * 2 * 3 * (numSides + 1));
	textco.resize(numP * 2 * 2 * (numSides + 1));
	tangent.resize(numP * 2 * 3 * (numSides + 1));
	bitangent.resize(numP * 2 * 3 * (numSides + 1));

	float inc = 2 * 3.14159f / (numSides);
	float nx,ny;
	float delta;
	int smooth;
	std::vector<int> smoothness;
	int k = 0;
	float y[3] = { 0.0f, 1.0f, 0.0f };
	float z[3] = { 0.0f, 0.0f, 1.0f };

	// init bounding box
	for(int i = 0; i < 3; ++i) {
		bb[1][i] = -FLT_MAX;
		bb[0][i] = FLT_MAX;
	}

	for(int i=0; i < numP; i++) {
		revSmoothNormal2(points+(i*2),&nx,&ny, smoothCos, 0);
		for(int j=0; j<=numSides;j++) {

			if ((i == 0 && p[0] == 0.0f) || ( i == numP-1 && p[(i+1)*2] == 0.0))
				delta = inc * 0.5f;
			else
				delta = 0.0f;

			normal[((k)*(numSides+1) + j)*3]   = nx * cosf(j*inc+delta);
			normal[((k)*(numSides+1) + j)*3+1] = ny;
			normal[((k)*(numSides+1) + j)*3+2] = nx * sinf(-j*inc+delta);

			vertex[((k)*(numSides+1) + j)*4]   = p[i*2] * cosf(j*inc);
			vertex[((k)*(numSides+1) + j)*4+1] = p[(i*2)+1];
			vertex[((k)*(numSides+1) + j)*4+2] = p[i*2] * sinf(-j*inc);
			vertex[((k)*(numSides+1) + j)*4+3] = 1.0f;

			textco[((k)*(numSides+1) + j)*2]   = ((j+0.0f)/numSides);
			textco[((k)*(numSides+1) + j)*2+1] = (i+0.0f)/(numP-1);

			float t[3]; float *n = (float *)&(normal[((k)*(numSides + 1) + j) * 3]);
			if (fabs(VSMathLib::dotProduct(y, n)) > 0.98) {
				t[0] = sinf(-j*inc + delta); t[1] = 0; t[2] = -cosf(j*inc + delta);
			}
			else {
				VSMathLib::crossProduct(y, n, t);
			}
			tangent[((k)*(numSides + 1) + j) * 3] = t[0];
			tangent[((k)*(numSides + 1) + j) * 3 + 1] = t[1];
			tangent[((k)*(numSides + 1) + j) * 3 + 2] = t[2];


			float bt[3];
			VSMathLib::crossProduct(t, n, bt);
			bitangent[((k)*(numSides + 1) + j) * 3] = bt[0];
			bitangent[((k)*(numSides + 1) + j) * 3 + 1] = bt[1];
			bitangent[((k)*(numSides + 1) + j) * 3 + 2] = bt[2];

			// find bounding box
			if (vertex[((k)*(numSides+1) + j)*4] < bb[0][0]) 
				bb[0][0] = vertex[((k)*(numSides+1) + j)*4];
			if (vertex[((k)*(numSides+1) + j)*4] > bb[1][0]) 
				bb[1][0] = vertex[((k)*(numSides+1) + j)*4];

			if (vertex[((k)*(numSides+1) + j)*4+1] < bb[0][1]) 
				bb[0][1] = vertex[((k)*(numSides+1) + j)*4+1];
			if (vertex[((k)*(numSides+1) + j)*4+1] > bb[1][1]) 
				bb[1][1] = vertex[((k)*(numSides+1) + j)*4+1];

			if (vertex[((k)*(numSides+1) + j)*4+2] < bb[0][2]) 
				bb[0][2] = vertex[((k)*(numSides+1) + j)*4+2];
			if (vertex[((k)*(numSides+1) + j)*4+2] > bb[1][2]) 
				bb[1][2] = vertex[((k)*(numSides+1) + j)*4+2];
		}

		k++;
		if (i < numP-1) {
			smooth = revSmoothNormal2(points+((i+1)*2),&nx,&ny, smoothCos, 1);

			if (!smooth) {
				smoothness.push_back(1);
				for(int j=0; j<=numSides;j++) {

					normal[((k)*(numSides+1) + j)*3]   = nx * cosf(j*inc);
					normal[((k)*(numSides+1) + j)*3+1] = ny;
					normal[((k)*(numSides+1) + j)*3+2] = nx * sinf(-j*inc);

					vertex[((k)*(numSides+1) + j)*4]   = p[(i+1)*2] * cosf(j*inc);
					vertex[((k)*(numSides+1) + j)*4+1] = p[((i+1)*2)+1];
					vertex[((k)*(numSides+1) + j)*4+2] = p[(i+1)*2] * sinf(-j*inc);
					vertex[((k)*(numSides+1) + j)*4+3] = 1.0f;

					textco[((k)*(numSides+1) + j)*2]   = ((j+0.0f)/numSides);
					textco[((k)*(numSides+1) + j)*2+1] = (i+1+0.0f)/(numP-1);

					float t[3]; float *n = (float *)&(normal[((k)*(numSides + 1) + j) * 3]);
					if (fabs(VSMathLib::dotProduct(y, n)) > 0.98) {
						t[0] = sinf(-j*inc + delta); t[1] = 0; t[2] = -cosf(j*inc + delta);
					}
					else {
						VSMathLib::crossProduct(y, n, t);
					}
					tangent[((k)*(numSides + 1) + j) * 3] = t[0];
					tangent[((k)*(numSides + 1) + j) * 3 + 1] = t[1];
					tangent[((k)*(numSides + 1) + j) * 3 + 2] = t[2];


					float bt[3];
					VSMathLib::crossProduct(t, n, bt);
					bitangent[((k)*(numSides + 1) + j) * 3] = bt[0];
					bitangent[((k)*(numSides + 1) + j) * 3 + 1] = bt[1];
					bitangent[((k)*(numSides + 1) + j) * 3 + 2] = bt[2];

					// find bounding box
					if (vertex[((k)*(numSides+1) + j)*4] < bb[0][0]) 
						bb[0][0] = vertex[((k)*(numSides+1) + j)*4];
					if (vertex[((k)*(numSides+1) + j)*4] > bb[1][0]) 
						bb[1][0] = vertex[((k)*(numSides+1) + j)*4];

					if (vertex[((k)*(numSides+1) + j)*4+1] < bb[0][1]) 
						bb[0][1] = vertex[((k)*(numSides+1) + j)*4+1];
					if (vertex[((k)*(numSides+1) + j)*4+1] > bb[1][1]) 
						bb[1][1] = vertex[((k)*(numSides+1) + j)*4+1];

					if (vertex[((k)*(numSides+1) + j)*4+2] < bb[0][2]) 
						bb[0][2] = vertex[((k)*(numSides+1) + j)*4+2];
					if (vertex[((k)*(numSides+1) + j)*4+2] > bb[1][2]) 
						bb[1][2] = vertex[((k)*(numSides+1) + j)*4+2];
				}
				k++;
			}
			else
				smoothness.push_back(0);
		}
	}

	bbInit = true;

	std::vector<unsigned int> faceIndex;
	faceIndex.resize((numP - 1) * (numSides + 1) * 6);
	int count = 0;
	k = 0;
	for (int i = 0; i < numP-1; ++i) {
		for (int j = 0; j < numSides; ++j) {
		
			/*if (i != 0 || p[0] != 0.0)*/ {
				faceIndex[count++] = (unsigned int)(k * (numSides+1) + j);
				faceIndex[count++] = (unsigned int)((k+1) * (numSides+1) + j + 1);
				faceIndex[count++] = (unsigned int)((k+1) * (numSides+1) + j);
			}
			/*if (i != numP-2 || p[(numP-1)*2] != 0.0)*/ {
				faceIndex[count++] = (unsigned int)(k * (numSides+1) + j);
				faceIndex[count++] = (unsigned int)(k * (numSides+1) + j + 1);
				faceIndex[count++] = (unsigned int)((k+1) * (numSides+1) + j + 1);
			}

		}
		k++;
		k += smoothness[i];	
	}

	MyMesh aMesh;

	buildVAO(aMesh, (k + 1)*(numSides + 1), &(vertex[0]), &(normal[0]), &(textco[0]), &(tangent[0]), &(bitangent[0]), count, &(faceIndex[0]));

	aMesh.type = GL_TRIANGLES;
	aMesh.mat.ambient[0] = 0.2f;
	aMesh.mat.ambient[1] = 0.2f;
	aMesh.mat.ambient[2] = 0.2f;
	aMesh.mat.ambient[3] = 1.0f;

	aMesh.mat.diffuse[0] = 0.8f;
	aMesh.mat.diffuse[1] = 0.8f;
	aMesh.mat.diffuse[2] = 0.8f;
	aMesh.mat.diffuse[3] = 1.0f;

	aMesh.mat.specular[0] = 0.8f;
	aMesh.mat.specular[1] = 0.8f;
	aMesh.mat.specular[2] = 0.8f;
	aMesh.mat.specular[3] = 1.0f;

	aMesh.mat.shininess = 100.0f;
	aMesh.mat.texCount = 0;

	mVSML->loadIdentity(VSMathLib::AUX0);
	memcpy(aMesh.transform, mVSML->get(VSMathLib::AUX0),
			sizeof(float) * 16);

	mMyMeshes.push_back(aMesh);
}


int 
VSSurfRevLib::revSmoothNormal2(float *p, float *nx, float *ny, float smoothCos, int beginEnd) {

	float v1x,v1y,v2x,v2y,x,y,norm;
	float auxX, auxY, angle;

	auxX = p[0] - p[2];
	auxY = p[1] - p[3];
	v1x = -auxY;
	v1y = auxX;
	norm = sqrtf((v1x*v1x) + (v1y*v1y));
	v1x /= norm;
	v1y /= norm;

	auxX = p[2] - p[4];
	auxY = p[3] - p[5];
	v2x = -auxY;
	v2y = auxX;
	norm = sqrtf((v2x*v2x) + (v2y*v2y));
	v2x /= norm;
	v2y /= norm;

	angle = v1x * v2x + v1y * v2y;

 	if (angle > smoothCos) {
		x = v1x + v2x;
		y = v1y + v2y;
	}
	else if (beginEnd == 0) {
		x = v2x;
		y = v2y;
	}
	else  {
		x = v1x;
		y = v1y;
	
	}

	norm=sqrtf(x*x+ y*y);

	assert(norm!=0);
	x /= norm;
	y /= norm;

	*nx = x;
	*ny = y;
	if (angle > smoothCos)
		return 1;
	else
		return 0;
}


float 
VSSurfRevLib::computeTotalSegmentLength(float *p, int numP) {

	float l,aux;
	int i;

	l = 0;
	for(i=0;i<numP-1;i++) {

		aux = (p[(i+1)*2]-p[i*2]) * (p[(i+1)*2]-p[i*2]) + (p[(i+1)*2+1]-p[i*2+1]) * (p[(i+1)*2+1]-p[i*2+1]);
		l += sqrt(aux);
	}
	return l;
}


float 
VSSurfRevLib::computeSegmentLength(float *p) {

	float l,aux;

	aux = (p[2]-p[0]) * (p[2]-p[0]) + (p[3]-p[1]) * (p[3]-p[1]);
	l = sqrtf(aux);

	return l;

}


void 
VSSurfRevLib::circularProfile(std::vector<float> &res, float minAngle, float maxAngle, float radius, int divisions, float transX, float transY ) {

	float step = (maxAngle - minAngle)/ divisions;

	for (int i = 0, k = -1; i < divisions+3; ++i,++k) {
	
		res.push_back(radius * cosf(minAngle + k * step) + transX);
		res.push_back(radius * sinf(minAngle + k * step) + transY);
	}

}

