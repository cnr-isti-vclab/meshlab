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
	@file IFXBoneNode.h
*/

#ifndef IFXBONENODE_H
#define IFXBONENODE_H

#include "IFXCoreNode.h"
#include "IFXConstraints.h"
#include "IFXCylinder.h"
#include "IFXMeshVertexMap.h"
#include "IFXBoneLinks.h"

class IFXBoneNode : public IFXCoreNode
{
	class IFXBoneNodeShare
	{
	public:
		IFXBoneNodeShare(void)  { Reset(); };
		void            Reset(void);
		// rotation, length, constraints are relative to end of parent

		I32             m_boneid;
		I32             m_linkboneid;
		F32             m_length;
		IFXConstraints  m_positional;
		IFXConstraints  m_rotational;

		F32             m_implant_len;
		IFXQuaternion   m_implant_rot;
		F32             m_implant_lenref;
		IFXQuaternion   m_implant_rotref;

		IFXMeshVertex   m_bestvert; // vert with best generate score
		F32             m_bestscore;

		BOOL            m_influential;

		BOOL            m_autojoint;
		IFXCylinder     m_bounds;

		BOOL            m_autolink;
		IFXBoneLinks    m_bonelinks;
		BOOL            m_isbonelink;
		U32             m_count;
	};

public:
	IFXBoneNode(void)
	{
		// only here for IFXList<IFXBoneNode>
		IFXTRACE_GENERIC(L"don't use "
			L"IFXBoneNode::IFXBoneNode(void)");

		IFXBoneNode(NULL);
	}

	IFXBoneNode(IFXCharacter *set, IFXBoneNode *pCloneOf=NULL)
		:IFXCoreNode(pCloneOf)
	{
		ResetBone();
		if(pCloneOf)
		{
			m_share=pCloneOf->m_share;
			m_share->m_count++;
		}
		else
		{
			m_share=new IFXBoneNodeShare();
			m_share->m_count=1;
		}
		SetRootCharacter(set);
	}

	virtual ~IFXBoneNode(void)
	{
		if(!(--m_share->m_count))
			delete m_share;
	}

	void Reset(void)
	{
		IFXCoreNode::Reset();
		ResetBone();
	}

	void ResetBone(void);

	void SetBoneIndex(I32 set) {
		m_share->m_boneid=set; 
	}

	I32 GetBoneIndex(void) const {
		return m_share->m_boneid; 
	}

	void SetLength(F32 set) { 
		m_share->m_length = set; 
	}
	
	F32 GetLength(void) const { 
		return m_share->m_length; 
	}

	void                SetInfluential(BOOL set)
	{ m_share->m_influential=set; };
	BOOL                IsInfluential(void) const
	{ return m_share->m_influential; };

	void                SetEffected(BOOL set)   { m_effected=set; };
	BOOL                IsEffected(void) const  { return m_effected; };


	IFXConstraints      &PositionalConstraints(void)
	{ return m_share->m_positional; };
	const   IFXConstraints      &PositionalConstraintsConst(void) const
	{ return m_share->m_positional; };
	IFXConstraints      &RotationalConstraints(void)
	{ return m_share->m_rotational; };
	const   IFXConstraints      &RotationalConstraintsConst(void) const
	{ return m_share->m_rotational; };

	void                SetAutoJoint(BOOL set)
	{ m_share->m_autojoint=set; };
	BOOL                IsAutoJoint(void) const
	{ return m_share->m_autojoint; };

	void                SetAutoLink(BOOL set)
	{ m_share->m_autolink=set; };
	BOOL                IsAutoLink(void) const
	{ return m_share->m_autolink; };

	//* internal:

	I32                 &BoneIndex(void)
	{ return m_share->m_boneid; };

	BOOL                &Influential(void)
	{ return m_share->m_influential; };

	BOOL                &Effected(void)         { return m_effected; };
	IFXVector3          &Effector(void)         { return m_effector; };
	const   IFXVector3          &EffectorConst(void) const
	{ return m_effector; };

	BOOL                &AutoJoint(void)
	{ return m_share->m_autojoint; };
	BOOL                &AutoLink(void)
	{ return m_share->m_autolink; };

	IFXBoneLinks        &BoneLinks(void)
	{ return m_share->m_bonelinks; };
	I32                 &LinkBoneIndex(void)
	{ return m_share->m_linkboneid; };

	BOOL                IsBoneLink(void)
	{ return m_share->m_isbonelink; };
	void                SetBoneLink(BOOL set)
	{ m_share->m_isbonelink=set; };

	IFXCylinder         &CylindricalBounds(void)
	{ return m_share->m_bounds; };

	BOOL                &DebugPicked(void)      { return m_picked; };

	void                CalcImplantReference(void);
	IFXQuaternion       &ImplantReferenceRotation(void)
	{ return m_share->m_implant_rotref;};
	F32                 &ImplantReferenceLength(void)
	{ return m_share->m_implant_lenref;};
	void                CalcImplant(void);
	IFXQuaternion       &ImplantRotation(void)
	{ return m_share->m_implant_rot; };
	F32                 &ImplantLength(void)
	{ return m_share->m_implant_len; };

	void                CreateEffections(IFXCoreNode *base);

	BOOL                HasAncestor(IFXCoreNode *node);

	void                CreateBoneLinks(I32 defaultnumber);
	void                CalcBoneLinks(void);
	void                CalcBoneLinkReferences(void);

	IFXMeshVertex       &BestVertex(void)
	{ return m_share->m_bestvert; };
	F32                 &BestVertexScore(void)
	{ return m_share->m_bestscore; };

private:

	F32                 CalcLinkLength(void);

	IFXBoneNodeShare    *m_share;

	// NOTE non-sharable data

	//* effector is the desired position of this node (for IK)
	BOOL                m_effected;
	IFXVector3          m_effector;
	BOOL                m_picked;
};

#endif
