//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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

#ifndef _COCTREENODE_H_
#define _COCTREENODE_H_

#include "IFXObserver.h"
#include "IFXSpatial.h"
#include "IFXSpatialSetQuery.h"

#include "CIFXSubject.h"
#include "CArrayList.h"
#include "CIFXOctreeCollection.h"

#include "IFXSceneGraph.h"
#include "IFXNode.h"
#include "IFXAutoRelease.h"

class CIFXOctreeCuller;
class CIFXOctreePicker;
class CIFXOctreeCollisionCuller;


const int MAX_CHILD_X = 2;
const int MAX_CHILD_Y = 2;
const int MAX_CHILD_Z = 2;
const int MAX_NUM_CHILDREN = 8;

class COctreeNode : public IFXObserver{
	SPATIALINSTANCE_LIST* m_pDataBuffer;
	U32 m_uRefCount;

public : // for now
	// Size of the Octant
	// needed for our new client based change manager
	CIFXOctreeCollection* m_pTree;
	
	COctreeNode*  m_pParent;

	U32 m_depth;
    
	COctreeNode*  m_pChildren[MAX_CHILD_X][MAX_CHILD_Y][MAX_CHILD_Z];
	// Octant Center
	IFXVector4 m_center;
	// no of Objects Contained in the Octree node
	U32 m_objectCount;

	// Speed up VF culls.
	//  U32 m_nodeMask;
	friend class CIFXOctreeCollection;
	friend class CIFXOctreeCuller;
	friend class CIFXOctreePicker;
	friend class CIFXOctreeCollisionCuller;

	U32 IFXAPI  AddRef (void)
	{
		return ++m_uRefCount;
	}

	U32 IFXAPI  Release (void)
	{
		if (--m_uRefCount == 0)
		{
			delete this;
			return 0;
		}
		return m_uRefCount;
	}

	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID riid, void **ppv)
	{
		IFXRESULT iResult = IFX_E_UNSUPPORTED;
		if (IID_IFXUnknown == riid)
		{
			AddRef ();
			*ppv = (IFXUnknown *) this;
			iResult = IFX_OK;
		}
		return iResult;
	}

	// Constructors
	COctreeNode(CIFXOctreeCollection * oTree, COctreeNode* p,
		U32 d, float cx, float cy, float cz, float sz):
		m_pTree(oTree),
		m_pParent(p),
		m_depth(d),
		m_center(cx,cy,cz,sz)            
	{
		Initialize();
	}

	COctreeNode(CIFXOctreeCollection * oTree, COctreeNode* p, U32 d, IFXVector4 c)
		:m_pTree(oTree),m_pParent(p),m_depth(d),m_center(c)
	{
		Initialize();
	}

	virtual ~COctreeNode()
	{
		int x,y,z,listCnt;
		U32 iter;
		IFXSubject * pSubject=0;
		IFXSpatial * pSpatial=0;
		// IFXAPI  Release  all the Spatials in the Octant before Destoying the Octant
		for(listCnt=0;listCnt<(I32)IFXSpatial::TYPE_COUNT;listCnt++)
		{
			if(!m_pDataBuffer[listCnt].empty())
			{

				for(iter=m_pDataBuffer[listCnt].begin();iter!=m_pDataBuffer[listCnt].end();++iter)
				{
					pSpatial = m_pDataBuffer[listCnt][iter].m_pSpatial;

					U32 i;
					BOOL bDetach = TRUE;
					for(i=iter+1;i!=m_pDataBuffer[listCnt].end();++i) {
						if (pSpatial == m_pDataBuffer[listCnt][i].m_pSpatial) {
							bDetach = FALSE;
							break;
						}
					}

					if (bDetach) {
						pSpatial->QueryInterface(IID_IFXSubject,(void **)&pSubject);
						pSubject->Detach((IFXObserver *)this);
					}
				}

				m_pDataBuffer[listCnt].clear();
			}
		}
		for ( x = 0; x < MAX_CHILD_X; x++)
			for ( y = 0; y < MAX_CHILD_Y; y++)
				for ( z = 0; z < MAX_CHILD_Z; z++)
					if (m_pChildren[x][y][z])
					{
						delete m_pChildren[x][y][z];
					}
					IFXDELETE_ARRAY(m_pDataBuffer);
	}

	void Initialize(){
		int x,y,z;
		m_objectCount = 0;

		for ( x = 0; x < MAX_CHILD_X; x++)
			for ( y = 0; y < MAX_CHILD_Y; y++)
				for ( z = 0; z < MAX_CHILD_Z; z++)
					m_pChildren[x][y][z] = 0;

		m_pDataBuffer = new SPATIALINSTANCE_LIST[IFXSpatial::TYPE_COUNT];
	}

	BOOL IsInstanceExist(IFXSpatial *pSpatial, U32 Instance)
	{
		U32 index;
		IFXSpatialInstance SpatialInstance;

		SpatialInstance.m_pSpatial = pSpatial;
		SpatialInstance.m_Instance = Instance;

		return m_pDataBuffer[pSpatial->GetSpatialType()].find(SpatialInstance, index);
	}

	IFXRESULT IFXAPI 	Update( IFXSubject * subject, U32 changedBits, IFXREFIID rIType )
	{
		IFXRESULT result = IFX_OK;
		//IFXVector4 o;
		IFXDECLARELOCAL(IFXSpatial, pSpatial);
		U32 numInstances = 0;
		IFXDECLARELOCAL(IFXNode, pSpatialNode);

		result = subject->QueryInterface( IID_IFXSpatial, (void**)&pSpatial );
		if( IFXSUCCESS(result) )
			result = pSpatial->QueryInterface(IID_IFXNode, (void**)&pSpatialNode);
		if(IFXSUCCESS(result))
			result = pSpatialNode->GetNumberOfInstances(&numInstances);
 
		if( changedBits == IFXSpatial::EOL)
		{
			IFXSceneGraph *pSceneGraph = NULL;
			if (IFXSUCCESS(result))
				result = pSpatialNode->GetSceneGraph(&pSceneGraph);

			if (IFXSUCCESS(result) && NULL != pSceneGraph) {
				IFXRELEASE(pSceneGraph);

				U32 i, spatialType = pSpatial->GetSpatialType();
				U32 begin = m_pDataBuffer[spatialType].begin(),
					end = m_pDataBuffer[spatialType].end();
				for (i = begin; i != end && IFXSUCCESS(result); ++i) {
					if (m_pDataBuffer[spatialType][i].m_pSpatial == pSpatial)
						result = RemoveObjectFromNode(pSpatial, m_pDataBuffer[spatialType][i].m_Instance, spatialType); 
				}
			} else {
				U32 listCnt, iter;
				result = IFX_OK;
				for (listCnt = 0; listCnt < IFXSpatial::TYPE_COUNT && IFXSUCCESS(result); listCnt++) {
					if (!m_pDataBuffer[listCnt].empty()) {
						for(iter=m_pDataBuffer[listCnt].begin();iter!=m_pDataBuffer[listCnt].end() && IFXSUCCESS(result);++iter)
						{
							U32 i;
							BOOL bDetach = TRUE;
							for(i=iter+1;i!=m_pDataBuffer[listCnt].end();++i) {
								if (m_pDataBuffer[listCnt][iter].m_pSpatial == m_pDataBuffer[listCnt][i].m_pSpatial) {
									bDetach = FALSE;
									break;
								}
							}

							if (bDetach) {
								IFXDECLARELOCAL(IFXSubject, pSubject);
								result = (m_pDataBuffer[listCnt])[iter].m_pSpatial->QueryInterface(IID_IFXSubject,(void **)&pSubject);
								if (IFXSUCCESS(result))
									result = pSubject->Detach((IFXObserver *)this);
							}
						}
						m_pDataBuffer[listCnt].clear();
					}
				}
			}
		}
		/** @todo: let's investigate and fix this part of code which most likely 
				   should be used to replace node from one Octant to another.*/
		/*else
		{
			U32 i;
			for (i = 0; i < numInstances && IFXSUCCESS(result); i++) {
				result = pSpatial->GetSpatialBound(o, i);
				// remove model Subject from Model List;
				if (IFXSUCCESS(result) && 
					(m_pParent) &&
					( o.X() - o.Radius() < m_center.X() - m_center.Radius() ||
					o.Y() - o.Radius() < m_center.Y() - m_center.Radius() ||
					o.Y() + o.Radius() > m_center.Y() + m_center.Radius() ||
					o.Z() - o.Radius() < m_center.Z() - m_center.Radius() ||
					o.Z() + o.Radius() > m_center.Z() + m_center.Radius() ))
				{
					if (IsInstanceExist(pSpatial, i))
					{
						// Object moved to another node
						result = RemoveObjectFromNode( pSpatial, i, pSpatial->GetSpatialType() ); 
						// Overloaded notify to inform tree to reinsert model1
						if (IFXSUCCESS(result))
							m_pTree->Insert(m_pParent, pSpatial, i); 
						// Write up reInsert instead to inserts objects moving to
						// a higher position in the tree determined by min depth
					}
				}
			}
		}*/
		return result;
	}


	U32 GetNodeDepth() const { return m_depth;}

	void InsertObjectInNode( IFXSpatial * o,U32 spatialType, U32 Instance)
	{
		// Check for validity of spatialType
		U32 i =0;
		BOOL bAttach = TRUE;
		if((spatialType != IFXSpatial::VIEW)&&(spatialType != IFXSpatial::GROUP))
		{
			for (i=m_pDataBuffer[spatialType].begin();i!=m_pDataBuffer[spatialType].end();++i) {
				if ((m_pDataBuffer[spatialType])[i].m_pSpatial == o) {
					bAttach = FALSE;
					break;
				}
			}

			IFXSpatialInstance SpatialInstance;
			SpatialInstance.m_pSpatial = o;
			SpatialInstance.m_Instance = Instance;
			m_pDataBuffer[spatialType].push_back(SpatialInstance);

			if (bAttach)
				((IFXSubject*)o)->Attach((IFXObserver *) this, 
											IFXSpatial::POSITION | IFXSpatial::EOL,
											IID_IFXSpatial);
			m_objectCount++;
		}
	}

	IFXRESULT RemoveObjectFromNode(IFXSpatial * o, I32 Instance, U32 spatialType)
	{
		IFXRESULT iResult = IFX_OK;
		BOOL bDetach;
		U32 i, j;

		IFXDECLARELOCAL(IFXSubject, pSubject);
		o->QueryInterface(IID_IFXSubject,(void **)&pSubject);

		if (Instance == -1) {
			U32 begin, end;
			begin = m_pDataBuffer[spatialType].begin();
			end = m_pDataBuffer[spatialType].end();
			/**@todo: Develop it a little better. 
			There is no SG now. Default SpatialType is ATTENUATED light.
			But maybe we need to remove INFINITE light spatials.
			So if function was called to remove ATTENUATED lights but they are not present
			try to find INFINITE and remove.*/
			if (spatialType == 3 && begin == 0 && end == 0) { 
				spatialType = 2; 
				begin = m_pDataBuffer[spatialType].begin();
				end = m_pDataBuffer[spatialType].end();
			}
			for (i=end;i!=begin;--i) {
				if ((m_pDataBuffer[spatialType])[i-1].m_pSpatial == o) {
					m_pDataBuffer[spatialType].remove(i-1);
					m_objectCount--;
					bDetach = TRUE;
					for (j=m_pDataBuffer[spatialType].begin();j!=m_pDataBuffer[spatialType].end();++j) {
						if ((m_pDataBuffer[spatialType])[j].m_pSpatial == o) {
							bDetach = FALSE;
							break;
						}
					}
					if(bDetach)
						iResult = pSubject->Detach((IFXObserver *)this);
				}
			}
		} else {
			IFXSpatialInstance SpatialInstance;
			SpatialInstance.m_pSpatial = o;
			SpatialInstance.m_Instance = Instance;

			if(m_pDataBuffer[spatialType].remove(SpatialInstance)) {
				m_objectCount--;
				bDetach = TRUE;
				for (i=m_pDataBuffer[spatialType].begin();i!=m_pDataBuffer[spatialType].end();++i) {
					if (m_pDataBuffer[spatialType][i].m_pSpatial == o) {
						bDetach = FALSE;
						break;
					}
				}
				if(bDetach)
					iResult = pSubject->Detach((IFXObserver *)this);
			} else {
				// Request to remove a non existant object.
				IFXASSERT(0);
				iResult = IFX_E_CANNOT_FIND;
			}
		}
		return iResult;
	}

	void GetObjectsFromNode( IFXSpatial**&      rpOutSpatials,
		U32&         ruOutNumberOfSpatials,
		U32        uListStart,
		IFXSpatial::eType  eInType ) const
	{
		IFXASSERT(0);
#if 0 ///@todo: (MP) update it when it will be useful
		U32 iter;
		ruOutNumberOfSpatials =0;
		if(!m_pDataList[eInType].empty())
		{
			for(iter = 0;iter != m_pDataList[eInType].m_dataCnt;++iter)
			{
				rpOutSpatials[uListStart+ruOutNumberOfSpatials] =
					m_pDataList[eInType].m_data[iter];
				ruOutNumberOfSpatials++;
			}
		}
#endif
	}

};
#endif
