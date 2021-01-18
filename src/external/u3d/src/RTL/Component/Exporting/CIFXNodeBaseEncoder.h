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
//	CIFXNodeBaseEncoder.h
//
//	DESCRIPTION:
//		Declaration of the CIFXNodeBaseEncoder.
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

#ifndef CIFXNODEBASEENCODER_H__
#define CIFXNODEBASEENCODER_H__


// no ordering requirements for include files 
#include "IFXBitStreamX.h"
#include "IFXCoreServices.h"
#include "IFXDataBlockQueueX.h"
#include "IFXSceneGraph.h"
#include "IFXNode.h"


class  CIFXNodeBaseEncoder 
{
protected:
	// methods
	CIFXNodeBaseEncoder();
	virtual ~CIFXNodeBaseEncoder() = 0;

	void CommonNodeEncodeU3D( IFXString& rName, F64 units ); // use m_pBitStream 
	
	// Initialize only local members - it's up to each individual derived class
	// to initialize their class-specific members and thus should override this 
	// method with their own specific version. However, the overridden version 
	// should still call this base class method.
	void Initialize( IFXCoreServices& rCoreServices ); 
	
	void SetNode( IFXNode& rNode );


	// members
	BOOL				m_bInitialized;
	IFXBitStreamX*		m_pBitStream;
	IFXCoreServices*	m_pCoreServices;
	IFXNode*			m_pNode;  
	IFXSceneGraph*		m_pSceneGraph;
};

#endif
