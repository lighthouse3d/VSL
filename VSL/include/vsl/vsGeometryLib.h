/** ----------------------------------------------------------
 * \VSGeometryLib
 *
 * Lighthouse3D
 *
 * VSGeometryLib - Very Simple Geometry Library
 *
 * \version 0.1.0
 *		Initial Release
 *
 * This lib is composed of a set of classes that generate geometric primitives
 *
 * This lib requires the following classes from VSL:
 * (http://www.lighthouse3d.com/very-simple-libs)
 *
 * VSResourceLib
 * VSModelLib
 * VSMathLib 
 * VSLogLib
 * VSShaderLib
 *
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

 
#ifndef __VSGeometryLib__
#define __VSGeometryLib__

#include "vslConfig.h"

#include <string>
#include <vector>
#include <assert.h>

#include "vsSurfRevLib.h"


 /* -------------------------------------------------
				VSCartesian
 ------------------------------------------------- */

class VSCartesian : public VSModelLib {

public:
	~VSCartesian();

protected:
	static bool Init();
	static bool sInit;

	static VSSurfRevLib sCylinder, sCone, sSphere;

	VSCartesian();
};


/* -------------------------------------------------
				Point3
------------------------------------------------- */

class Point3 {

public:
	float x, y, z;

	Point3(float *p) {
		x = p[0];
		y = p[1];
		z = p[2];
	};

	Point3(float xx, float yy, float zz) {
		x = xx;
		y = yy;
		z = zz;
	};

	const Point3 & operator += (const Point3 &p) {
		x += p.x;
		y += p.y;
		z += p.z;
		return *this;
	};

	const Point3 & operator /= (float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	};

	const Point3 & operator *= (float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	};

	Point3 operator+ (const Point3 &p) {
		Point3 res;
		res.x = x + p.x;
		res.y = y + p.y;
		res.z = z + p.z;
		return res;
	};

	Point3 operator- (const Point3 &p) {
		Point3 res;
		res.x = x - p.x;
		res.y = y - p.y;
		res.z = z - p.z;
		return res;
	};

	Point3 operator- () {
		return Point3(-x, -y, -z);
	};

	Point3() {
		x = 0.0f; y = 0.0f; z = 0.0f;
	};
};


/* -------------------------------------------------
				POINT
------------------------------------------------- */


class VSPoint : public VSCartesian {

public:
	VSPoint();

	void set(const Point3 &p, float radius = 0.01f);

	void setPosition(const Point3 &p);
	void setPosition(float *p);
	void setPosition(float x, float y, float z);

	void setRadius(float rad);

	const Point3 &getPosition();
	float getRadius();

protected:
	Point3 mPosition;
	float mRadius;

	void prepare();

};


/* -------------------------------------------------
				VECTOR
------------------------------------------------- */


class VSVector : public VSCartesian {

public:
	VSVector();

	void set(const Point3 &from, const Point3 &to, float radius = 0.01);
	void setFrom(const Point3 &p);
	void setTo(const Point3 &p);

	void setRadius(float radius);

	const Point3 &getFrom();
	const Point3 &getTo();

	float getRadius();

protected:
	Point3 mFrom, mTo;
	float mRadius;

	void prepare();
};


/* -------------------------------------------------
				AXIS
------------------------------------------------- */


class VSAxis : public VSCartesian {

public:

	VSAxis();

	void set(float length = 1.0f, float radius = 0.01f, bool positiveOnly = true);

protected:
	VSVector mX, mY, mZ;
	VSPoint mP;
	float mLength;
	float mRadius;
	bool mPositiveOnly;

	void prepare();
};


/* -------------------------------------------------
				GRID
------------------------------------------------- */


class VSGrid : public VSModelLib {

public:
	enum Axis {
		X, Y, Z
	};

	VSGrid();

	void set(Axis ax, float max = 1.0, float divisions = 10);

protected:
	float mMax;
	float mDivisions;
	Axis mAxis;

	void prepare();
};


/* -------------------------------------------------
				SIMPLE AXIS
------------------------------------------------- */


class VSSimpleAxis : public VSModelLib {

public:

	VSSimpleAxis();

	void set(float length = 1.0f);

protected:
	float mLength;

	void prepare();
};



/* -------------------------------------------------
				POLY LINE
------------------------------------------------- */

class VSPolyLine : public VSModelLib {

public:

	VSPolyLine();

	void set(const std::vector<Point3> &polyLine, bool mLoop = false);

protected:
	std::vector<Point3> mPolyLine;
	bool mLoop;

	void prepare();
};


/* -------------------------------------------------
				DASHED_LINE
------------------------------------------------- */


class VSDashedLine : public VSModelLib {

public:

	VSDashedLine();

	void set(const Point3 &from, const Point3 &to, float dashInterval = 0.05f);

protected:
	Point3 mFrom, mTo;
	float mDashInterval;

	void prepare();
};


/* -------------------------------------------------
				DASHED_ARC
------------------------------------------------- */

class VSDashedArc : public VSModelLib {

public:

	VSDashedArc();

	void set(float angleDegrees, float radius, float dashInterval = 0.05f);

protected:
	float mAngle, mRadius;
	float mDashInterval;

	void prepare();

};


/* -------------------------------------------------
				CUBIC CURVE
------------------------------------------------- */

class VSCubicCurve : public VSModelLib {

public:

	enum CurveType {
		CATMULL_ROM,
		BEZIER
	};

	VSCubicCurve();

	void set(const std::vector<Point3> &ctrlPts, unsigned int tessLevel, bool loop = false);
	void setType(CurveType c);

	void getPoint(float t, Point3 &res) const;
	void getTangent(float t, Point3 &res) const;

protected:

	static float sMatrix[2][16];

	float mCPM[16];
	std::vector<Point3> mCtrlPts;
	CurveType mType;
	unsigned int mTessLevel;
	bool mLoop;

	void prepare();
	void prepareCurve(int numPts);
	void buildControlPointMatrix(const Point3 &cp0, const Point3 &cp1, const Point3 &cp2, const Point3 &cp3, float *res) const ;
	void buildCurve(const Point3 &cp0, const Point3 &cp1, const Point3 &cp2, const Point3 &cp3, std::vector<float> &res);
};


/* -------------------------------------------------
				CUBIC PATCH
------------------------------------------------- */


class VSCubicPatch : public VSModelLib {

public:
	VSCubicPatch();

	void set(const std::vector<Point3> &ctrlPts, unsigned int tessLevel);
	void getPoint(float u, float v, Point3 &res) const;
	void getNormal(float u, float v, Point3 &res) const;
	void getTangent(float u, float v, Point3 &res) const;
	void getBiTangent(float u, float v, Point3 &res) const;

	const std::vector<Point3> &getControlPoints() const;

protected:
	static float sM[16];

	std::vector<Point3> mCtrlPoints;
	float mX[16], mY[16], mZ[16];
	float mMX[16], mMY[16], mMZ[16];
	unsigned int mTessLevel;
	bool mInit;

	void prepare();
};


/* -------------------------------------------------
				GEOM CONSTRUCTION
------------------------------------------------- */


class VSGeomConstruction : public VSModelLib {

public:
	VSGeomConstruction();

	void clear();
	void projectPointXZ(const Point3 &p, float size = 0.05f);
	void cubicPatchConvexHull(const VSCubicPatch &cp);
	void cubicPatchNormalField(const VSCubicPatch &cp, unsigned int div, float size = 1.0);

protected:
};

	




#endif
