//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
//
//	CIFXNodeBaseEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXNodeBaseEncoder.
//		The CIFXNodeBaseEncoder contains common node encoding functionality 
//		that is used by the individual specific node encoders 
//		(e.g. CIFXLightEncoder).
//
//	NOTES: This class is intended to be used as an abstract base class
//		   for various types of node encoders. As such, both the 
//		   constructor and destructor are declared as protected members. 
//		   This does, in itself, suffice in keeping a stand-alone instance 
//		   of this class from being created. Note that the destructor is 
//		   also declared as pure virtual to further enforce the abstract 
//		   nature of this class - but the destructor does still have an 
//		   implementation.
//
//*****************************************************************************


#include "CIFXNodeBaseEncoder.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXPalette.h"
#include "IFXMatrix4x4.h"


CIFXNodeBaseEncoder::CIFXNodeBaseEncoder()
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pNode = NULL;
	m_pSceneGraph = NULL;
}

CIFXNodeBaseEncoder::~CIFXNodeBaseEncoder()
{
	IFXRELEASE(m_pBitStream);
	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pNode);
	IFXRELEASE(m_pSceneGraph);
}

void CIFXNodeBaseEncoder::CommonNodeEncodeU3D(IFXString& rName, F64 units)
{
	IFXUnknown*	pUnknown = NULL;
	IFXPalette*	pNodePalette = NULL;
	IFXString sParentNodeName;

	try
	{
		// check for initialization
		if (FALSE == m_bInitialized)
			throw IFXException(IFX_E_NOT_INITIALIZED);
		// check to see if the node was set
		if (NULL == m_pNode)
			throw IFXException(IFX_E_CANNOT_FIND);
		if (units <= 0.0f)
			throw IFXException(IFX_E_INVALID_RANGE);

		// 1. Encode the node name
		m_pBitStream->WriteIFXStringX(rName);

		// 2. Encode number of parents
		U32 nParentCount = m_pNode->GetNumberOfParents();
		m_pBitStream->WriteU32X(nParentCount);

		IFXCHECKX(m_pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette));

		F32 invScale = 1.0f/(F32)units;
		U32 i;
		for ( i = 0; i < nParentCount; i++) 
		{
			// 2.1. Encode parent node name
			U32 index = 0;
			IFXNode* pParentNodeNR = m_pNode->GetParentNR(i);
			sParentNodeName.Assign(L"");
			if (pParentNodeNR) 
			{
				IFXCHECKX(pParentNodeNR->QueryInterface(IID_IFXUnknown, (void**)&pUnknown));

				IFXCHECKX(pNodePalette->FindByResourcePtr(pUnknown, &index));
				IFXCHECKX(pNodePalette->GetName(index, &sParentNodeName));

				IFXRELEASE(pUnknown);
			}
			m_pBitStream->WriteIFXStringX(sParentNodeName);

			// 4. Encode the matrix
			IFXMatrix4x4 matrix = m_pNode->GetMatrix(i);

			if(units != 1.0)
			{
				// apply units scaling factor to translation part

				matrix[12] *= invScale;
				matrix[13] *= invScale;
				matrix[14] *= invScale;
			}

			U32 j;
			for ( j = 0; j < 16; j++)
				m_pBitStream->WriteF32X(matrix[j]);
		}

		IFXRELEASE(pNodePalette);
	}
	catch (...)
	{
		IFXRELEASE(pUnknown);
		IFXRELEASE(pNodePalette);

		throw;
	}
}

void CIFXNodeBaseEncoder::Initialize(IFXCoreServices& rCoreServices)
{
	try
	{
		// latch onto the core services object passed in
		IFXRELEASE(m_pCoreServices)
		m_pCoreServices = &rCoreServices;
		m_pCoreServices->AddRef();

		// get pointer to scenegraph
		IFXRELEASE(m_pSceneGraph);
		IFXCHECKX(m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph, (void**)&m_pSceneGraph));

		// create a bitstream
		IFXRELEASE(m_pBitStream);
		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStream));

		m_bInitialized = TRUE;
	}
	catch (...)
	{
		IFXRELEASE(m_pCoreServices);
		IFXRELEASE(m_pSceneGraph);
		IFXRELEASE(m_pBitStream);
		throw;
	}
}

void CIFXNodeBaseEncoder::SetNode(IFXNode& rNode)
{
	try
	{
		IFXRELEASE(m_pNode);
		m_pNode = &rNode;
		m_pNode->AddRef();
	}
	catch (...)
	{
		throw;
	}
}
