/*
 * Connect3D - Reconstruction of water-tight triangulated meshes from unstructured point clouds
 *
 * please credit the following article: Stefan Ohrhallinger, Sudhir Mudur and Michael Wimmer:
 * 'Minimizing Edge Length to Connect Sparsely Sampled Unstructured Point Sets',
 * Shape Modeling International 2013, published in Computers & Graphics Journal, 2013.
 *
 * Copyright (C) 2013 Stefan Ohrhallinger, Daniel Prieler
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "precompiled.h"

#include "c3dMath.h"


/*
 * compares 2 points lexicographically (to their coordinate), return p0 < p1
 * for the case of 2 equal points (not handled), false is returned
 */
bool comparePoints(const Vector3D &p0, const Vector3D &p1)
{
	return (p0[0] < p1[0]) ||
		((p0[0] == p1[0]) && (p0[1] < p1[1])) ||
		((p0[0] == p1[0]) && (p0[1] == p1[1]) && (p0[2] < p1[2]));
}



/*
 * sort points lexicographically to make sure that for 3 given
 * points the circumcenter is always the same
 */
Vector3D calcCircumcenterDet(Vector3D p0, Vector3D p1, Vector3D p2)
{
	// sort points lexicographically (to their coordinate)
	if (comparePoints(p0, p1))
	{
		Vector3D temp = p0;
		p0 = p1;
		p1 = temp;
	}

	if (comparePoints(p0, p2))
	{
		Vector3D temp = p0;
		p0 = p2;
		p2 = temp;
	}

	if (comparePoints(p1, p2))
	{
		Vector3D temp = p1;
		p1 = p2;
		p2 = temp;
	}

	return calcCircumcenter(p0, p1, p2);
}


/*
 * sort points lexicographically to make sure that for 3 given
 * points the circumcenter is always the same
 */
float calcCircumcenterRadiusDet(const Vector3D &p0, const Vector3D &p1, const Vector3D &p2, Vector3D &cc)
{
	cc = calcCircumcenterDet(p0, p1, p2);

	return Vector3D::distance(cc, p0);
}

/*****************************************************************************/
/*  from comp.graphics.algorithms                                            */
/*  tricircumcenter3d()   Find the circumcenter of a triangle in 3D.         */
/*                                                                           */
/*  The result is returned both in terms of xyz coordinates and xi-eta       */
/*  coordinates, relative to the triangle's point `a' (that is, `a' is       */
/*  the origin of both coordinate systems).  Hence, the xyz coordinates      */
/*  returned are NOT absolute; one must add the coordinates of `a' to        */
/*  find the absolute coordinates of the circumcircle.  However, this means  */
/*  that the result is frequently more accurate than would be possible if    */
/*  absolute coordinates were returned, due to limited floating-point        */
/*  precision.  In general, the circumradius can be computed much more       */
/*  accurately.                                                              */
/*                                                                           */
/*****************************************************************************/
Vector3D calcCircumcenter(const Vector3D &a, const Vector3D &b, const Vector3D &c)
{
	Vector3D center;
	double xba, yba, zba, xca, yca, zca;
	double balength, calength;
	double xcrossbc, ycrossbc, zcrossbc;
	double denominator;
	double xcirca, ycirca, zcirca;

	/* Use coordinates relative to point `a' of the triangle. */
	xba = b[0] - a[0];
	yba = b[1] - a[1];
	zba = b[2] - a[2];
	xca = c[0] - a[0];
	yca = c[1] - a[1];
	zca = c[2] - a[2];
	/* Squares of lengths of the edges incident to `a'. */
	balength = xba * xba + yba * yba + zba * zba;
	calength = xca * xca + yca * yca + zca * zca;

	/* Cross product of these edges. */
	xcrossbc = yba * zca - yca * zba;
	ycrossbc = zba * xca - zca * xba;
	zcrossbc = xba * yca - xca * yba;

	double div = xcrossbc * xcrossbc + ycrossbc * ycrossbc + zcrossbc * zcrossbc;

	/* Calculate the denominator of the formulae. */
	if (div == 0.0)
		denominator = 0.0;
	else
		denominator = 0.5 / div;

	/* Calculate offset (from `a') of circumcenter. */
	xcirca = ((balength * yca - calength * yba) * zcrossbc -
			(balength * zca - calength * zba) * ycrossbc) * denominator;
	ycirca = ((balength * zca - calength * zba) * xcrossbc -
			(balength * xca - calength * xba) * zcrossbc) * denominator;
	zcirca = ((balength * xca - calength * xba) * ycrossbc -
			(balength * yca - calength * yba) * xcrossbc) * denominator;
	center[0] = a[0] + static_cast<float>(xcirca);
	center[1] = a[1] + static_cast<float>(ycirca);
	center[2] = a[2] + static_cast<float>(zcirca);

	return center;
}

/*
 * assures to return a valid float value (not NaN) for comparison
 */
double acosSafe(double x)
{
	if (x > 1.0)
		x = 1.0;
	else
	if (x < -1.0)
		x = -1.0;

	return acos(x);
}


/*
 * returns the dihedral angle (in form of cos angle) so that angles > 90 degrees are negative
 * between triangle #0 (v0-v1-v2) and triangle #1 (v1-v2-v3) across their shared edge v1-v2
 * does not consider direction, cos angle -1..+1 (angles between 0..180 degrees)
 */
float dihedralAngle(const Vector3D &v0, const Vector3D &v1, const Vector3D &v2, const Vector3D &v3)
{
	Vector3D n0 = Vector3D::crossProduct(v1 - v0, v2 - v0);
	Vector3D n1 = Vector3D::crossProduct(v2 - v3, v1 - v3);

	n0.normalize();
	n1.normalize();

	float angle = Vector3D::dotProduct(n0, n1);

	return angle;
}

/*
 * calculate triangle area
 */
float calcTriangleArea(const Vector3D &p0, const Vector3D &p1, const Vector3D &p2)
{
	return 0.5f * Vector3D::distance(p0, p1) * Vector3D::distance(p0, p2);
}


/*
 * calculate triangle aspect ratio
 */
float calcTriangleAspectRatio(const Vector3D &p0, const Vector3D &p1, const Vector3D &p2)
{
	float semiperimeter = calcTriangleSemiPerimeter(p0, p1, p2);
	float inradius = calcTriangleArea(p0, p1, p2)/semiperimeter;
	Vector3D cc;
	float circumradius = calcCircumcenterRadiusDet(p0, p1, p2, cc);

	return 0.5f*circumradius/inradius;
}


/*
 * calculate triangle semi-perimeter
 */
float calcTriangleSemiPerimeter(const Vector3D &p0, const Vector3D &p1, const Vector3D &p2)
{
	return 0.5f * (Vector3D::distance(p0, p1) + Vector3D::distance(p1, p2) + Vector3D::distance(p2, p0));
}