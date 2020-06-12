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
	@file IFXCoreNode.h 
*/

#ifndef IFXCORENODE_H
#define IFXCORENODE_H

#include "IFXTransform.h"

class IFXBoneNode;
class IFXBoneNodeList;
class IFXCharacter;

/**
	Defined here: Effections are an association to an effector through
	an IFXBoneNode
*/
class IFXCoreNode
{
	class IFXCoreNodeShare
	{
	public:
		IFXCoreNodeShare(void)  { Reset(); }
		void Reset(void)
		{
			m_name=L"none";
			m_reference_displacement.Reset();
			m_local_reference_rotation.MakeIdentity();
			m_referenceTransform.Reset();
			m_is_bone=false;
		}

		bool            m_is_bone;
		IFXVector3      m_reference_displacement;
		IFXQuaternion   m_local_reference_rotation;
		IFXTransform    m_referenceTransform;
		IFXString       m_name;
		U32             m_count;
	};

public:
	IFXCoreNode(IFXCoreNode *pCloneOf=NULL);
	virtual                     ~IFXCoreNode(void);
	void                Reset(void);

	bool                IsBone(void) const  { return m_share->m_is_bone; };

	IFXString&          Name(void)              { return m_share->m_name; }
	const   IFXString&  NameConst(void) const   { return m_share->m_name; }
	void                SetName(const IFXString& rSet)  { m_share->m_name=rSet; }
	void                AppendChild(IFXBoneNode *bonenode);
	void                RemoveChild(IFXBoneNode *bonenode);

	IFXVector3&			Displacement(void) { return m_displacement; }

	IFXVector3 const&	DisplacementConst(void) const { return m_displacement; }

	IFXQuaternion&		Rotation(void)         { return m_rotation; }
	const IFXQuaternion& RotationConst(void) const { return m_rotation; }

	IFXVector3&			Scale(void)            { return m_scale; }
	IFXVector3 const&	ScaleConst(void) const { return m_scale; }

	//* internal:

	IFXCoreNode*		Parent(void)           { return m_parent; }
	const   IFXCoreNode* ParentConst(void) const { return m_parent; }
	void                SetParent(IFXCoreNode *set) { m_parent=set; }

	IFXBoneNodeList&	Children(void)         { return *m_pChildren; }
	const   IFXBoneNodeList& ChildrenConst(void) const { return *m_pChildren; }

	IFXBoneNodeList&	Effections(void) { return *m_pEffections; }
	IFXBoneNodeList const& EffectionsConst(void) const { 
		return *m_pEffections; 
	}

	IFXVector3 const &GetReferenceDisplacement(void) { 
		return m_share->m_reference_displacement; 
	}

	IFXVector3 &ReferenceDisplacement(void) { 
		return m_share->m_reference_displacement; 
	}

	IFXQuaternion       &LocalReferenceRotation(void) {
		return m_share->m_local_reference_rotation; 
	}

	IFXQuaternion const &LocalReferenceRotationConst(void) {
		return m_share->m_local_reference_rotation; 
	}

	IFXQuaternion const &GlobalReferenceRotationConst(void) { 
		return m_share->m_referenceTransform.QuaternionConst(); 
	}

	IFXVector3          &BlendDisplacement(void)
	{ return m_blend_displacement; }
	const   IFXVector3          &BlendDisplacementConst(void) const
	{ return m_blend_displacement; }

	IFXQuaternion       &BlendRotation(void)    { return m_blend_rotation;};
	const   IFXQuaternion       &BlendRotationConst(void) const
	{ return m_blend_rotation;}

	IFXVector3          &BlendScale(void)       { return m_blend_scale; };
	const   IFXVector3          &BlendScaleConst(void) const
	{ return m_blend_scale; }

	void                SetRootCharacter(IFXCharacter* set)
	{ m_character=set; }
	IFXCharacter        *RootCharacter(void)    { return m_character; }


	void                StoreReferenceTransform(IFXTransform &transform)
	{
		transform.UpdateMatrix();
		m_share->m_referenceTransform=transform;
	}
	IFXTransform        &ReferenceTransform(void)
	{ return m_share->m_referenceTransform;}

	void StoreTransform(IFXTransform &rTransform)
	{
		rTransform.UpdateMatrix();
		m_storedTransform=rTransform;
	}
	IFXTransform        &StoredTransform(void)
	{ return m_storedTransform; }

protected:
	void                SetIsBone(bool set) { m_share->m_is_bone=set; }

private:

	IFXCoreNodeShare    *m_share;

	//* WARNING not sharable
	IFXCharacter        *m_character;
	IFXCoreNode         *m_parent;
	IFXBoneNodeList     *m_pChildren;
	IFXBoneNodeList     *m_pEffections;
	IFXTransform        m_storedTransform;
	IFXVector3          m_displacement;
	IFXQuaternion       m_rotation;
	IFXVector3          m_scale;
	IFXVector3          m_blend_displacement;
	IFXQuaternion       m_blend_rotation;
	IFXVector3          m_blend_scale;
};

#endif
