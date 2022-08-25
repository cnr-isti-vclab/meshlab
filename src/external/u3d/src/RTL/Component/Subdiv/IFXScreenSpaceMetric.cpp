//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

#include <math.h>
#include "IFXMatrix4x4.h"
#include "IFXScreenSpaceMetric.h"
#include "IFXTQTBaseTriangle.h"

// back-facing triangles may have subdivided children that are front-facting.
// so allow some back-facing triangles to be subdivided by increasing this value
// these are cos(theta in radians) values, so
// 0.3 is about 72 degrees, 0.8 is about 36 degrees. 0 degrees (straight back) is 1.
#define SLIGHT_BACKFACE 0.2f
#define VERY_BACKFACE 0.5f

#define SCREENSPACE_THRESHOLD 0.9f

// minimum pixel tolerance
#define MIN_PIXTOL 0.1f

// for converting from degrees
#define RADIANS 0.01745329f


//*****************************************************************************
IFXScreenSpaceMetric::IFXScreenSpaceMetric()
{
	// initialize a random eye position
	m_eye.Set(0.0f, 0.0f, 0.0f);

	// initialize other variables
	m_xdir.Reset();
	m_ydir.Reset();
	m_zdir.Reset();


	m_fPixelTolerance = 1.0f; // default pixel tolerance
	m_fModelSize = 500;    // default model bounding box diagonal length

	SetFieldofView(30);
	SetPixelTolerance(1000);

	m_fModelCenter.Reset();
	m_bLockViewpoint = FALSE;

}

//*****************************************************************************
// Three stage metric:  1) Frustum, 2) Back-Facing, 3) screen-space geometric error
void IFXScreenSpaceMetric::EvaluateTriangle (IFXTQTTriangle *pTriangle, Action *pAction)
{
	/**
	// Hack a test case:
	IFXTQTBaseTriangle *pBaseTriangle = pTriangle->GetBaseTriangle();

	if (pBaseTriangle->m_usId == 31)
	*pAction = IFXAdaptiveMetric::Subdivide;
	else
	*pAction = IFXAdaptiveMetric::Sustain;

	return;
	**/

	IFXTQTVertex *ppVertex[3];
	pTriangle->GetVertices (&ppVertex[IFXTQTAddress::Left],
		&ppVertex[IFXTQTAddress::Base],
		&ppVertex[IFXTQTAddress::Right]);

	//--------------------------------------------------------
	// First Metric Stage:  consolidate triangles outside frustum

	// check each vertex of each triangle to see if in view frustum
	bool visible = true;
	int tight_visibility[3] = {0,0,0};
	//cycle through the four frustum planes
	int j;
	for( j=0;j<4;j++) {
		int count = 0;
		IFXVector3 *plane = &m_frustum[j];
		// cycle through the three points of the triangle
		int i;
		for( i=0; i<3; i++){
			// count the number of vertices inside the plane
			F32 dotprod = ppVertex[i]->m_position.DotProduct(*plane);
			if (dotprod >= m_frustumdist[j]) {
				count++;
				// if the triangle is _well_ inside the frustum, take note
				if (0.95 * dotprod >= m_frustumdist[j]) tight_visibility[i]++;
			} /* if */
		}
		// if all three vertices are on the wrong side of the plane, it's not visible.
		if (count==0) {
			visible = false; //bail, object is not visible
			break;
		} /* if */
	} /* for */
	//--------------------------------------------------------

	// apply rest of metric to triangles inside frustum
	if (visible) {
		*pAction = ScreenSpace(pTriangle,ppVertex);
		// if the triangle is near the frustum boundary, sustain instead of
		// subdivide.  If the sustain was an "else" clause of visibility,
		// you would not get the "consolidate" decisions, and
		// thus would sustain triangles unnecessarily

		// this test says:  if not even one vertex is well within all four planes, sustain
		if ((*pAction == IFXAdaptiveMetric::Subdivide) &&
			((tight_visibility[0] < 4) && (tight_visibility[1] < 4) &&
			(tight_visibility[2] < 4)))
			*pAction = IFXAdaptiveMetric::Sustain;
	}
	else
		*pAction = IFXAdaptiveMetric::Consolidate;
}

//*****************************************************************************
// Use the Samet orientation to compute the face normal
bool IFXScreenSpaceMetric::GetFaceNormal(IFXTQTTriangle *pTriangle, IFXTQTVertex **ppVertex,
										 IFXVector3 &faceNormal)
{
	// Get two unit vecs, u and v, then compute face normal:
	IFXVector3 u, v;
	v.Subtract(ppVertex[IFXTQTAddress::Right]->m_position,
		ppVertex[IFXTQTAddress::Base]->m_position);
	u.Subtract(ppVertex[IFXTQTAddress::Left]->m_position,
		ppVertex[IFXTQTAddress::Base]->m_position);

	// it is unnecessary to normalize v and u before the cross-product
	if (pTriangle->GetSametOrientation() == IFXTQTTriangle::TipUp)
		faceNormal.CrossProduct(v,u);
	else
		faceNormal.CrossProduct(u,v);

	// Guard against bogus normals, zero area faces, and 0-distances:
	if (faceNormal.X() == 0.0 && faceNormal.Y() == 0.0 && faceNormal.Z() == 0.0) {
		return false;
	}
	faceNormal.Normalize();
	return true;
}

//*****************************************************************************
// Average vertices together to find a centroid:
void IFXScreenSpaceMetric::FindTriangleCenter(IFXVector3 &center, IFXTQTVertex **ppVertex) {
	center.X() = (ppVertex[IFXTQTAddress::Left]->m_position.X() +
		ppVertex[IFXTQTAddress::Base]->m_position.X() +
		ppVertex[IFXTQTAddress::Right]->m_position.X()) * 0.333333333f;
	center.Y() = (ppVertex[IFXTQTAddress::Left]->m_position.Y() +
		ppVertex[IFXTQTAddress::Base]->m_position.Y() +
		ppVertex[IFXTQTAddress::Right]->m_position.Y()) * 0.333333333f;
	center.Z() = (ppVertex[IFXTQTAddress::Left]->m_position.Z() +
		ppVertex[IFXTQTAddress::Base]->m_position.Z() +
		ppVertex[IFXTQTAddress::Right]->m_position.Z()) * 0.333333333f;
}

//*****************************************************************************
// Evaluate Hoppe's screen space geometric error equation
// this could be inlined, but the overhead doesn't appear to be significant.
void IFXScreenSpaceMetric::EvaluateGeometricError(IFXTQTTriangle *pTriangle,
												  IFXTQTVertex **ppVertex,
												  IFXVector3 &faceNormal, F32 delsquared,
												  F32 *pLeftmetric, F32 *pRightmetric)
{
	// Average vertices together to find a centroid:
	IFXVector3 center;
	FindTriangleCenter(center,ppVertex);

	// DO NOT normalize vecMinusEye (used for normcmpnt and distcmpnt)
	IFXVector3 vecMinusEye;
	vecMinusEye.Subtract(center, m_eye);

	// get the distance and normal components of the equation
	F32 distcmpnt = vecMinusEye.CalcMagnitudeSquared();

	//--------------------------------------------------------------------------------------
	// find mesh boundaries
	IFXTQTAddress address, nbraddress;
	pTriangle->GetAddress(&address);

	IFXTQTAddress::Direction meshBoundary = IFXTQTAddress::UnDefined;

	IFXTQTAddress::Direction direction, orientation;
	IFXTQTBaseTriangle *pBaseNeighbor = 0;

	// check each edge to see if it is a mesh boundary.
	// if the neighbor address calculation faults in the direction of the edge,
	// then there might be a distal neighbor.  Check the base mesh for a neighbor
	// in that direction.  If no neighbor, then mesh boundary.
	for (I32 i = 0; i < 3; i++) {
		direction = IFXTQTAddress::Direction(i);
		if ((IFXTQTAddress::UnDefined == meshBoundary) &&
			(address.LocalNeighbor(direction,&nbraddress))) {
				pTriangle->GetBaseTriangle()->GetBaseNeighbor(direction,&pBaseNeighbor,&orientation);
				if (!pBaseNeighbor) meshBoundary = direction;
			}
	}

	// use the length of the boundary edge as the Hoppe quality measure "mu squared"
	/// @todo: find the angle this edge boundary forms with its neighbor...
	if (IFXTQTAddress::UnDefined != meshBoundary) {
		IFXVector3 edge;
		IFXTQTAddress::Direction endpoint1 = (IFXTQTAddress::Right==meshBoundary)?IFXTQTAddress::Base:IFXTQTAddress::Right;
		IFXTQTAddress::Direction endpoint2 = (IFXTQTAddress::Left==meshBoundary)?IFXTQTAddress::Base:IFXTQTAddress::Left;
		edge.Subtract(
			ppVertex[endpoint1]->m_position,
			ppVertex[endpoint2]->m_position);
		*pLeftmetric = edge.CalcMagnitude();
		*pLeftmetric *= *pLeftmetric / m_fModelSize;

		*pRightmetric = m_fConstant * distcmpnt;
		return;
	}
	//--------------------------------------------------------------------------------------

	F32 normcmpnt = vecMinusEye.DotProduct(faceNormal);
	normcmpnt *= normcmpnt;

	// left and right sides of the equation
	*pLeftmetric  = delsquared * (distcmpnt - normcmpnt);
	*pRightmetric = m_fConstant * distcmpnt * distcmpnt;
}

//*****************************************************************************
// Screen-space metric.  The trick is getting a good delta.
IFXAdaptiveMetric::Action IFXScreenSpaceMetric::ScreenSpace(IFXTQTTriangle *pTriangle,
															IFXTQTVertex **ppVertex)
{

	//---------------------------------------------------------
	// Find face normal for screen space and back-facing stages
	IFXVector3 faceNormal;
	if (!GetFaceNormal(pTriangle,ppVertex,faceNormal))
		return IFXAdaptiveMetric::Sustain;
	//---------------------------------------------------------

	//=========================================================
	// Second Metric Stage: consolidate back-facing triangles

	// Note that a sustain is needed for triangles that are
	// nearly visible to prevent flicker.  Especially important
	// because these will be silhouette triangles.
	// Note2 :  actually, with access to the parent triangle,
	// the sustain isn't as necessary.

	F32 backface = -m_zdir.DotProduct(faceNormal);
	IFXASSERT( (backface > -1.001) && (backface < 1.001));
	IFXAdaptiveMetric::Action returnvalue = IFXAdaptiveMetric::Subdivide;

	if (backface > SLIGHT_BACKFACE) {
		if (backface > VERY_BACKFACE) {
			IFXTQTTriangle *pParent = pTriangle->GetParentTriangle();
			if (pParent) {
				// geometric error doesn't work for a truly back-facing triangle.
				// so, only consolidate this back-facing triangle if the parent was also
				// back-facing, otherwise we will thrash.
				pParent->GetVertices (&ppVertex[IFXTQTAddress::Left],
					&ppVertex[IFXTQTAddress::Base],
					&ppVertex[IFXTQTAddress::Right]);
				if (!GetFaceNormal(pParent,ppVertex,faceNormal))
					return IFXAdaptiveMetric::Sustain;

				backface = -m_zdir.DotProduct(faceNormal);
				// if the parent was back-facing too, then consolidate it.
				if (backface > SLIGHT_BACKFACE)
					return IFXAdaptiveMetric::Consolidate;
				// fail the above check and fall through to "sustain" below.
			} // end if parent
		} // end if very backfacing triangle
		// Instead of returning sustain here (thereby sustaining every slightly backfacing triangle)
		// allow the geometric error evaluation an opportunity to consolidate it.
		returnvalue = IFXAdaptiveMetric::Sustain;
	}
	//---------------------------------------------------------

	//=========================================================
	// Third Metric Stage:  evaluate Hoppe's metric.

	//---------------------------------------------------------
	// calculate "delta," the deviation of this triangle from its potential
	F32 delsquared = pTriangle->GetErrorMeasure();

	// see if the triangle already has a defined error term.
	// the error can't be negative, so if it is it must not be initialized
	if (delsquared < 0) {
		delsquared = 0;

		/// @todo: these should already be normalized?
		ppVertex[IFXTQTAddress::Left]->m_normal.Normalize();
		ppVertex[IFXTQTAddress::Right]->m_normal.Normalize();
		ppVertex[IFXTQTAddress::Base]->m_normal.Normalize();

		IFXVector3 center;
		FindTriangleCenter(center,ppVertex);

		IFXVector3 crnrvect;
		F32 hypotenuse, cosangle;

		// use the difference of the vertex normals from the face normal,
		// and the distance of the vertex from the center, to predict error
		for (I32 i = 0; i < 3; i++) {
			IFXTQTVertex *pVertex = ppVertex[IFXTQTAddress::Direction(i)];

			cosangle = faceNormal.DotProduct(pVertex->m_normal);
			if (cosangle > 0) {
				crnrvect.Subtract(pVertex->m_position,center);
				hypotenuse = crnrvect.CalcMagnitude() / cosangle;
				crnrvect.CrossProduct(faceNormal,pVertex->m_normal);
				delsquared += hypotenuse * crnrvect.CalcMagnitude();
			}
		}

		// normalize the cumulative error
		delsquared *= 0.33333333333f;

		// remember this error term for future evaluation
		pTriangle->SetErrorMeasure(delsquared);
	}
	//---------------------------------------------------------

	//===================================================================
	// This is the heart of the metric: the geometric error calculation
	F32 leftmetric = 0, rightmetric = 0;
	EvaluateGeometricError(pTriangle,ppVertex,faceNormal,delsquared,&leftmetric,&rightmetric);
	//===================================================================

	//---------------------------------------------------------
	// Now, make thresholds for subdivide, sustain, or consolidate

	// if the left side exceeds the right, the triangle has
	// too much "geometric error" and should be subdivided.
	if (leftmetric >= rightmetric)

		// note that if the triangle failed the "slightly backfacing" test
		// above, then the best we can do is sustain.
		return returnvalue;

	// Sustain threshold.
	// as this parameter -> 1, metric becomes unstable
	// however, if it too small, too many subdivisions are preserved.
	else if (leftmetric >= (SCREENSPACE_THRESHOLD * rightmetric))
		return IFXAdaptiveMetric::Sustain;

	else { // the triangle is a consolidation candidate.
		// Only consolidate if the parent does not need to be subdivided,
		// so evaluate the metric with the parent triangle.
		IFXTQTTriangle *pParent = pTriangle->GetParentTriangle();
		if (pParent) {
			delsquared = pParent->GetErrorMeasure();

			pParent->GetVertices (&ppVertex[IFXTQTAddress::Left],
				&ppVertex[IFXTQTAddress::Base], &ppVertex[IFXTQTAddress::Right]);
			if (!GetFaceNormal(pParent,ppVertex,faceNormal))
				return IFXAdaptiveMetric::Sustain;

			EvaluateGeometricError(pParent,ppVertex,faceNormal,delsquared,&leftmetric,&rightmetric);

			if (leftmetric < rightmetric) {
				return IFXAdaptiveMetric::Consolidate;
			}
		}
		return IFXAdaptiveMetric::Sustain;
	}
}

//*****************************************************************************
// Each frame, update the viewpoint one time.  Must re-compute the view frustum
// for the first stage of the metric.
void IFXScreenSpaceMetric::UpdateViewpoint (IFXVector3 &pCameraX, IFXVector3 &pCameraY,
											IFXVector3 &pCameraZ, IFXVector3 &pCameraPos, F32 fAspect, F32 fFieldOfView)
{

	// if the viewpoint has been locked, don't change eye position
	if (!m_bLockViewpoint) {
		m_xdir = pCameraX;
		m_ydir = pCameraY;
		m_zdir = pCameraZ;
		m_eye  = pCameraPos;

		// the camera may not be normalized
		m_xdir.Normalize();
		m_ydir.Normalize();
		m_zdir.Normalize();
	}


	// compute frustum plane normals
	IFXMatrix4x4 rotation;
	F32 hfovy; // half the y fov in radians
	F32 hfovx; // half the x fov in radians

	hfovy = fFieldOfView * RADIANS * 0.5f;
	hfovx = hfovy * fAspect;

	IFXVector4 fn[4];  // normals for frustum planes
	IFXVector4 x,y,z;

	// lame conversion
	x.X() = m_xdir.Value(0);
	x.Y() = m_xdir.Value(1);
	x.Z() = m_xdir.Value(2);
	y.X() = m_ydir.Value(0);
	y.Y() = m_ydir.Value(1);
	y.Z() = m_ydir.Value(2);
	z.X() = m_zdir.Value(0);
	z.Y() = m_zdir.Value(1);
	z.Z() = m_zdir.Value(2);

	rotation.RotateAxis(x, hfovy);  // create rotation matrix that rotates about x axis by hfovy radians.
	fn[0] = rotation * y;  // rotate the up vector to get the normal for the top frustum plane.

	rotation.Reset();
	rotation.RotateAxis(x, -hfovy);
	fn[1] = rotation * y;   // normal for bottom frustum plane.
	fn[1] = fn[1].Scale(-1);

	rotation.Reset();
	rotation.RotateAxis(y, -hfovx);
	fn[2] = rotation * x; // rotate the x axis vector to get the normal for the right frustum plane.

	rotation.Reset();
	rotation.RotateAxis(y, hfovx);
	fn[3] = rotation * x;
	fn[3] = fn[3].Scale(-1);

	m_fFOV = fFieldOfView * RADIANS;

	int i;
	for (i = 0; i<4; i++) {
		m_frustum[i].Value(0) = -fn[i].X();
		m_frustum[i].Value(1) = -fn[i].Y();
		m_frustum[i].Value(2) = -fn[i].Z();
	}

	for (i =0; i<4; i++) {
		m_frustumdist[i] = m_eye.DotProduct(m_frustum[i]);
	}
}

//*****************************************************************************
void IFXScreenSpaceMetric::SetPixelTolerance(F32 factor) {
	if (0 == factor) factor = 1;
	m_fPixelTolerance = MIN_PIXTOL / factor;

	// k squared = sqr(2*cot(fov/2) * tolerance)
	F32 fDLambda = (float) tan((double)(m_fFOV * 0.5f));
	if (0 == fDLambda) fDLambda = 1;
	m_fConstant  = 4.0f * (m_fPixelTolerance * m_fPixelTolerance) /
		(fDLambda * fDLambda);
}

F32 IFXScreenSpaceMetric::GetPixelTolerance() {
	if (m_fPixelTolerance)
		return ((MIN_PIXTOL/m_fPixelTolerance));
	else
		return 1;
}

// convert degrees to radians
void IFXScreenSpaceMetric::SetFieldofView(F32 degrees) {
	m_fFOV = degrees * RADIANS;

	// recompute pixel tolerance, it's modified by field of view
	SetPixelTolerance(GetPixelTolerance());
}

// convert radians to degrees
F32 IFXScreenSpaceMetric::GetFieldofView() {
	return m_fFOV / RADIANS;
}

void IFXScreenSpaceMetric::SetViewpointLock(BOOL state) {
	m_bLockViewpoint = state;
}

BOOL IFXScreenSpaceMetric::GetViewpointLock() {
	return m_bLockViewpoint;
}
