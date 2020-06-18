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
	@file IFXCharacter.h
*/

#ifndef IFXCHARACTER_H
#define IFXCHARACTER_H

#include "IFXTransformStack.h"
#include "IFXSkin.h"
#include "IFXIKModes.h"
#include "IFXBoneContext.h"
#include "IFXBonesManager.h"
#include "IFXCoreNode.h"

class IFXCharacter;
#define IFXALLOW_TIMEPROGRESS   TRUE

#if IFXALLOW_TIMEPROGRESS
	#define TIME_PROGRESS(x,y,z)    IFXCharacter::TimeProgress(x,y,z)
#else
	inline void IFXNoTimeProgress(IFXCharacter *character,U32 index,bool on) {}

	#define TIME_PROGRESS       1 ? (void)0 : ::IFXNoTimeProgress
#endif

#define IFXPARENTFIRST      0
#define IFXCHILDFIRST       1
#define IFXSTORE_XFORM      2

typedef void (*IFXDrawLineCB)(I32 color,const IFXVector3 &first,
													const IFXVector3 &second);
typedef void (*IFXTimeProgressCB)(IFXCharacter *character,U32 index,bool on);

typedef BOOL (*IFXForEachNodeCB)(IFXCoreNode &,IFXVariant state);
typedef BOOL (*IFXForEachNodeTransformedCB)(IFXCoreNode &,
											IFXTransform &,IFXVariant state);


/**
	for children, probably will have just one root bone
*/
class IFXCharacter : public IFXCoreNode
{
public:
	IFXCharacter(IFXCharacter *pCloneOf=NULL);
	virtual                     ~IFXCharacter(void);

	void                Reset(void);
	void                GrabSkin(void);

	IFXSkin             *GetSkin(void)                  { return m_skin; }

	IFXCoreNode         *FindDescendentByName(const IFXString& rName)
	{
		return FindDescendentByName2(*this,rName);
	}

	void                ClearBoneTable(void)
	{
		m_bonetable.Clear();
	}
	void                RemoveBoneLinks(void);
	void                RebuildBoneTable(void);
	I32                 GetBoneTableSize(void)
	{
		return m_bonetable.GetNumberElements();
	}
	void                SetBoneAtIndex(I32 index,IFXBoneNode *set);
	IFXBoneNode         *LookupBoneIndex(I32 index)
	{
		if(index>=(I32)m_bonetable.GetNumberElements() 
			|| index<0)
		{
			return NULL;
		}
		return m_bonetable[index];
	}

	IFXTransformStack   &TransformStack(void)   { return m_transforms;}

	// not inline:

	void                CalcNodeRotationsFromStoredTranslations(
		bool lookback);

	void                ApplyIK(void);

	void                ResetToReference(void);
	void                ResetToFullReach(void);

	void                ForEachNode(I32 flags,IFXForEachNodeCB callback,
		IFXVariant state=0);
	void                ForEachNodeDual(IFXForEachNodeCB preCallback,
		IFXForEachNodeCB postCallback,
		IFXVariant state=0);
	void                ForEachNodeTransformed(I32 flags,
		IFXForEachNodeTransformedCB callback=NULL,
		IFXVariant state=0);

	I32                 CountRealBones(void);
	F32                 CalcReachingLimit(void);
	void                StoreReferencePositions(void);
	void                GenerateBoneLinks(I32 defaultnumber);
	void                RecalcLinks(void);
	void                CalculateImplantReferences(void);
	void                BlendBones(F32 weight);

	void                RemoveDescendents(IFXCoreNode &parent);

	IFXString           Out(bool verbose=false) const;

	static  void                DrawLine(I32 color,const IFXVector3 &first,
		const IFXVector3 &second)
	{
		if(m_drawlineCB)
			m_drawlineCB(color,first,second);
	}
	static  void                StateProgress(const char *text)
	{
		if(m_progressCB)
			m_progressCB(text);
	}
	static  void                TimeProgress(IFXCharacter *character,
		U32 index,bool on)
	{
		if(m_timeProgressCB)
			m_timeProgressCB(character,index,on);
	}
	void                SetDrawLineCB(IFXDrawLineCB set)
	{ m_drawlineCB=set; }
	void                SetProgressCB(IFXProgressCB set)
	{ m_progressCB=set; }
	void                SetTimeProgressCB(IFXTimeProgressCB set)
	{ m_timeProgressCB=set; }

	IFXVertexWeights    *GetVertexWeights(void) { return m_vertexweights; }
	void                SetVertexWeights(IFXVertexWeights *set)
	{ m_vertexweights=set; }

	IFXIKModes          &GetIKModes(void)       { return m_ikmodes; }

	I32                 GetTestFlag(void)       { return m_testflag; }
	void                SetTestFlag(I32 set)    { m_testflag=set; }

	// internal:
	void                ForEachNode2(I32 flags,IFXCoreNode &parent,
		IFXForEachNodeCB callback,
		IFXVariant state);
	void                ForEachNodeDual2(IFXCoreNode &parent,
		IFXForEachNodeCB preCallback,
		IFXForEachNodeCB postCallback,
		IFXVariant state);
	void                ForEachNodeTransformed2(I32 flags,
		IFXCoreNode &parent,
		IFXForEachNodeTransformedCB callback,
		IFXVariant state);

private:
	void                CopyBoneHierarchy(IFXCoreNode *pParent,
		IFXCoreNode *pNewParent);
	void                CalcNodeRotations(bool lookback,
		IFXCoreNode &parent);

	IFXCoreNode         *FindDescendentByName2(IFXCoreNode &parent,
		const IFXString& rName);

	static  BOOL                CreateLinksOnNode(IFXCoreNode &node,
		IFXVariant state);
	static  BOOL                RecalcLinksOnNode(IFXCoreNode &node,
		IFXVariant state);
	static  BOOL                CalcLinkReferencesOnNode(IFXCoreNode &node,
		IFXVariant state);
	static  BOOL                RemoveBoneLinkNode(IFXCoreNode &node,
		IFXVariant state);
	static  BOOL                AddBoneToTable(IFXCoreNode &node,IFXVariant state);
	static  BOOL                BlendBoneNode(IFXCoreNode &node,IFXVariant state);


	static  BOOL                ResetToReferenceOnNode(IFXCoreNode &node,
		IFXVariant state);
	static  BOOL                ResetToFullReachOnNode(IFXCoreNode &node,
		IFXVariant state);
	static  BOOL                CountRealBone(IFXCoreNode &node,IFXVariant state);
	static  BOOL                StoreReferenceForBone(IFXCoreNode &node,
		IFXTransform &transform,IFXVariant state);
	static  BOOL                KineBone(IFXCoreNode &node,IFXTransform &transform,
		IFXVariant state);
	static  BOOL                StoreIKData(IFXCoreNode &node,IFXVariant state);
	static  BOOL                CalculateImplantReferenceForBone(IFXCoreNode &node,
		IFXTransform &transform,IFXVariant state);

	IFXBoneContext          *m_pBoneContext;
	IFXSkin                 *m_skin;

	IFXTransformStack       m_transforms;
	IFXVertexWeights        *m_vertexweights;   ///< pointer into skin
	IFXArray<IFXBoneNode *> m_bonetable;
	F32                     m_timer;
	IFXIKModes              m_ikmodes;
	static  IFXDrawLineCB           m_drawlineCB;
	static  IFXProgressCB           m_progressCB;
	static  IFXTimeProgressCB       m_timeProgressCB;
	I32                     m_testflag;
};

#endif
