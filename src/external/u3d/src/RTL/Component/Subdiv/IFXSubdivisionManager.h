
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
	@file	IFXSubdivisionManager.h

			Class IFXSubdivisionManager implements the interface defined by class
			IFXSubdivisionManagerInterface.
*/


#ifndef IFXSUBDIVISIONMANAGER_DOT_H
#define IFXSUBDIVISIONMANAGER_DOT_H

// Subdivision specific includes:
#include "IFXSubdivManagerInterface.h"
#include "IFXTQTTriangleAllocator.h"
#include "IFXVertexAllocator.h"
#include "IFXTQTBaseTriangle.h"
#include "IFXAdaptiveMetric.h"
#include "IFXButterflyScheme.h"


class IFXSubdivisionManager : public IFXSubdivisionManagerInterface
{

public:

    // Constructors and destructors:
    IFXSubdivisionManager();
    virtual ~IFXSubdivisionManager();

    // Property accessors:
    IFXRESULT  IFXAPI 	 SetBool       (BooleanProperty,   BOOL bValue);
    IFXRESULT  IFXAPI 	 GetBool       (BooleanProperty,   BOOL *pbValue);
    IFXRESULT  IFXAPI 	 SetInteger    (IntegerProperty,   U32 uValue);
    IFXRESULT  IFXAPI 	 GetInteger    (IntegerProperty,   U32 *puValue);
    IFXRESULT  IFXAPI 	 SetFloat      (FloatProperty,   F32 fValue);
    IFXRESULT  IFXAPI 	 GetFloat      (FloatProperty,   F32 *pfValue);

    // Subdivision interface methods:
    IFXRESULT  IFXAPI 	 ConsolidateLevel (U32 uLevel);
    IFXRESULT  IFXAPI 	 ResetAll();
    IFXRESULT  IFXAPI 	 SetAdaptiveMetric (IFXAdaptiveMetric *pInterface);
    IFXRESULT  IFXAPI 	 GetAdaptiveMetric (IFXAdaptiveMetric **ppMetric);
    IFXRESULT  IFXAPI 	 InitMesh      (IFXMeshGroup   *pMeshGrp,
                               IFXNeighborMesh* pNeighborMesh );

  // copy some or all of the vertex data with a vertexIter
  void      CopyVertexData  (IFXMeshGroup *pMeshGroup);

  IFXRESULT  IFXAPI 	 UpdateMesh    (IFXMeshGroup **pOutMeshGrp, BOOL *pUpdated);

    static const U32  m_uMaxBaseMeshSize;

private:

    friend class IFXTQTTriangle;

    // Exposed/user settable properties:
    U32     m_puInteger  [MAX_NUM_INTEGER_PROPERTIES];
    F32     m_pfFloat    [MAX_NUM_FLOAT_PROPERTIES];
    BOOL    m_pbBoolean  [MAX_NUM_BOOLEAN_PROPERTIES];

    // Constants:
    static const F32  m_fMaxSurfaceTensionParam;
    static const F32  m_fMinSurfaceTensionParam;

    IFXAdaptiveMetric *m_pAdaptiveMetric;

    // The output mesh:
    IFXMeshGroup    *m_pOutputMeshGrp;

    // Unexposed/private properties:
    IFXTQTBaseTriangle  *m_pBaseTriangle;
  IFXTQTVertex      **m_ppBaseVertex;
    U32           m_uNumBaseTriangles;
  U32           m_uNumBaseVertices;

    // Free Lists:
    IFXTQTTriangleAllocator     *m_pTriangleAllocator;
    IFXVertexAllocator          *m_pVertexAllocator;

  // OutputMesh allocation mgt:
  F32 m_uOutMshInitial_ratio;
  U32 m_numOutVertsIncr;
  U32 m_numOutFacesIncr;
  U32 m_uNumOutMeshes;
  U32*m_pOutMshVtxCnt;

    // The subdivision scheme implementation:
    IFXButterflyScheme          *m_pButterflyScheme;

    // Internal flags:
    BOOL                        m_bDontUpdate;
  BOOL            m_bUpdateRequired;
  BOOL            m_bReallocateOutputMesh;

  // Implementation methods:
    void SetDefaultProperties();
    BOOL CheckRangeAndScaling (FloatProperty property,  F32 *pfValue);
    void Update();
  IFXRESULT AllocateFreeLists();
  IFXRESULT DeallocateFreeLists();
    IFXRESULT AllocateOutputMesh(IFXMeshGroup *pInputMeshGrp);
  IFXRESULT ReallocateOutputMesh ();
    IFXRESULT DeallocateOutputMesh();
    IFXRESULT AddRenderTriangle(IFXTQTTriangle *pTriangle);
    IFXRESULT AddRenderCrackFillTriangle(IFXTQTTriangle *pTriangle,
    IFXTQTVertex *pCorner[3],
    IFXTQTTriangle::SametOrientation bSametOrientation);
    IFXButterflyScheme *GetSubdivisionScheme();
  void AllocateBaseMesh (IFXMeshGroup *pMeshGrp, U32 *pFaceOffsetTable);
  void ResetOutputMeshGroup (IFXMeshGroup   *pMeshGrp);
};


IFXINLINE IFXRESULT IFXSubdivisionManager::GetAdaptiveMetric (IFXAdaptiveMetric **ppMetric)
{
    *ppMetric = this->m_pAdaptiveMetric;

    return IFX_OK;
}

#endif
