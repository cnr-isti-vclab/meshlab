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

/**
	@file	IFXScreenSpaceMetric.h

			Implements Hoppe's Screen Space Metric (SIGGRAPH 1997)
			Modified for subdivision triangles.
*/

#ifndef IFXSREENSPACEMETRIC_DOT_H
#define IFXSREENSPACEMETRIC_DOT_H

#include "IFXScreenSpaceMetricInterface.h"
#include "IFXAdaptiveMetric.h"
#include "IFXTQTTriangle.h"

class IFXTQTTriangle;


class IFXScreenSpaceMetric : public IFXScreenSpaceMetricInterface
{
private:

    IFXVector3  m_eye;
  IFXVector3  m_xdir, m_ydir, m_zdir;

    F32         m_fConstant;
    F32         m_fPixelTolerance;

  F32         m_fFOV;

  IFXVector3  m_frustum[4];
  F32         m_frustumdist[4];
  IFXVector3  m_frustumEdge[4];

  BOOL        m_bLockViewpoint;
  F32         m_fModelSize;
  IFXVector3  m_fModelCenter;

  void FindTriangleCenter(IFXVector3 &center, IFXTQTVertex **ppVertex);

  // Evaluate Hoppe's screen space geometric error equation
  void EvaluateGeometricError(IFXTQTTriangle *pTriangle,
    IFXTQTVertex **ppVertex, IFXVector3 &faceNormal, F32 delsquared,
    F32 *pLeftmetric, F32 *pRightmetric);

  // Use "tipup" or "tipdown" orientation to compute face normal
  bool GetFaceNormal(IFXTQTTriangle *pTriangle, IFXTQTVertex **ppVertex,
    IFXVector3 &faceNormal);

  // Determine whether a triangle should be subdivided, consolidated, or sustained
  IFXAdaptiveMetric::Action ScreenSpace(IFXTQTTriangle *pTriangle,
      IFXTQTVertex **ppVertex);
public:

    IFXScreenSpaceMetric();

    virtual void EvaluateTriangle (IFXTQTTriangle *pTriangle, Action *pAction);

    void IFXAPI   UpdateViewpoint (IFXVector3 &pCameraX, IFXVector3 &pCameraY,
    IFXVector3 &pCameraZ, IFXVector3 &pCameraPos, F32 fAspect, F32 fFieldOfView);

	F32 IFXAPI   GetPixelTolerance();
	void IFXAPI   SetPixelTolerance(F32 factor);

	F32 IFXAPI   GetFieldofView();
	void IFXAPI   SetFieldofView(F32 degrees);

	BOOL IFXAPI   GetViewpointLock();
	void IFXAPI   SetViewpointLock(BOOL state);

};

#endif
