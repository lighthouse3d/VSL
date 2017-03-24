#include "vsGeometryLib.h"

#include <stdlib.h>
#include <vector>
#include <cmath>


/* -------------------------------------------------
				VSCartesianLib
------------------------------------------------- */

VSSurfRevLib VSCartesian::sCylinder;
VSSurfRevLib VSCartesian::sCone;
VSSurfRevLib VSCartesian::sSphere;


bool VSCartesian::sInit = false;


bool
VSCartesian::Init() {

	sCylinder.createCylinder(1.0f, 1.0f, 8, 8);
	sCone.createCone(2.0f, 1.0f, 8);
	sSphere.createSphere(1.0f, 8);
	sInit = true;
	return true;
}


VSCartesian::VSCartesian() {}


VSCartesian::~VSCartesian() {}



/* -------------------------------------------------
				POINT
------------------------------------------------- */


VSPoint::VSPoint() : mPosition(Point3(0.0f, 0.0f, 0.0f)), mRadius(0.01f) {

}


void
VSPoint::set(const Point3 &pos, float rad) {

	mPosition = pos;
	mRadius = rad;
	prepare();
}


void
VSPoint::setPosition(const Point3 &p) {
	
	mPosition = p;
	prepare();
}


void
VSPoint::setPosition(float *p) {
	
	mPosition.x = p[0];
	mPosition.y = p[1];
	mPosition.z = p[2];
	prepare();
}


void
VSPoint::setPosition(float x, float y, float z) {
	
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	prepare();
}


void
VSPoint::setRadius(float r) {

	mRadius = r;
	prepare();
}


const Point3 &
VSPoint::getPosition() {

	return mPosition;
}


float 
VSPoint::getRadius() {

	return mRadius;
}


void
VSPoint::prepare() {

	if (!sInit)
		Init();

	mMyMeshes.clear();
	addMeshes(sSphere);

	mMyMeshes[0].transform[0] = mRadius * 2;
	mMyMeshes[0].transform[1] = 0.0f;   
	mMyMeshes[0].transform[2] = 0.0f;   
	mMyMeshes[0].transform[3] = 0.0f;

	mMyMeshes[0].transform[4] = 0.0f;  
	mMyMeshes[0].transform[5] = mRadius * 2;
	mMyMeshes[0].transform[6] = 0.0f;  
	mMyMeshes[0].transform[7] = 0.0f;

	mMyMeshes[0].transform[8] = 0.0f;  
	mMyMeshes[0].transform[9] = 0.0f;   
	mMyMeshes[0].transform[10] = mRadius * 2;
	mMyMeshes[0].transform[11] = 0.0f;

	mMyMeshes[0].transform[12] = mPosition.x;  
	mMyMeshes[0].transform[13] = mPosition.y;  
	mMyMeshes[0].transform[14] = mPosition.z;  
	mMyMeshes[0].transform[15] = 1.0f;
}


/* -------------------------------------------------
				VECTOR
------------------------------------------------- */


VSVector::VSVector(): mFrom(Point3(0.0f, 0.0f, 0.0f)), mTo(Point3(1.0f, 1.0f, 1.0f)), mRadius(0.01f) { }


void 
VSVector::set(const Point3 &f, const Point3 &t, float r) {

	mFrom = f;
	mTo = t;
	mRadius = r;
	prepare();
}


void 
VSVector::setFrom(const Point3 &p) {

	mFrom = p;
	prepare();
}


void 
VSVector::setTo(const Point3 &p) {
	mTo = p;
	prepare();
}


void 
VSVector::setRadius(float r) {

	mRadius = r;
	prepare();
}


const Point3 &
VSVector::getFrom() {

	return mFrom;
}


const Point3 &
VSVector::getTo() {

	return mFrom;
}


float
VSVector::getRadius() {

	return mRadius;
}


void 
VSVector::prepare() {

	if (!sInit)
		Init();

	mMyMeshes.clear();

	float x[3] = { 1.0f, 0.0f, 0.0f }, y[3] = { 0.0f, 1.0f, 0.0f }, z[3] = { 0.0f, 0.0f, 1.0f };
	float dot[2];

	y[0] = mTo.x - mFrom.x;
	y[1] = mTo.y - mFrom.y;
	y[2] = mTo.z - mFrom.z;

	float length = mVSML->length(y) - mRadius * 6;

	dot[0] = mVSML->dotProduct(y, z);
	dot[1] = mVSML->dotProduct(y, x);

	if (fabs(dot[0]) < fabs(dot[1])) {
		mVSML->crossProduct(y, z, x);
		mVSML->crossProduct(x, y, z);
	}
	else {
		mVSML->crossProduct(x, y, z);
		mVSML->crossProduct(y, z, x);
	}

	mVSML->normalize(x);
	mVSML->normalize(y);
	mVSML->normalize(z);

	float transform[16];
	transform[0] = x[0];  transform[1] = x[1];   transform[2] = x[2];   transform[3] = 0.0f;
	transform[4] = y[0];  transform[5] = y[1];   transform[6] = y[2];   transform[7] = 0.0f;
	transform[8] = z[0];  transform[9] = z[1];   transform[10] = z[2];  transform[11] = 0.0f;
	transform[12] = 0.0f; transform[13] = 0.0f;  transform[14] = 0.0f;  transform[15] = 1.0f;

	mVSML->pushMatrix(VSMathLib::AUX0);
	mVSML->loadIdentity(VSMathLib::AUX0);
	mVSML->translate(VSMathLib::AUX0, mFrom.x, mFrom.y, mFrom.z);
	mVSML->multMatrix(VSMathLib::AUX0,transform);
	mVSML->scale(VSMathLib::AUX0, mRadius, length, mRadius);
	mVSML->translate(VSMathLib::AUX0, 0.0f, 0.5, 0.0f);

	addMeshes(sCylinder);
	memcpy(mMyMeshes[0].transform, mVSML->get(VSMathLib::AUX0), sizeof(float) * 16);


	mVSML->loadIdentity(VSMathLib::AUX0);
	mVSML->translate(VSMathLib::AUX0, mTo.x, mTo.y, mTo.z);
	mVSML->multMatrix(VSMathLib::AUX0, transform);
	mVSML->scale(VSMathLib::AUX0, 2 * mRadius, 3 * mRadius, 2 * mRadius);
	mVSML->translate(VSMathLib::AUX0, 0.0f, -2, 0.0f);

	addMeshes(sCone);
	memcpy(mMyMeshes[1].transform, mVSML->get(VSMathLib::AUX0), sizeof(float) * 16);


	mVSML->popMatrix(VSMathLib::AUX0);
}


/* -------------------------------------------------
				GRID
------------------------------------------------- */


VSGrid::VSGrid() : mAxis(Y), mMax(1.0f), mDivisions(10) {}


void
VSGrid::set(Axis ax, float max, float divisions) {
	
	mAxis = ax;
	mMax = max;
	mDivisions = divisions;

	prepare();
}


void
VSGrid::prepare() {

	std::vector<float> p;

	float step = mMax * 2.0f / mDivisions;
	float pos = -mMax;
	for (int i = 0; i < mDivisions + 1; ++i) {

		if (mAxis == Y) {
			p.push_back(pos); p.push_back(0.0f); p.push_back(-mMax); p.push_back(1.0f);
			p.push_back(pos); p.push_back(0.0f); p.push_back(mMax); p.push_back(1.0f);
		}
		else if (mAxis == X) {
			p.push_back(0.0f); p.push_back(pos); p.push_back(-mMax); p.push_back(1.0f);
			p.push_back(0.0f); p.push_back(pos); p.push_back(mMax); p.push_back(1.0f);
		}
		else  {
			p.push_back(pos); p.push_back(-mMax); p.push_back(0.0f); p.push_back(1.0f);
			p.push_back(pos); p.push_back(mMax); p.push_back(0.0f); p.push_back(1.0f);
		}
		pos += step;
	}

	pos = -mMax;
	for (int i = 0; i < mDivisions + 1; ++i) {

		if (mAxis == Y) {
			p.push_back(-mMax); p.push_back(0.0f); p.push_back(pos); p.push_back(1.0f);
			p.push_back(mMax); p.push_back(0.0f); p.push_back(pos); p.push_back(1.0f);
		}
		else if (mAxis == X) {
			p.push_back(0.0f); p.push_back(-mMax); p.push_back(pos); p.push_back(1.0f);
			p.push_back(0.0f); p.push_back(mMax); p.push_back(pos); p.push_back(1.0f);
		}
		else {
			p.push_back(-mMax); p.push_back(pos); p.push_back(0.0f); p.push_back(1.0f);
			p.push_back(mMax); p.push_back(pos); p.push_back(0.0f); p.push_back(1.0f);
		}
		pos += step;
	}

	MyMesh aMesh;
	buildVAO(aMesh, p.size()/4, &(p[0]), NULL, NULL, NULL, NULL, 0, NULL);

	memcpy(aMesh.transform, sIdentityMatrix, sizeof(float) * 16);
	aMesh.type = GL_LINES;

	mMyMeshes.clear();
	mMyMeshes.push_back(aMesh);
}


/* -------------------------------------------------
				AXIS
------------------------------------------------- */


VSAxis::VSAxis() : mLength(1.0f), mRadius(0.01f), mPositiveOnly(true) { }


void
VSAxis::set(float length, float radius, bool positiveOnly) {
	
	mLength = length;
	mRadius = radius;
	mPositiveOnly = positiveOnly;


	prepare();
}


void
VSAxis::prepare() {

	if (!sInit)
		Init();

	mMyMeshes.clear();

	Point3 origin(0.0f, 0.0f, 0.0f);
	Point3 px( mLength, 0.0f, 0.0f );
	Point3 py( 0.0f, mLength, 0.0f );
	Point3 pz( 0.0f, 0.0f,  mLength );

	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	float grey[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float greyA[4] = { 0.125f, 0.125f, 0.125f, 1.0f };

	if (mPositiveOnly)
		mX.set(origin, px);
	else
		mX.set(-px, px);
	addMeshes(mX);
	memcpy(mMyMeshes[0].mat.specular, white, sizeof(float) * 4);
	memcpy(mMyMeshes[0].mat.diffuse, red, sizeof(float) * 4);
	memcpy(mMyMeshes[0].mat.ambient, red, sizeof(float) * 4);

	memcpy(mMyMeshes[1].mat.specular, white, sizeof(float) * 4);
	memcpy(mMyMeshes[1].mat.diffuse, red, sizeof(float) * 4);
	memcpy(mMyMeshes[1].mat.ambient, red, sizeof(float) * 4);

	if (mPositiveOnly)
		mY.set(origin, py);
	else
		mY.set(-py, py);
	addMeshes(mY);
	memcpy(mMyMeshes[2].mat.specular, white, sizeof(float) * 4);
	memcpy(mMyMeshes[2].mat.diffuse, green, sizeof(float) * 4);
	memcpy(mMyMeshes[2].mat.ambient, green, sizeof(float) * 4);

	memcpy(mMyMeshes[3].mat.specular, white, sizeof(float) * 4);
	memcpy(mMyMeshes[3].mat.diffuse, green, sizeof(float) * 4);
	memcpy(mMyMeshes[3].mat.ambient, green, sizeof(float) * 4);

	if (mPositiveOnly)
		mZ.set(origin, pz);
	else
		mZ.set(-pz, pz);
	addMeshes(mZ);
	memcpy(mMyMeshes[4].mat.specular, white, sizeof(float) * 4);
	memcpy(mMyMeshes[4].mat.diffuse, blue, sizeof(float) * 4);
	memcpy(mMyMeshes[4].mat.ambient, blue, sizeof(float) * 4);

	memcpy(mMyMeshes[5].mat.specular, white, sizeof(float) * 4);
	memcpy(mMyMeshes[5].mat.diffuse, blue, sizeof(float) * 4);
	memcpy(mMyMeshes[5].mat.ambient, blue, sizeof(float) * 4);

	mP.set(origin);
	addMeshes(mP);
	memcpy(mMyMeshes[6].mat.specular, white, sizeof(float) * 4);
	memcpy(mMyMeshes[6].mat.diffuse, grey, sizeof(float) * 4);
	memcpy(mMyMeshes[6].mat.ambient, grey, sizeof(float) * 4);
}


/* -------------------------------------------------
				SIMPLE AXIS
------------------------------------------------- */


VSSimpleAxis::VSSimpleAxis(): mLength(1.0f) { }


void
VSSimpleAxis::set(float length) {
	
	mLength = length;
	prepare();
}


void
VSSimpleAxis::prepare() {

	const int arrayLen = 4 * 6;
	float p[arrayLen] = { -mLength,0,0,1, mLength,0,0,1,
		0, -mLength, 0, 1,  0, mLength, 0, 1,
		0, 0, -mLength, 1,  0, 0, mLength, 1 };

	MyMesh aMesh;
	buildVAO(aMesh, arrayLen / 4, &(p[0]), NULL, NULL, NULL, NULL, 0, NULL);

	memcpy(aMesh.transform, sIdentityMatrix, sizeof(float) * 16);
	aMesh.type = GL_LINES;
	mMyMeshes.push_back(aMesh);
}


/* -------------------------------------------------
				POLY LINE
------------------------------------------------- */


VSPolyLine::VSPolyLine() {}


void 
VSPolyLine::set(const std::vector<Point3> &polyLine, bool loop) {

	mPolyLine = polyLine;
	mLoop = loop;
	prepare();
}


void
VSPolyLine::prepare() {

	mMyMeshes.resize(1);

	std::vector<float> pp;
	for (unsigned int i = 0; i < mPolyLine.size(); ++i) {
		pp.push_back(mPolyLine[i].x); pp.push_back(mPolyLine[i].y); pp.push_back(mPolyLine[i].z);
		pp.push_back(1.0f);
	}
	if (mLoop) {
		pp.push_back(mPolyLine[0].x); pp.push_back(mPolyLine[0].y); pp.push_back(mPolyLine[0].z);
		pp.push_back(1.0f);
	}

	size_t numP = mPolyLine.size();
	if (mLoop)
		numP++;
	buildVAO(mMyMeshes[0], numP, (float *)&(pp[0]), NULL, NULL, NULL, NULL, 0, NULL);

	memcpy(mMyMeshes[0].transform, sIdentityMatrix, sizeof(float) * 16);
	mMyMeshes[0].type = GL_LINE_STRIP;
}


/* -------------------------------------------------
				DASHED_LINE
------------------------------------------------- */


VSDashedLine::VSDashedLine() : mFrom(Point3(0.0f, 0.0f, 0.0f)), mTo(Point3(0.0f, 0.0f, 0.0f)), mDashInterval(0.05f) { }


void
VSDashedLine::set(const Point3 &from, const Point3 &to, float dashInterval) {

	mFrom = from;
	mTo = to;
	mDashInterval = dashInterval;
	prepare();
}


void 
VSDashedLine::prepare() {

	std::vector<float> p;
	float len, res[3], aux[3];

	VSMathLib::subtract((float *)&(mFrom.x), (float *)&(mTo.x), res);
	len = VSMathLib::length(res);

	if (mDashInterval * 3.0 > len)
		mDashInterval = len / 3.0f;

	int divisions = (int)((len - mDashInterval) / mDashInterval);

	VSMathLib::normalize(res);
	res[0] = res[0] * mDashInterval;
	res[1] = res[1] * mDashInterval;
	res[2] = res[2] * mDashInterval;

	float f[3];
	memcpy(f, &(mFrom.x), sizeof(float) * 3);

	for (int i = 0; i < divisions; i += 2) {

		p.push_back(f[0]); p.push_back(f[1]); p.push_back(f[2]); p.push_back(1.0f);
		VSMathLib::add(f, res, aux);
		p.push_back(aux[0]); p.push_back(aux[1]); p.push_back(aux[2]); p.push_back(1.0f);
		VSMathLib::add(aux, res, f);
	}

	p.push_back(f[0]); p.push_back(f[1]); p.push_back(f[2]); p.push_back(1.0f);
	p.push_back(mTo.x); p.push_back(mTo.y); p.push_back(mTo.z); p.push_back(1.0f);

	MyMesh aMesh;
	buildVAO(aMesh, p.size() / 4, &(p[0]), NULL, NULL, NULL, NULL, 0, NULL);

	memcpy(aMesh.transform,sIdentityMatrix, sizeof(float) * 16);
	aMesh.type = GL_LINES;
	mMyMeshes.push_back(aMesh);
}


/* -------------------------------------------------
				DASHED_ARC
------------------------------------------------- */


VSDashedArc::VSDashedArc() : mAngle(45.0f), mRadius(1.0f), mDashInterval(0.05f) { }


void
VSDashedArc::set(float angle, float radius, float dashInterval) {

	mAngle = angle;
	mRadius = radius;
	mDashInterval = dashInterval;
	prepare();
}


void
VSDashedArc::prepare() {

	std::vector<float> p;
	float len, angRad;

	angRad = 2 * 3.14159f * mAngle / 360.0f;

	len = mRadius * angRad;
	if (mDashInterval * 3.0 > len)
		mDashInterval = len / 3.0f;

	int divisions = (int)((len - mDashInterval) / mDashInterval);
	int i = 0;
	float angStep = angRad / divisions;

	for (; i < divisions; i += 2) {

		p.push_back(mRadius * sin(angStep * i));	 p.push_back(0); p.push_back(mRadius * cos(angStep * i)); p.push_back(1.0f);
		p.push_back(mRadius * sin(angStep * (i + 1))); p.push_back(0); p.push_back(mRadius * cos(angStep * (i + 1))); p.push_back(1.0f);
	}

	p.push_back(mRadius * sin(angStep * (i - 1))); p.push_back(0); p.push_back(mRadius * cos(angStep * (i - 1))); p.push_back(1.0f);
	p.push_back(mRadius * sin(angRad)); p.push_back(0); p.push_back(mRadius * cos(angRad)); p.push_back(1.0f);

	mMyMeshes.resize(1);
	buildVAO(mMyMeshes[0], p.size() / 4, &(p[0]), NULL, NULL, NULL, NULL, 0, NULL);

	memcpy(mMyMeshes[0].transform, sIdentityMatrix, sizeof(float) * 16);
	mMyMeshes[0].type = GL_LINES;
}


// ---------------------------------------------------------------
//				Cubic Curve
// ---------------------------------------------------------------


float VSCubicCurve::sMatrix[2][16] = {
	{ -0.5f,  1.5f, -1.5f,  0.5f,  
	   1.0f, -2.5f,  2.0f, -0.5f,  
	  -0.5f,  0.0f,  0.5f,  0.0f,  
	   0.0f,  1.0f,  0.0f,  0.0f },

	{ -1.0f,  3.0f, -3.0f,  1.0f, 
	   3.0f, -6.0f,  3.0f,  0.0f, 
	  -3.0f,  3.0f,  0.0f,  0.0f, 
	   1.0f,  0.0f,  0.0f,  0.0f }
};


VSCubicCurve::VSCubicCurve() {

	mCtrlPts.resize(4);
	mCtrlPts[0].x = -1.0f; mCtrlPts[0].y = 0.0f; mCtrlPts[0].z = 0.0f;
	mCtrlPts[1].x = -0.5f; mCtrlPts[1].y = 1.0f; mCtrlPts[1].z = 0.0f;
	mCtrlPts[2].x =  0.5f; mCtrlPts[2].y = 1.0f; mCtrlPts[2].z = 0.0f;
	mCtrlPts[3].x =  1.0f; mCtrlPts[3].y = 0.0f; mCtrlPts[3].z = 0.0f;
	mType = CATMULL_ROM;
}


void
VSCubicCurve::set(const std::vector<Point3> &ctrlPts, unsigned int tessLevel, bool loop) {

	assert("At least 4 control points are requires" && ctrlPts.size() >= 4);

	mCtrlPts = ctrlPts;
	mTessLevel = tessLevel;
	mLoop = loop;
	prepare();
}


void
VSCubicCurve::setType(CurveType aType) {

	mType = aType;
	prepare();
}


void
VSCubicCurve::getPoint(float tt, Point3 &res) const {
	
	int start = (int)floor(tt);

	assert(mCtrlPts.size() < start + 4);
		
	float *m = sMatrix[mType];

	float t[4], resAux[4], aux[4];

	VSMathLib *vsml = VSMathLib::getInstance();

	vsml->pushMatrix(VSMathLib::AUX0);
	vsml->pushMatrix(VSMathLib::AUX1);
	vsml->loadMatrix(VSMathLib::AUX0, m);

	float cpM[16];
	buildControlPointMatrix(mCtrlPts[start], mCtrlPts[start+1], mCtrlPts[start+2], mCtrlPts[start+3], cpM);

	vsml->loadMatrix(VSMathLib::AUX1, cpM);

	t[3] = 1; t[2] = tt; t[1] = tt*tt; t[0] = tt*tt*tt;
	//note: VSMathLib stores matrices in column major mode
	vsml->multMatrixPoint(VSMathLib::AUX0, t, aux);
	vsml->multMatrixPoint(VSMathLib::AUX1, aux, resAux);

	vsml->popMatrix(VSMathLib::AUX0);
	vsml->popMatrix(VSMathLib::AUX1);

	res.x = resAux[0]; res.y = resAux[1]; res.z = resAux[2];
}


void
VSCubicCurve::getTangent(float tt, Point3 &res) const {

	int start = (int)floor(tt);

	assert(mCtrlPts.size() < start + 4);

	float *m = sMatrix[mType];

	float t[4], resAux[4], aux[4];

	VSMathLib *vsml = VSMathLib::getInstance();

	vsml->pushMatrix(VSMathLib::AUX0);
	vsml->pushMatrix(VSMathLib::AUX1);
	vsml->loadMatrix(VSMathLib::AUX0, m);

	float cpM[16];
	buildControlPointMatrix(mCtrlPts[start], mCtrlPts[start + 1], mCtrlPts[start + 2], mCtrlPts[start + 3], cpM);

	vsml->loadMatrix(VSMathLib::AUX1, cpM);

	t[3] = 0; t[2] = 1; t[1] = 2.0f * tt; t[0] = 3.0f*tt*tt;
	//note: VSMathLib stores matrices in column major mode
	vsml->multMatrixPoint(VSMathLib::AUX0, t, aux);
	vsml->multMatrixPoint(VSMathLib::AUX1, aux, resAux);

	vsml->popMatrix(VSMathLib::AUX0);
	vsml->popMatrix(VSMathLib::AUX1);

	res.x = resAux[0]; res.y = resAux[1]; res.z = resAux[2];
}


void
VSCubicCurve::prepare() {

	switch (mType) {
	case BEZIER:
		prepareCurve(4);
		mLoop = false;
		break;
	case CATMULL_ROM:
		prepareCurve((int)mCtrlPts.size());
	}
}


void
VSCubicCurve::prepareCurve(int numPts) {

	std::vector<float> p;
	float t[4];
	t[3] = 1;
	float *m = sMatrix[mType];


	size_t limit;
	if (mLoop)
		limit = mCtrlPts.size();
	else
		limit = mCtrlPts.size() - 3;
	size_t s = mCtrlPts.size();
	for (unsigned int n = 0; n < limit; ++n) {
		buildCurve(mCtrlPts[n%s], mCtrlPts[(n + 1) % s], mCtrlPts[(n + 2) % s], mCtrlPts[(n + 3) % s], p);
	}

	mMyMeshes.resize(1);
	buildVAO(mMyMeshes[0], p.size() / 4, &(p[0]), NULL, NULL, NULL, NULL, 0, NULL);

	memcpy(mMyMeshes[0].transform, sIdentityMatrix, sizeof(float) * 16);
	mMyMeshes[0].type = GL_LINE_STRIP;
}


void
VSCubicCurve::buildControlPointMatrix(const Point3 &cp0, const Point3 &cp1, const Point3 &cp2, const Point3 &cp3, float *res) const {

	float cpM[16] = {
		cp0.x, cp0.y, cp0.z, 1.0f,
		cp1.x, cp1.y, cp1.z, 1.0f,
		cp2.x, cp2.y, cp2.z, 1.0f,
		cp3.x, cp3.y, cp3.z, 1.0f
	};
	memcpy(res, cpM, sizeof(float) * 16);
}


void
VSCubicCurve::buildCurve(const Point3 &cp0, const Point3 &cp1, const Point3 &cp2, const Point3 &cp3, std::vector<float> &res) {

	float t[4], p[4];
	t[3] = 1.0f;

	VSMathLib *vsml = VSMathLib::getInstance();

	vsml->pushMatrix(VSMathLib::AUX1);
	vsml->pushMatrix(VSMathLib::AUX0);

	vsml->loadMatrix(VSMathLib::AUX0, sMatrix[mType]);

	float cpm[16];
	buildControlPointMatrix(cp0, cp1, cp2, cp3, cpm);
	vsml->loadMatrix(VSMathLib::AUX1, cpm);
	for (unsigned int i = 0; i < mTessLevel + 1; ++i) {

		t[2] = i * 1.0f / mTessLevel;
		t[1] = t[2] * t[2]; t[0] = t[2] * t[1];
		float aux[4];
		//note: VSMathLib stores matrices in column major mode
		vsml->multMatrixPoint(VSMathLib::AUX0, t, aux);
		vsml->multMatrixPoint(VSMathLib::AUX1, aux, p);

		res.push_back(p[0]); res.push_back(p[1]); res.push_back(p[2]); res.push_back(p[3]);
	}
	vsml->popMatrix(VSMathLib::AUX0);
	vsml->popMatrix(VSMathLib::AUX1);
}


/* -------------------------------------------------
				CUBIC BEZIER PATCH
------------------------------------------------- */


float VSCubicPatch::sM[16] = { -1,3,-3,1 , 3,-6,3,0 , -3,3,0,0 , 1,0,0,0 };


VSCubicPatch::VSCubicPatch(): mTessLevel(1), mInit(false) { }


void 
VSCubicPatch::set(const std::vector<Point3> &cp, unsigned int tessLevel) {

	assert("16 control points are required and tessellationlevel must be greater than zero" && cp.size() >= 16 && tessLevel > 0);

	mInit = true;

	mTessLevel = tessLevel;
	mCtrlPoints = cp;

	// copy points to control point matrices
	mX[0] = cp[0].x;	 mY[0] = cp[0].y;	mZ[0] = cp[0].z;
	mX[1] = cp[4].x;	 mY[1] = cp[4].y;	mZ[1] = cp[4].z;
	mX[2] = cp[8].x;	 mY[2] = cp[8].y;	mZ[2] = cp[8].z;
	mX[3] = cp[12].x;	 mY[3] = cp[12].y;	mZ[3] = cp[12].z;
						 					
	mX[4] = cp[1].x;	 mY[4] = cp[1].y;	mZ[4] = cp[1].z;
	mX[5] = cp[5].x;	 mY[5] = cp[5].y;	mZ[5] = cp[5].z;
	mX[6] = cp[9].x;	 mY[6] = cp[9].y;	mZ[6] = cp[9].z;
	mX[7] = cp[13].x;	 mY[7] = cp[13].y;	mZ[7] = cp[13].z;
						 				
	mX[8] = cp[2].x;	 mY[8] = cp[2].y;	mZ[8] = cp[2].z;
	mX[9] = cp[6].x;	 mY[9] = cp[6].y;	mZ[9] = cp[6].z;
	mX[10] = cp[10].x;	 mY[10] = cp[10].y;	mZ[10] = cp[10].z;
	mX[11] = cp[14].x;	 mY[11] = cp[14].y;	mZ[11] = cp[14].z;
						 				
	mX[12] = cp[3].x;	 mY[12] = cp[3].y;	mZ[12] = cp[3].z;
	mX[13] = cp[7].x;	 mY[13] = cp[7].y;	mZ[13] = cp[7].z;
	mX[14] = cp[11].x;	 mY[14] = cp[11].y;	mZ[14] = cp[11].z;
	mX[15] = cp[15].x;	 mY[15] = cp[15].y;	mZ[15] = cp[15].z;

	// compute the middle section of the patch expression
	VSMathLib *vsml = VSMathLib::getInstance();

	vsml->pushMatrix(VSMathLib::AUX0);
	vsml->pushMatrix(VSMathLib::AUX1);
	vsml->pushMatrix(VSMathLib::AUX2);

	vsml->loadMatrix(VSMathLib::AUX0, sM);
	vsml->multMatrix(VSMathLib::AUX0, mX);
	vsml->multMatrix(VSMathLib::AUX0, sM);

	vsml->loadMatrix(VSMathLib::AUX1, sM);
	vsml->multMatrix(VSMathLib::AUX1, mY);
	vsml->multMatrix(VSMathLib::AUX1, sM);

	vsml->loadMatrix(VSMathLib::AUX2, sM);
	vsml->multMatrix(VSMathLib::AUX2, mZ);
	vsml->multMatrix(VSMathLib::AUX2, sM);

	memcpy(mMX, vsml->get(VSMathLib::AUX0), sizeof(float)*16);
	memcpy(mMY, vsml->get(VSMathLib::AUX1), sizeof(float) * 16);
	memcpy(mMZ, vsml->get(VSMathLib::AUX2), sizeof(float) * 16);

	vsml->popMatrix(VSMathLib::AUX0);
	vsml->popMatrix(VSMathLib::AUX1);
	vsml->popMatrix(VSMathLib::AUX2);

	prepare();
}


void
VSCubicPatch::getPoint(float u, float v, Point3 &res) const {

	assert(mInit);

	VSMathLib *vsml = VSMathLib::getInstance();

	vsml->pushMatrix(VSMathLib::AUX0);
	vsml->pushMatrix(VSMathLib::AUX1);
	vsml->pushMatrix(VSMathLib::AUX2);

	vsml->loadMatrix(VSMathLib::AUX0, mMX);
	vsml->loadMatrix(VSMathLib::AUX1, mMY);
	vsml->loadMatrix(VSMathLib::AUX2, mMZ);

	float tu[4] = { u*u*u, u*u, u, 1 };
	float tv[4] = { v*v*v, v*v, v, 1 };

	float resX[4], resY[4], resZ[4];

	vsml->multPointMatrix(tu, VSMathLib::AUX0, resX);
	vsml->multPointMatrix(tu, VSMathLib::AUX1, resY);
	vsml->multPointMatrix(tu, VSMathLib::AUX2, resZ);

	res.x = resX[0] * tv[0] + resX[1] * tv[1] + resX[2] * tv[2] + resX[3] * tv[3];
	res.y = resY[0] * tv[0] + resY[1] * tv[1] + resY[2] * tv[2] + resY[3] * tv[3];
	res.z = resZ[0] * tv[0] + resZ[1] * tv[1] + resZ[2] * tv[2] + resZ[3] * tv[3];

	vsml->popMatrix(VSMathLib::AUX0);
	vsml->popMatrix(VSMathLib::AUX1);
	vsml->popMatrix(VSMathLib::AUX2);
}


void
VSCubicPatch::getNormal(float u, float v, Point3 &normal) const {

	Point3 t, bt;

	getTangent(u, v, t);
	getBiTangent(u, v, bt);

	VSMathLib::crossProduct(&t.x, &bt.x, &normal.x);
}


void
VSCubicPatch::getTangent(float u, float v, Point3 &tangent) const {

	assert(mInit);

	std::vector<float> n, tang, bitang, tc;
	std::vector<unsigned int> ind;

	VSMathLib *vsml = VSMathLib::getInstance();

	vsml->pushMatrix(VSMathLib::AUX0);
	vsml->pushMatrix(VSMathLib::AUX1);
	vsml->pushMatrix(VSMathLib::AUX2);

	vsml->loadMatrix(VSMathLib::AUX0, mMX);
	vsml->loadMatrix(VSMathLib::AUX1, mMY);
	vsml->loadMatrix(VSMathLib::AUX2, mMZ);

	float uderiv[4] = { 3*u*u, 2*u, 1, 0 };
	float vv[4] = { v*v*v, v*v, v, 1 };

	float resX[4], resY[4], resZ[4];

	vsml->multPointMatrix(uderiv, VSMathLib::AUX0, resX);
	vsml->multPointMatrix(uderiv, VSMathLib::AUX1, resY);
	vsml->multPointMatrix(uderiv, VSMathLib::AUX2, resZ);

	vsml->popMatrix(VSMathLib::AUX0);
	vsml->popMatrix(VSMathLib::AUX1);
	vsml->popMatrix(VSMathLib::AUX2);

	tangent.x = resX[0] * vv[0] + resX[1] * vv[1] + resX[2] * vv[2] + resX[3] * vv[3];
	tangent.y = resY[0] * vv[0] + resY[1] * vv[1] + resY[2] * vv[2] + resY[3] * vv[3];
	tangent.z = resZ[0] * vv[0] + resZ[1] * vv[1] + resZ[2] * vv[2] + resZ[3] * vv[3];
}


void
VSCubicPatch::getBiTangent(float u, float v, Point3 &bitangent) const {

	assert(mInit);

	std::vector<float> n, tang, bitang, tc;
	std::vector<unsigned int> ind;

	VSMathLib *vsml = VSMathLib::getInstance();

	vsml->pushMatrix(VSMathLib::AUX0);
	vsml->pushMatrix(VSMathLib::AUX1);
	vsml->pushMatrix(VSMathLib::AUX2);

	vsml->loadMatrix(VSMathLib::AUX0, mMX);
	vsml->loadMatrix(VSMathLib::AUX1, mMY);
	vsml->loadMatrix(VSMathLib::AUX2, mMZ);

	float uu[4] = { u*u*u, u*u, u, 1 };
	float vderiv[4] = { 3 * v*v, 2 * v, 1, 0 };

	float resX[4], resY[4], resZ[4];

	vsml->multPointMatrix(uu, VSMathLib::AUX0, resX);
	vsml->multPointMatrix(uu, VSMathLib::AUX1, resY);
	vsml->multPointMatrix(uu, VSMathLib::AUX2, resZ);

	vsml->popMatrix(VSMathLib::AUX0);
	vsml->popMatrix(VSMathLib::AUX1);
	vsml->popMatrix(VSMathLib::AUX2);

	bitangent.x = resX[0] * vderiv[0] + resX[1] * vderiv[1] + resX[2] * vderiv[2] + resX[3] * vderiv[3];
	bitangent.y = resY[0] * vderiv[0] + resY[1] * vderiv[1] + resY[2] * vderiv[2] + resY[3] * vderiv[3];
	bitangent.z = resZ[0] * vderiv[0] + resZ[1] * vderiv[1] + resZ[2] * vderiv[2] + resZ[3] * vderiv[3];
}


const std::vector<Point3> &
VSCubicPatch::getControlPoints() const{

	return mCtrlPoints;
}


void 
VSCubicPatch::prepare() {

	std::vector<float> p, n, tang, bitang, tc;
	std::vector<unsigned int> ind;

	float t;
	VSMathLib *vsml = VSMathLib::getInstance();

	vsml->pushMatrix(VSMathLib::AUX0);
	vsml->pushMatrix(VSMathLib::AUX1);
	vsml->pushMatrix(VSMathLib::AUX2);
	
	vsml->loadMatrix(VSMathLib::AUX0, mMX);
	vsml->loadMatrix(VSMathLib::AUX1, mMY);
	vsml->loadMatrix(VSMathLib::AUX2, mMZ);

	for (unsigned int i = 0; i < mTessLevel + 1; ++i) {

		t = i *1.0f / mTessLevel;
		float tu[4] = { t*t*t, t*t, t, 1 };
		float uderiv[4] = { 3 * t*t, 2 * t, 1, 0 };

		float resX[4], resY[4], resZ[4], resDerivX[4], resDerivY[4], resDerivZ[4];

		mVSML->multPointMatrix(tu, VSMathLib::AUX0, resX);
		mVSML->multPointMatrix(tu, VSMathLib::AUX1, resY);
		mVSML->multPointMatrix(tu, VSMathLib::AUX2, resZ);

		mVSML->multPointMatrix(uderiv, VSMathLib::AUX0, resDerivX);
		mVSML->multPointMatrix(uderiv, VSMathLib::AUX1, resDerivY);
		mVSML->multPointMatrix(uderiv, VSMathLib::AUX2, resDerivZ);

		for (unsigned int j = 0; j < mTessLevel + 1; ++j) {

			float v = j  *1.0f / mTessLevel;
			float tv[4] = { v*v*v, v*v, v, 1 };
			float vderiv[4] = { 3 * v*v, 2 * v, 1, 0 };
			float tangent[3], bitangent[3];

			p.push_back(resX[0] * tv[0] + resX[1] * tv[1] + resX[2] * tv[2] + resX[3] * tv[3]);
			p.push_back(resY[0] * tv[0] + resY[1] * tv[1] + resY[2] * tv[2] + resY[3] * tv[3]);
			p.push_back(resZ[0] * tv[0] + resZ[1] * tv[1] + resZ[2] * tv[2] + resZ[3] * tv[3]);
			p.push_back(1.0f);

			tangent[0] = resDerivX[0] * tv[0] + resDerivX[1] * tv[1] + resDerivX[2] * tv[2] + resDerivX[3] * tv[3];
			tangent[1] = resDerivY[0] * tv[0] + resDerivY[1] * tv[1] + resDerivY[2] * tv[2] + resDerivY[3] * tv[3];
			tangent[2] = resDerivZ[0] * tv[0] + resDerivZ[1] * tv[1] + resDerivZ[2] * tv[2] + resDerivZ[3] * tv[3];

			bitangent[0] = resX[0] * vderiv[0] + resX[1] * vderiv[1] + resX[2] * vderiv[2] + resX[3] * vderiv[3];
			bitangent[1] = resY[0] * vderiv[0] + resY[1] * vderiv[1] + resY[2] * vderiv[2] + resY[3] * vderiv[3];
			bitangent[2] = resZ[0] * vderiv[0] + resZ[1] * vderiv[1] + resZ[2] * vderiv[2] + resZ[3] * vderiv[3];

			float normal[3];
			VSMathLib::crossProduct(tangent, bitangent, normal);
			mVSML->normalize(normal); 
			mVSML->normalize(tangent);
			mVSML->normalize(bitangent);

			tang.push_back(tangent[0]); tang.push_back(tangent[1]); tang.push_back(tangent[2]); 
			bitang.push_back(bitangent[0]); bitang.push_back(bitangent[1]); bitang.push_back(bitangent[2]); 

			n.push_back(normal[0]); n.push_back(normal[1]); n.push_back(normal[2]); 

			tc.push_back(t); tc.push_back(v);
		}
	}

	mVSML->popMatrix(VSMathLib::AUX0);
	mVSML->popMatrix(VSMathLib::AUX1);
	mVSML->popMatrix(VSMathLib::AUX2);

	int div = mTessLevel + 1;
	for (unsigned int i = 0; i < mTessLevel; ++i) {
		for (unsigned int j = 0; j < mTessLevel; ++j) {


			ind.push_back(i*div + j + 1);
			ind.push_back(i*div + j);
			ind.push_back((i + 1)*div + j);

			ind.push_back(i*div + j + 1);
			ind.push_back((i + 1)*div + j);
			ind.push_back((i + 1)*div + j + 1);
		}
	}
	mMyMeshes.resize(1);
	buildVAO(mMyMeshes[0], p.size()/4, &(p[0]), &(n[0]), &(tc[0]), &(tang[0]), &(bitang[0]), ind.size(), &(ind[0]));

	memcpy(mMyMeshes[0].transform, sIdentityMatrix, sizeof(float) * 16);
	mMyMeshes[0].type = GL_TRIANGLES;
}


/* -------------------------------------------------
				GEOM CONSTRUCTION
------------------------------------------------- */


VSGeomConstruction::VSGeomConstruction() { }


void 
VSGeomConstruction::clear() {
	
	mMyMeshes.clear();
}


void 
VSGeomConstruction::cubicPatchConvexHull(const VSCubicPatch &cp) {

	const std::vector<Point3> &c = cp.getControlPoints();

	VSPolyLine pl;
	std::vector<Point3> p1 = { c[0], c[1], c[2], c[3] };
	pl.set(p1);
	addMeshes(pl);
	std::vector<Point3> p2 = { c[4], c[5], c[6], c[7] };
	pl.set(p2);
	addMeshes(pl);
	std::vector<Point3> p3 = { c[8], c[9], c[10], c[11] };
	pl.set(p3);
	addMeshes(pl);
	std::vector<Point3> p4 = { c[12], c[13], c[14], c[15] };
	pl.set(p4);
	addMeshes(pl);

	std::vector<Point3> p5 = { c[0], c[4], c[8], c[12] };
	pl.set(p5);
	addMeshes(pl);
	std::vector<Point3> p6 = { c[1], c[5], c[9], c[13] };
	pl.set(p6);
	addMeshes(pl);
	std::vector<Point3> p7 = { c[2], c[6], c[10], c[14] };
	pl.set(p7);
	addMeshes(pl);
	std::vector<Point3> p8 = { c[3], c[7], c[11], c[15] };
	pl.set(p8);
	addMeshes(pl);
}


void
VSGeomConstruction::cubicPatchNormalField(const VSCubicPatch &cp, unsigned int div, float size) {

	Point3 origin, dest, normal;
	VSVector v;

	unsigned int d = div + 1;
	for (unsigned int i = 0; i <= d; ++i) {
		for (unsigned int j = 0; j <= d; ++j) {
			cp.getPoint(i*1.0f / d, j*1.0f/d, origin);
			cp.getNormal(i*1.0f / d, j*1.0f / d, normal);
			if (size > 0.0f)
				normal *= (size/VSMathLib::length(&normal.x));
			dest = origin + normal;
			v.set(origin, dest, 0.01f * size);
			addMeshes(v);
		}
	}
}

void
VSGeomConstruction::projectPointXZ(const Point3 &point,float size) {

/*	float pXZ[3] = {0.0f, 0.0f, 0.0f};
	float pX[3] = {0.0f, 0.0f, 0.0f};
	float pZ[3] = {0.0f, 0.0f, 0.0f};
	float o[3] = {0.0f, 0.0f, 0.0f};
	std::vector<float> p;

	pX[0] = point.x; pZ[2] = point.z;
	pXZ[0] = point.x; pXZ[2] = point.z;

	createDashedLineAux(point, pXZ, p, size);
	createDashedLineAux(pXZ,pX, p, size);
	createDashedLineAux(pXZ,pZ, p, size);
	createDashedLineAux(pXZ, o, p, size);

	MyMesh aMesh;
	buildVAO(aMesh, p.size()/4, &(p[0]), NULL, NULL, NULL, NULL, 0, NULL);

	memcpy(aMesh.transform, sIdentityMatrix, sizeof(float) * 16);
	aMesh.type = GL_LINES;
	mMyMeshes.push_back(aMesh);
*/}



