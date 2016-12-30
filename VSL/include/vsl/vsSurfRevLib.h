/** ----------------------------------------------------------
 * \class VSSurfRevLib
 *
 * Lighthouse3D
 *
 * VSSurfRevLib - Very Simple Surfaces of Revolution Model Library
 *
 * \version 0.1.0
 *		Initial Release
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

 
#ifndef __VSResSurfRevLib__
#define __VSResSurfRevLib__

#include <string>
#include <assert.h>

#ifdef __ANDROID_API__
#include <GLES3/gl31.h>
#include <android/asset_manager.h>

#else
#include <GL/glew.h>
#endif

#include "vsModelLib.h"


class VSSurfRevLib : public VSModelLib{

public:

	VSSurfRevLib();
	~VSSurfRevLib();

	void create (float *p, int numP, int sides, int closed, float smoothCos);
	void createSphere(float radius, int divisions);
	void createTorus(float innerRadius, float outerRadius, int rings, int sides);
	void createCylinder(float height, float radius, int sides, int stacks);
	void createCone(float height, float baseRadius, int sides);
	void createPawn();

private:

	void computeVAO(int numP, float *p, float *points, int sides, float smoothCos);
	void circularProfile(std::vector<float> &res, float minAngle, float maxAngle, float radius, int divisions, float transX= 0.0f, float transY = 0.0f);
	int revSmoothNormal2(float *p, float *nx, float *ny, float smoothCos, int beginEnd);
	float computeSegmentLength(float *p);
	float computeTotalSegmentLength(float *p, int numP);

};
#endif
