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
	@file IFXBoneNode.cpp
*/

#include "IFXBoneNodeList.h"
#include "IFXCharacter.h"

void IFXBoneNode::IFXBoneNodeShare::Reset(void)
{
	m_autojoint=true;
	m_autolink=true;
	m_boneid=0;
	m_linkboneid= -1;
	m_length=0.0f;
	m_influential=true;
	m_implant_len=0.0f;
	m_implant_rot.MakeIdentity();
	m_implant_lenref=0.0f;
	m_implant_rotref.MakeIdentity();
	m_isbonelink=false;
}

void IFXBoneNode::ResetBone(void)
{
	SetIsBone(true);
	m_picked=false;
	m_effected=false;
	m_effector.Reset();
}

BOOL IFXBoneNode::HasAncestor(IFXCoreNode *node)
{
	if(Parent()==node)
		return true;
	else if(!Parent()->IsBone())
		return false;

	IFXBoneNode *bone=(IFXBoneNode *)Parent();
	return bone->HasAncestor(node);
}

void IFXBoneNode::CalcImplant(void)
{
	IFXCoreNode *pParent = Parent();
	if(!pParent || !pParent->IsBone())
		return;

	IFXVector3 const zero(0.0f,0.0f,0.0f);
	IFXVector3		 xaxis (1.0f,0.0f,0.0f);

	// orient from child's view
	IFXVector3 parent_local, parent_world;
	if(DisplacementConst().IsApproximately(zero,1e-3f))
		parent_local.Set(-1.0f,0.0f,0.0f);
	else
		parent_local.Set(((IFXBoneNode *)pParent)->GetLength(),0.0f,0.0f);

	pParent->StoredTransform().TransformVector(parent_local,parent_world);
	StoredTransform().ReverseTransformVector(parent_world,parent_local);

	IFXQuaternion local_rot;
	parent_local.Normalize();
	parent_local.Invert();
	local_rot.MakeRotation(xaxis,parent_local);
	ImplantRotation().Multiply(RotationConst(),local_rot);

	ImplantLength()=DisplacementConst().CalcMagnitude();
}

void IFXBoneNode::CalcImplantReference(void)
{
	CalcImplant();

	ImplantReferenceRotation()=ImplantRotation();
	ImplantReferenceLength()=ImplantLength();
}



/******************************************************************************
void IFXBoneNode::CreateEffections(IFXCoreNode *base)

set effections recursively up to given base node
if base==NULL, goes all the way to the root-most node

effections can be made to an unaffected bone;
they are ignored until reactivated

******************************************************************************/
void IFXBoneNode::CreateEffections(IFXCoreNode* base)
{
	IFXCoreNode *node=this;

	while(node!=NULL)
	{
		node->Effections().Append(this);

		if(node==base)
			break;

		node=node->Parent();
	}
}

void IFXBoneNode::CreateBoneLinks(I32 defaultnumber)
{
	if(!Parent()->IsBone())
		return;

	if(IsAutoLink())
	{
		//      BoneLinks().ResetSamples();
		BoneLinks().SetNumberLinks(defaultnumber);

		IFXCylinder &bounds=CylindricalBounds();
		IFXVector3 &jointscale=bounds.GetScale(0);
		F32 jointsize=0.5f*(jointscale[1]+jointscale[2]);

		F32 totallength=GetLength()*0.3f+jointsize*1.5f;

		BoneLinks().SetLinkLength(totallength);
	}

	BoneLinks().AllocateArray();

	I32 m,number=BoneLinks().GetArraySize();
	for(m=0;m<number;m++)
	{
		IFXBoneNode *newnode=new IFXBoneNode(RootCharacter());
		Parent()->AppendChild(newnode);
		BoneLinks().SetLink(m,newnode);
		I32 newindex=RootCharacter()->GetBoneTableSize();
		RootCharacter()->SetBoneAtIndex(newindex,newnode);

		IFXString buffer( NameConst().Raw() );
		buffer += ( L" Link" );
		char mstr[10];
		sprintf( mstr, "%u", m);
		buffer += ( (U8*)mstr );

		newnode->SetName(buffer);
		newnode->SetBoneIndex(newindex);
		newnode->LinkBoneIndex()=this->GetBoneIndex();
		newnode->SetBoneLink(true);
	}
}

void IFXBoneNode::CalcBoneLinkReferences(void)
{
	I32 m,number=BoneLinks().GetArraySize();
	for(m=0;m<number;m++)
	{
		IFXBoneNode *linknode=BoneLinks().GetLink(m);
		linknode->ReferenceDisplacement()=linknode->DisplacementConst();
		linknode->LocalReferenceRotation()=linknode->RotationConst();

		linknode->StoreReferenceTransform(linknode->StoredTransform());
	}
}


/******************************************************************************
link locations based on weighting between two paths
first path follows bone X axes exactly
second path is a straight line between attachment points
******************************************************************************/
void IFXBoneNode::CalcBoneLinks(void)
{
	BOOL force_a_line=(RootCharacter()->GetTestFlag()!=0);

	const I32 number=BoneLinks().GetArraySize();
	if(number<1)
		return;

	const F32 link_length=BoneLinks().GetLinkLength()/(F32)number;

	IFXVector3 &dref=ReferenceDisplacement();
	const IFXVector3 &disp=DisplacementConst();

	/*** NOTE  The implant is a otherwise non-contributing psuedo-parent bone
	needed when the real parent is not directly connected to this child.
	The implant represents where a connected parent might be. */

	// start on implant
	const IFXQuaternion lref=ImplantRotation();

	const IFXQuaternion &lrot=RotationConst();

	IFXTransform transform;
	IFXVector3 fdisp,slide,result;
	slide.Reset();

	IFXVector3 slide0,sliden,slidemid,onbone;
	slide[0]=link_length*(0-(number+2)/2.0f+0.5f);
	transform=lref;
	transform.RotateVector(slide,slide0);

	slide[0]=link_length*((number+2)-1-(number+2)/2.0f+0.5f);
	transform=lrot;
	transform.RotateVector(slide,sliden);

	//* location
	I32 m;
	F32 tween,ratio,weight;
	for(m=0;m<number;m++)
	{
		ratio=(number>1)? m/(F32)(number-1): 0.0f;  // from 0 to 1
		weight=0.5f-(F32)fabs(ratio-0.5f);          // between paths
		tween=(m+1)/(F32)(number+1);                    // never 0 or 1

		//* account for bone displacement
		fdisp.Subtract(disp,dref);
		fdisp.Scale(tween);
		fdisp.Add(dref);

		transform=(m<number/2)? lref: lrot;

		slide[0]=link_length*(m-number/2.0f+0.5f);
		transform.RotateVector(slide,onbone);

		slidemid.Interpolate(tween,slide0,sliden);
		result.Interpolate(weight,onbone,slidemid);

		IFXBoneNode *linknode=BoneLinks().GetLink(m);
		linknode->SetLength(link_length);
		linknode->Displacement().Add(fdisp,result);
	}

	//* rotation
	IFXVector3 diff,prev,next,xaxis(1.0f,0.0f,0.0f);
	for(m=0;m<number;m++)
	{
		tween=(m+1)/(F32)(number+1);
		IFXBoneNode *linknode=BoneLinks().GetLink(m);

		if(force_a_line)
		{
			if(m==0)
				prev.Add(dref,slide0);
			else
				prev=BoneLinks().GetLink(m-1)->DisplacementConst();

			if(m==number-1)
				next.Add(disp,sliden);
			else
				next=BoneLinks().GetLink(m+1)->DisplacementConst();

			diff.Subtract(next,prev);
			diff.Normalize();
			linknode->Rotation().MakeRotation(xaxis,diff);
		}
		else
			linknode->Rotation().Interpolate(tween,lref,lrot);
	}

	//* computed points are the bone centers, so we have to adjust
	//* since bone positions are stored as the base point
	for(m=0;m<number;m++)
	{
		tween=(m+1)/(F32)(number+1);
		IFXBoneNode *linknode=BoneLinks().GetLink(m);

		//      tween=powf(tween,1.0f); // FUTURE explore and parameterize link power
		//      linknode->RotationConst().Interpolate(tween,lref,lrot);

		slide[0]= -link_length/2.0f;
		transform=linknode->RotationConst();
		transform.RotateVector(slide,result);
		linknode->Displacement().Add(result);
	}

	//* twist correction
	if(force_a_line)
	{
		IFXQuaternion twist,inverse,desired,adjust;

		tween=number/(F32)(number+1);
		desired.Interpolate(tween,lref,lrot);
		inverse.Invert(BoneLinks().GetLink(number-1)->RotationConst());
		twist.Multiply(inverse,desired);
		for(m=0;m<number;m++)
		{
			tween=(m+1)/(F32)(number);      // different meaning than above
			IFXBoneNode *linknode=BoneLinks().GetLink(m);

			adjust.Interpolate(tween,IFXQuaternion::Identity(),twist);
			linknode->Rotation().Multiply(adjust);
		}
	}
}
