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
	@file IFXCharacter.cpp
*/

#include <string.h>
#include "IFXCharacter.h"
#include "IFXBoneNodeList.h"
#include "IFXMeshInterface.h"

#define IFXKINE_DEBUG     FALSE
#define IFXKINE_CONSTRAIN   TRUE  // FALSE=bypass constraints

IFXDrawLineCB   IFXCharacter::m_drawlineCB=NULL;
IFXProgressCB   IFXCharacter::m_progressCB=NULL;
IFXTimeProgressCB IFXCharacter::m_timeProgressCB=NULL;

IFXCharacter::IFXCharacter(IFXCharacter *pCloneOf): IFXCoreNode(pCloneOf)
{
	if(pCloneOf)
	{
		m_pBoneContext=pCloneOf->m_pBoneContext;
		m_pBoneContext->IncReferences();
		Reset();
		CopyBoneHierarchy(pCloneOf,this);
	}
	else
	{
		m_pBoneContext=new IFXBoneContext();
		Reset();
		m_skin->ClearWeights();
	}
}

IFXCharacter::~IFXCharacter(void)
{
	RemoveDescendents(*this);

	m_pBoneContext->DecReferences();
}

void IFXCharacter::Reset(void)
{
	IFXCoreNode::Reset();
	m_transforms.GetCurrent().MakeIdentity();

	SetRootCharacter(this);
	RemoveDescendents(*this);

	IFXASSERT(m_pBoneContext);
	m_skin=&m_pBoneContext->Skin();
	GrabSkin();
	SetVertexWeights(&m_skin->GetVertexWeights());
	GetIKModes().Reset();

	m_bonetable.Clear();

	m_testflag=0;
}

void IFXCharacter::GrabSkin(void)
{
	m_skin->SetCharacter(this);
}

void IFXCharacter::CopyBoneHierarchy(IFXCoreNode *pParent,
									 IFXCoreNode *pNewParent)
{
	IFXBoneNodeList &childlist=pParent->Children();
	if(!childlist.GetNumberElements())
		return;

	IFXBoneNode *pNode;
	IFXListContext context;

	childlist.ToHead(context);
	while( (pNode=childlist.PostIncrement(context)) != NULL)
	{
		IFXBoneNode *pNewNode=new IFXBoneNode(this,pNode);
		pNewParent->AppendChild(pNewNode);

		IFXASSERT(pNode->GetBoneIndex()==GetBoneTableSize());

		pNewNode->SetBoneIndex(pNode->GetBoneIndex());
		SetBoneAtIndex(pNewNode->GetBoneIndex(),pNewNode);

		CopyBoneHierarchy(pNode,pNewNode);
	}
}


IFXCoreNode *IFXCharacter::FindDescendentByName2(IFXCoreNode &parent,
												 const IFXString& rName)
{
	const IFXString& rParentName = parent.NameConst();

	if(!rParentName.Compare(rName))
		return &parent;

	const IFXBoneNodeList &childlist=parent.ChildrenConst();
	if(!childlist.GetNumberElements())
		return NULL;

	IFXCoreNode *found;
	IFXBoneNode *node;
	IFXListContext context;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
	{
		// recursively to children
		if((found=FindDescendentByName2(*node,rName)))
			return found;
	}

	return NULL;
}

void IFXCharacter::ForEachNode(I32 flags,IFXForEachNodeCB callback,
							   IFXVariant state)
{
	BOOL stop=false;
	if( !(flags&IFXCHILDFIRST) && callback)
		stop=callback(*this,state);

	if(!stop)
		ForEachNode2(flags,*this,callback,state);

	if( (flags&IFXCHILDFIRST) && callback)
		callback(*this,state);
}


void IFXCharacter::ForEachNode2(I32 flags,IFXCoreNode &parent,
								IFXForEachNodeCB callback,IFXVariant state)
{
	IFXBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	IFXBoneNode *node;
	IFXListContext context;
	BOOL stop=false;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
	{
		if( !(flags&IFXCHILDFIRST) && callback)
			stop=callback(*node,state);

		//* recursively to children
		if(!stop)
			ForEachNode2(flags,*node,callback,state);

		if( (flags&IFXCHILDFIRST) && callback)
			callback(*node,state);
	}
}


void IFXCharacter::ForEachNodeDual(IFXForEachNodeCB preCallback,
								   IFXForEachNodeCB postCallback,IFXVariant state)
{
	BOOL stop=false;
	if(preCallback)
		stop=preCallback(*this,state);

	if(!stop)
		ForEachNodeDual2(*this,preCallback,postCallback,state);

	if(postCallback)
		postCallback(*this,state);
}


void IFXCharacter::ForEachNodeDual2(IFXCoreNode &parent,
									IFXForEachNodeCB preCallback,IFXForEachNodeCB postCallback,
									IFXVariant state)
{
	IFXBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	IFXBoneNode *node;
	IFXListContext context;
	BOOL stop=false;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
	{
		if(preCallback)
			stop=preCallback(*node,state);

		//* recursively to children
		if(!stop)
			ForEachNodeDual2(*node,preCallback,postCallback,state);

		if(postCallback)
			postCallback(*node,state);
	}
}



/**
can check IFXCoreNode::IsBone() to see if you can cast callback arg
from IFXCoreNode to IFXBoneNode

NOTE if not IFXCHILDFIRST, the callback can legally change the transform
passed by reference.
*/
void IFXCharacter::ForEachNodeTransformed(
	I32 flags,
	IFXForEachNodeTransformedCB callback,
	IFXVariant state)
{
	BOOL stop=false;

	m_transforms.Push();

	m_transforms.GetCurrent().MakeIdentity();
	m_transforms.GetCurrent().Translate(DisplacementConst());
	m_transforms.GetCurrent().Rotate(RotationConst());
	m_transforms.GetCurrent().Scale(ScaleConst());

	if( !(flags&IFXCHILDFIRST) && callback)
		stop=callback(*this, m_transforms.GetCurrent(), state);

	if(flags & IFXSTORE_XFORM)
		this->StoreTransform(m_transforms.GetCurrent());

	if(!stop)
		ForEachNodeTransformed2(flags, *this, callback, state);

	if( (flags&IFXCHILDFIRST) && callback)
		callback(*this,m_transforms.GetCurrent(),state);

	m_transforms.Pop();
}


/**
	@note scaling DOES propagate to children
*/
void IFXCharacter::ForEachNodeTransformed2(I32 flags,IFXCoreNode &rParent,
										   IFXForEachNodeTransformedCB callback,IFXVariant state)
{
	IFXBoneNodeList &rChildlist=rParent.Children();
	
	if(!rChildlist.GetNumberElements())
		return;

	IFXBoneNode *pNode = NULL;
	IFXListContext context;
	BOOL stop=false;

	rChildlist.ToHead(context);
	while( (pNode=rChildlist.PostIncrement(context)) != NULL)
	{
		F32 length=pNode->GetLength();

		m_transforms.Push();

		// I've changed following code (commented out now) because I'm not sure
		// about IFXTransformation class quality.
		//	m_transforms.GetCurrent().Translate(pNode->DisplacementConst());
		//	m_transforms.GetCurrent().Rotate(pNode->RotationConst());
		//	m_transforms.GetCurrent().Scale(pNode->ScaleConst());


		IFXMatrix4x4 matrix; // for current transformation
		IFXMatrix4x4 parentMatrix = m_transforms.GetCurrent().GetMatrixData(READONLY);

		// add displacement
		{
			IFXMatrix4x4 temp;
			temp.MakeIdentity();
			temp.SetTranslation(pNode->DisplacementConst());

			matrix.Multiply(parentMatrix, temp);
		}

		// add scale
		matrix.Scale(pNode->ScaleConst());

		//rotate
		{
			IFXMatrix4x4 temp;
			IFXVector3 v3_temp;

			v3_temp.X() = matrix[12];
			v3_temp.Y() = matrix[13];
			v3_temp.Z() = matrix[14];
 
			matrix.ResetTranslation(); // translate to origin

			IFXMatrix4x4 rotMatrix(pNode->RotationConst());

			temp = matrix;
			matrix.Multiply(temp, rotMatrix);
			matrix.SetTranslation(v3_temp); // translate back
		}

		m_transforms.GetCurrent() = matrix;


		if( !(flags&IFXCHILDFIRST) && callback)
			stop=callback(*pNode,m_transforms.GetCurrent(),state);

		if(flags&IFXSTORE_XFORM)
			pNode->StoreTransform(m_transforms.GetCurrent());

		//was: m_transforms.GetCurrent().Translate(length,0.0f,0.0f);
		{
			IFXMatrix4x4 temp;
			temp.MakeIdentity();
			temp[12] = length;
			
			m_transforms.GetCurrent().Multiply(temp);
		}


		// recursively to children
		if(!stop)
			ForEachNodeTransformed2(flags,*pNode,callback,state);

		if( (flags&IFXCHILDFIRST) && callback)
		{
			m_transforms.GetCurrent().Translate(-length,0.0f,0.0f);

			callback(*pNode,m_transforms.GetCurrent(),state);
		}

		m_transforms.Pop();
	}
}

void IFXCharacter::RemoveDescendents(IFXCoreNode &parent)
{
	IFXBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	IFXBoneNode *node;
	IFXListContext context;

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
	{
		// recursively to children
		RemoveDescendents(*node);

		delete node;
	}

	childlist.Clear();
}

F32 IFXCharacter::CalcReachingLimit(void)
{
	F32 max=0.0f;

	ResetToFullReach();
	ForEachNodeTransformed(IFXPARENTFIRST|IFXSTORE_XFORM,NULL);

	// NOTE using potentially lowered res since higher data may not be loaded
	GetSkin()->DeformMesh(false,false);

	IFXMeshInterface *outmesh=GetSkin()->GetInputMesh();
	IFXASSERT(outmesh);
	IFXVertexIndexer *indexer=outmesh->GetVertexIndexer();
	IFXASSERT(indexer);
	I32 meshid,meshes=outmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
	{
		outmesh->ChooseMeshIndex(meshid);

		I32 index,vertices=outmesh->GetNumberVertices();
		for(index=0;index<vertices;index++)
		{
			indexer->MoveToIndex(index);
			IFXVector3 *vertex=indexer->GetPosition();
			F32 magnitude=vertex->CalcMagnitude();

			if(max<magnitude)
				max=magnitude;
		}
	}

	ResetToReference();
	ForEachNodeTransformed(IFXPARENTFIRST|IFXSTORE_XFORM,NULL);
	GetSkin()->DeformMesh(false,false);

	IFXTRACE_GENERIC(L"Reaching Limit is %.6G\n",max);
	return max;
}


void IFXCharacter::RemoveBoneLinks(void)
{
	ForEachNode(IFXCHILDFIRST,&IFXCharacter::RemoveBoneLinkNode,0);
	RebuildBoneTable();
}


BOOL IFXCharacter::RemoveBoneLinkNode(IFXCoreNode &node,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	IFXBoneNode &bnode=(IFXBoneNode &)node;

	if(!bnode.IsBoneLink())
		return false;

	IFXCoreNode *parent=node.Parent();
	IFXASSERT(parent);
	parent->RemoveChild(&bnode);
	return false;
}


void IFXCharacter::BlendBones(F32 weight)
{
	F32 weight2=weight;
	if(weight2<0.0f)
		weight2=0.0f;
	if(weight2>1.0f)
		weight2=1.0f;

	ForEachNode(IFXCHILDFIRST,&IFXCharacter::BlendBoneNode,&weight2);
}


BOOL IFXCharacter::BlendBoneNode(IFXCoreNode &node,IFXVariant state)
{
	if(node.IsBone())
	{
		IFXBoneNode &bnode=(IFXBoneNode &)node;

		if(bnode.IsBoneLink())
			return false;
	}

	F32* weight = NULL;
#ifdef _DEBUG    
	IFXRESULT result = 
#endif  
		state.CopyPointerTo(&weight);
	IFXASSERT(result==IFX_OK);
	IFXASSERT(weight);

	//* weighted average
	IFXVector3 pos,scale;
	IFXQuaternion rot;

	pos.Interpolate(*weight,node.BlendDisplacementConst(),
		node.DisplacementConst());
	rot.Interpolate(*weight,node.BlendRotationConst(),
		node.RotationConst());
	scale.Interpolate(*weight,node.BlendScaleConst(),
		node.ScaleConst());

	node.Displacement()=pos;
	node.Rotation()=rot;
	node.Scale()=scale;
	return false;
}


void IFXCharacter::RebuildBoneTable(void)
{
	ClearBoneTable();
	ForEachNode(IFXCHILDFIRST,&IFXCharacter::AddBoneToTable,this);
}


BOOL IFXCharacter::AddBoneToTable(IFXCoreNode &node,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	IFXCharacter *character=NULL;
#ifdef _DEBUG    
	IFXRESULT result = 
#endif  
		state.CopyPointerTo(&character);
	IFXASSERT(result==IFX_OK);
	IFXASSERT(character);

	IFXBoneNode &bnode=(IFXBoneNode &)node;

	character->SetBoneAtIndex(bnode.GetBoneIndex(),&bnode);
	return false;
}


void IFXCharacter::SetBoneAtIndex(I32 index,IFXBoneNode *set)
{
	IFXASSERT(index>=0);

	I32 m,length=m_bonetable.GetNumberElements();
	m_bonetable.ResizeToAtLeast(index+1);

	for(m=length;m<index;m++)
		m_bonetable[m]=NULL;
	m_bonetable[index]=set;
}


void IFXCharacter::GenerateBoneLinks(I32 defaultnumber)
{
	ForEachNode(IFXCHILDFIRST,&IFXCharacter::CreateLinksOnNode,&defaultnumber);
	GetSkin()->RelinkWeights();
	GetSkin()->FilterWeights();

	RecalcLinks();
	ForEachNodeTransformed(IFXPARENTFIRST|IFXSTORE_XFORM,NULL);
	ForEachNode(IFXCHILDFIRST,&IFXCharacter::CalcLinkReferencesOnNode);

	GetSkin()->ComputeVertexOffsets();
}


BOOL IFXCharacter::CreateLinksOnNode(IFXCoreNode &node,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	I32 *defaultnumber=NULL;
#ifdef _DEBUG    
	IFXRESULT result = 
#endif  
		state.CopyPointerTo(&defaultnumber);
	IFXASSERT(result==IFX_OK);
	IFXASSERT(defaultnumber);

	// don't create links on a link
	if(!((IFXBoneNode &)node).IsBoneLink())
		((IFXBoneNode &)node).CreateBoneLinks(*defaultnumber);

	return false;
}


BOOL IFXCharacter::CalcLinkReferencesOnNode(IFXCoreNode &node,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	((IFXBoneNode &)node).CalcBoneLinkReferences();
	return false;
}


void IFXCharacter::RecalcLinks(void)
{
	ForEachNode(IFXPARENTFIRST,&IFXCharacter::RecalcLinksOnNode);
}


BOOL IFXCharacter::RecalcLinksOnNode(IFXCoreNode &node,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	((IFXBoneNode &)node).CalcBoneLinks();
	return false;
}


void IFXCharacter::StoreReferencePositions(void)
{
	ForEachNodeTransformed(IFXPARENTFIRST,&IFXCharacter::StoreReferenceForBone);
}


BOOL IFXCharacter::StoreReferenceForBone(IFXCoreNode &node,
										 IFXTransform &transform,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	// store current orientation as the reference
	node.StoreReferenceTransform(transform);
	node.LocalReferenceRotation()=node.RotationConst();
	node.ReferenceDisplacement()=node.DisplacementConst();
	return false;
}



/******************************************************************************
non-link bones
******************************************************************************/
I32 IFXCharacter::CountRealBones(void)
{
	I32 bones=0;

	ForEachNode(IFXCHILDFIRST,&IFXCharacter::CountRealBone,&bones);

	return bones;
}


BOOL IFXCharacter::CountRealBone(IFXCoreNode &node,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	IFXBoneNode &bonenode=(IFXBoneNode &)node;

	if(bonenode.IsBoneLink())
		return false;

	I32* bones=NULL;
#ifdef _DEBUG    
	IFXRESULT result = 
#endif  
		state.CopyPointerTo(&bones);
	IFXASSERT(result==IFX_OK);
	IFXASSERT(bones);

	(*bones)++;
	return false;
}



#define IFXIK_INCREMENTAL TRUE
void IFXCharacter::ApplyIK(void)
{
	IFXIKModes &ik_modes=GetIKModes();

	// FUTURE try using a delta<threshhold check instead of fixed number
	// of iterations
	U32 iterations=ik_modes.GetIterations();

	if(!ik_modes.GetIncremental())
		ResetToReference();

	U32 m;
	for(m=0;m<iterations;m++)
		ForEachNodeTransformed(IFXCHILDFIRST|IFXSTORE_XFORM,
		&IFXCharacter::KineBone,&ik_modes);

	// Is this redundant?
	RecalcLinks();
}


BOOL IFXCharacter::StoreIKData(IFXCoreNode &node,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	//IFXBoneNode &bonenode=(IFXBoneNode &)node;

	/// @todo: fix it
	IFXASSERT(0);
	return false;
}


BOOL IFXCharacter::KineBone(IFXCoreNode &node,
							IFXTransform &transform,IFXVariant state)
{
	if(!node.IsBone())
		return false;

 	IFXBoneNodeList &effectionlist=node.Effections();

	I32 effections=effectionlist.GetNumberElements();
	if(!effections)
		return false;

	IFXIKModes *ik_modes=NULL;
#ifdef _DEBUG    
	IFXRESULT result = 
#endif  
		state.CopyPointerTo(&ik_modes);
	IFXASSERT(result==IFX_OK);
	IFXASSERT(ik_modes);

	IFXBoneNode &thisbone=(IFXBoneNode &)node;

	IFXBoneNode *effected_bone;
	IFXListContext context;

	// FUTURE try more flexible weighting
	F32 scalar=1.0f/(F32)effections;

	IFXQuaternion change;
	change.MakeIdentity();

	// FUTURE can move calcs for current out of loop

	effectionlist.ToHead(context);
	while( (effected_bone=effectionlist.PostIncrement(context)) != NULL)
	{
		if(!effected_bone->IsEffected())
			continue;

		IFXVector3 effected;

		IFXVector3 &effector=effected_bone->Effector();

		IFXVector3 tip(effected_bone->GetLength(),0.0f,0.0f);
		effected_bone->StoredTransform().TransformVector(tip,effected);

		// everything in world is reverse-transformed to local space
		IFXVector3 current,desired;

		thisbone.StoredTransform().ReverseTransformVector(effected,current);
		thisbone.StoredTransform().ReverseTransformVector(effector,desired);

		//* draw some lines for debugging
		IFXVector3 here;
		here=thisbone.StoredTransform().TranslationConst();

		IFXVector3 world_current,world_desired;
		thisbone.StoredTransform().TransformVector(current,world_current);
		thisbone.StoredTransform().TransformVector(desired,world_desired);

		bool effector_here= (&thisbone==effected_bone);
		IFXCharacter *character=thisbone.RootCharacter();
		character->DrawLine(effector_here? 0: 2,here,world_current);
		character->DrawLine(effector_here? 1: 3,here,world_desired);

		F32 cur_mag2=current.CalcMagnitudeSquared();
		F32 des_mag2=desired.CalcMagnitudeSquared();

		if(cur_mag2<0.1f || des_mag2<0.1f)
			continue;

		current.Scale(1.0f/IFXSQRT(cur_mag2));
		desired.Scale(1.0f/IFXSQRT(des_mag2));

		IFXQuaternion delta;
		delta.MakeRotation(current,desired);

		// divide influence by number of effections
		delta.ScaleAngle(scalar);

		change.Rotate(delta);
	}

	//* WARNING this limit is being apply per iteration, not frame or timestep
	//* limit angular velocity
	if(ik_modes->GetLimitAngVelocity())
	{
		F32 radians;
		IFXVector3 axis;

		change.ComputeAngleAxis(radians,axis);

		F32 maxangvel=ik_modes->GetMaxAngVelocity();
		if(radians>maxangvel)
		{
			radians=maxangvel;
			change.MakeRotation(radians,axis);
		}
	}

	//* apply change
	thisbone.Rotation().Rotate(change);

	//*       `constrain joint angles'

	IFXConstraints &rot=thisbone.RotationalConstraints();

	bool &world_relative=rot.WorldRelative();

	IFXEuler euler;
	IFXQuaternion inverse,local_delta;
	IFXCoreNode *parent=node.Parent();

#if IFXKINE_DEBUG
	IFXEuler dataeuler;
#endif

	if(world_relative)
	{
		// get global rotation without recomputing m_transforms
		IFXQuaternion global_rot;
		global_rot.Multiply(parent->StoredTransform().QuaternionConst(),
			thisbone.RotationConst());

		IFXQuaternion parent_delta,global_delta,mid;
		inverse.Invert(parent->StoredTransform().QuaternionConst());
		parent_delta.Multiply(parent->GlobalReferenceRotationConst(),inverse);

		mid.Multiply(parent_delta,global_rot);
		inverse.Invert(thisbone.GlobalReferenceRotationConst());
		global_delta.Multiply(mid,inverse);

		euler=global_delta;

#if IFXKINE_DEBUG
		IFXTRACE_GENERIC(L"parent Gref %ls\n",
			parent->GlobalReferenceRotationConst().Out(true).Raw());
		dataeuler=parent->GlobalReferenceRotationConst();
		IFXTRACE_GENERIC(L" euler %ls\n",dataeuler.Out(true).Raw());

		IFXTRACE_GENERIC(L"parent Gcur %ls\n",
			parent->StoredTransform().QuaternionConst()
			.Out(true).Raw());
		dataeuler=parent->StoredTransform().QuaternionConst();
		IFXTRACE_GENERIC(L" euler %ls\n",dataeuler.Out(true).Raw());

		IFXTRACE_GENERIC(L"child Gref %ls\n",
			thisbone.GlobalReferenceRotationConst().Out(true).Raw());
		dataeuler=thisbone.GlobalReferenceRotationConst();
		IFXTRACE_GENERIC(L" euler %ls\n",dataeuler.Out(true).Raw());

		IFXTRACE_GENERIC(L"child Gcur %ls\n",global_rot.Out(true).Raw());
		dataeuler=global_rot;
		IFXTRACE_GENERIC(L" euler %ls\n",dataeuler.Out(true).Raw());

		IFXTRACE_GENERIC(L"child Gdel %ls\n",global_delta.Out(true).Raw());
		dataeuler=global_delta;
		IFXTRACE_GENERIC(L" euler %ls\n",dataeuler.Out(true).Raw());
#endif
	}
	else
	{
		//* get rotation delta from reference
		inverse.Invert(thisbone.LocalReferenceRotation());
		local_delta.Multiply(inverse,thisbone.RotationConst());

		euler=local_delta;
	}

#if IFXKINE_DEBUG
	IFXTRACE_GENERIC(L"%s Before %ls\n",thisbone.Name().Raw8(),euler.Out(true).Raw());
	IFXTRACE_GENERIC(L"min %ls\n",rot.Min().Out(true).Raw());
	IFXTRACE_GENERIC(L"max %ls\n",rot.Max().Out(true).Raw());
#endif

	//* FUTURE also check alternate interpretation where mag of euler Y >90deg

#if IFXKINE_CONSTRAIN
	//IFXEuler eulerRef=thisbone.LocalReferenceRotation();
#if IFXKINE_DEBUG
	IFXTRACE_GENERIC(L"localRef %ls\n",thisbone.LocalReferenceRotation().Out(true).Raw());
	IFXTRACE_GENERIC(L"eulerRef %ls\n",eulerRef.Out(true).Raw());
	IFXQuaternion testquat;
	euler.ConvertToQuaternion(testquat);
	IFXTRACE_GENERIC(L"testquat %ls\n",testquat.Out(true).Raw());
#endif
	//* apply limits
	F32 eulerY=euler[1];
	F32 min,max,mindiff,maxdiff;
	int m;
	for(m=0;m<3;m++)
	{
#if IFXKINE_DEBUG
		IFXTRACE_GENERIC(L" [%d]",m);
#endif

		if(rot.Active()[m] && !rot.Limited()[m])
		{
#if IFXKINE_DEBUG
			IFXTRACE_GENERIC(L" active-unlimited");
#endif
			continue;
		}

		if(!rot.Active()[m])
		{
#if IFXKINE_DEBUG
			IFXTRACE_GENERIC(L" inactive");
#endif
			euler[m]=0.0f;
		}
		else
		{
			min=rot.Min()[m];
			max=rot.Max()[m];

			// swap if neccessary
			if(min>max)
			{
				min=max;
				max=rot.Min()[m];
			}

			//* if Y near +-90 deg, X and Z are unstable -> use previous
			if(m!=1)
			{
				// FUTURE paramaterize
				const F32 ymargin=0.05f;  // radians

				F32 yd90;
				if(eulerY>0.0f)
					yd90=(F32)fabs(eulerY-(90.0f*IFXTO_RAD));
				else
					yd90=(F32)fabs(eulerY+(90.0f*IFXTO_RAD));
				if(yd90<ymargin)
				{
#if IFXKINE_DEBUG
					IFXTRACE_GENERIC(L" lock");
#endif
					euler[m]=rot.LastEuler()[m];
					continue;
				}
#if IFXKINE_DEBUG
				else
					IFXTRACE_GENERIC(L" (%.6G)",yd90);
#endif
			}

			F32 euler2=euler[m];
			F32 min2=min;
			while(euler2<=0)
				euler2+=(IFXPI*2.0f);
			while(min2<=0)
				min2+=(IFXPI*2.0f);
			if(euler2<=max || euler2>=min2)
			{
#if IFXKINE_DEBUG
				IFXTRACE_GENERIC(L" ok");
#endif
				rot.Bias()[m]=2;
				continue;
			}

			mindiff=min-euler[m];
			maxdiff=euler[m]-max;

			while(mindiff<0)
				mindiff+=(IFXPI*2.0f);
			while(mindiff>=IFXPI)
				mindiff-=(IFXPI*2.0f);

			while(maxdiff<0)
				maxdiff+=(IFXPI*2.0f);
			while(maxdiff>IFXPI)
				maxdiff-=(IFXPI*2.0f);

			mindiff=(F32)fabs(mindiff);
			maxdiff=(F32)fabs(maxdiff);

			//* use historical bias to avoid alternating incremental solutions
			// this is where a solution far outside the constraints switches
			// between which constraints it is closest to
			// anything over PI is pretty much absolute: no switching
			// until it passes through a valid solution
			const F32 biasangle=10.0f;

			if(rot.Bias()[m]==0)
			{
				if(maxdiff>0.0f)
					maxdiff+=biasangle;
			}
			else if(rot.Bias()[m]==1)
			{
				if(mindiff>0.0f)
					mindiff+=biasangle;
			}

#if IFXKINE_DEBUG
			IFXTRACE_GENERIC(L" %d,%.6G,%.6G",rot.Bias()[m],mindiff,maxdiff);
#endif

			//* figure out which is closest (think circular)
			//  closest is smallest of non-zero values
			if(maxdiff<mindiff)
			{
				euler[m]=max;
				rot.Bias()[m]=1;
			}
			else
			{
				euler[m]=min;
				rot.Bias()[m]=0;
			}
		}
	}
#endif

#if IFXKINE_DEBUG
	IFXTRACE_GENERIC(L"\nAfter %ls\n",euler.Out(true).Raw());
#endif

	rot.LastEuler()=euler;

	//* return from delta to reference
	IFXQuaternion constrained;
	if(world_relative)
	{
		// convert world to local
		IFXQuaternion global_delta,inverse;

		euler.ConvertToQuaternion(global_delta);

		IFXQuaternion mid;
		inverse.Invert(parent->GlobalReferenceRotationConst());

		/*
		constrained.Multiply(inverse,global_delta);
		*/
		mid.Multiply(inverse,global_delta);
		constrained.Multiply(mid,thisbone.GlobalReferenceRotationConst());

#if IFXKINE_DEBUG
		IFXTRACE_GENERIC(L"global_delta %ls\n",global_delta.Out(true).Raw());
		dataeuler=global_delta;
		IFXTRACE_GENERIC(L" euler %ls\n",dataeuler.Out(true).Raw());
#endif
	}
	else
	{
		euler.ConvertToQuaternion(local_delta);

		constrained.Multiply(thisbone.LocalReferenceRotation(),local_delta);
	}

	thisbone.Rotation()=constrained;

#if IFXKINE_DEBUG
	IFXTRACE_GENERIC(L"Constrained %ls\n",constrained.Out(true).Raw());
	dataeuler=constrained;
	IFXTRACE_GENERIC(L" euler %ls\n\n",dataeuler.Out(true).Raw());
#endif

	//* recompute transforms
	// NOTE don't need to transform all, just this and descendants
	thisbone.RootCharacter()->ForEachNodeTransformed(
		IFXCHILDFIRST|IFXSTORE_XFORM);
	return false;
}


void IFXCharacter::ResetToReference(void)
{
	ForEachNode(IFXPARENTFIRST,&IFXCharacter::ResetToReferenceOnNode);
}


BOOL IFXCharacter::ResetToReferenceOnNode(IFXCoreNode &node,IFXVariant state)
{
	node.Displacement()=node.ReferenceDisplacement();
	node.Rotation()=node.LocalReferenceRotation();
	node.Scale().Set(1.0f,1.0f,1.0f);

	node.BlendDisplacement()=node.ReferenceDisplacement();
	node.BlendRotation()=node.LocalReferenceRotation();
	node.BlendScale().Set(1.0f,1.0f,1.0f);
	return false;
}


void IFXCharacter::ResetToFullReach(void)
{
	ForEachNode(IFXPARENTFIRST,&IFXCharacter::ResetToFullReachOnNode);
}


BOOL IFXCharacter::ResetToFullReachOnNode(IFXCoreNode &node,IFXVariant state)
{
	if(!node.Parent() || node.Parent()==node.RootCharacter())
		node.Displacement().Reset();
	else
		node.Displacement()=node.ReferenceDisplacement();

	node.Rotation().MakeIdentity();
	node.Scale().Set(1.0f,1.0f,1.0f);

	node.BlendDisplacement()=node.DisplacementConst();
	node.BlendRotation().MakeIdentity();
	node.BlendScale().Set(1.0f,1.0f,1.0f);
	return false;
}


void IFXCharacter::CalcNodeRotationsFromStoredTranslations(bool lookback)
{
	IFXTransformStack &m_transforms=TransformStack();

	m_transforms.Push();

	m_transforms.GetCurrent().MakeIdentity();
	m_transforms.GetCurrent().Translate(DisplacementConst());
	m_transforms.GetCurrent().Rotate(RotationConst());

	CalcNodeRotations(lookback,*this);

	m_transforms.Pop();
}


void IFXCharacter::CalcNodeRotations(bool lookback,IFXCoreNode &parent)
{
	IFXCharacter *character=parent.RootCharacter();
	IFXTransformStack &m_transforms=character->TransformStack();

	IFXVector3 worldvector,localvector;
	IFXVector3 parent_location,child_location,grandchild_location;

	IFXVector3 xaxis(1.0f,0.0f,0.0f);
	IFXVector3 zero(0.0f,0.0f,0.0f);

	IFXBoneNodeList &childlist=parent.Children();
	if(!childlist.GetNumberElements())
		return;

	IFXBoneNode *node;
	IFXListContext context;
	// F32 parentlength=0.0f;

	if(parent.IsBone())
	{
		node=(IFXBoneNode *)&parent;
		parent_location=node->StoredTransform().TranslationConst();
		// parentlength=node->GetLength();
	}
	else
		parent_location.Reset();

	childlist.ToHead(context);
	while( (node=childlist.PostIncrement(context)) != NULL)
	{
		child_location=node->StoredTransform().TranslationConst();

		if(lookback)
		{
			if(!parent.IsBone())
			{
				node->Displacement()=child_location;
				worldvector.Reset();
				localvector=xaxis;
			}
			else
			{
				worldvector.Subtract(child_location,parent_location);
				m_transforms.GetCurrent().ReverseTransformVector(child_location,
					localvector);
			}
		}
		else
		{
			IFXBoneNode *grandchild=node->Children().GetHead();
			if(grandchild)
			{
				grandchild_location=grandchild->StoredTransform()
					.TranslationConst();
				worldvector.Subtract(grandchild_location,child_location);
				m_transforms.GetCurrent().ReverseRotateVector(worldvector,
					localvector);
			}
			else
			{
				worldvector.Reset();
				localvector=xaxis;
			}
		}

		localvector.Normalize();

		node->SetLength(worldvector.CalcMagnitude());

		if(lookback)
		{
			//* for load ASE: create rotations
			if(node->GetLength()<0.1)
				node->Rotation().MakeIdentity();
			else
				node->Rotation().MakeRotation(xaxis,localvector);
		}
		else
		{
			//* for load ASH: find/create displacements
			m_transforms.GetCurrent().ReverseTransformVector(child_location,
				localvector);
			if(localvector.IsApproximately(zero,0.1f))
				node->Displacement().Reset();
			else
				node->Displacement()=localvector;
		}

		//* do child's new transform
		m_transforms.Push();

		m_transforms.GetCurrent().Translate(node->DisplacementConst());
		m_transforms.GetCurrent().Rotate(node->RotationConst());
		m_transforms.GetCurrent().Translate(node->GetLength(),0.0f,0.0f);

		CalcNodeRotations(lookback,*node);

		m_transforms.Pop();
	}
}

void IFXCharacter::CalculateImplantReferences(void)
{
	ForEachNodeTransformed(IFXPARENTFIRST|IFXSTORE_XFORM,
		&IFXCharacter::CalculateImplantReferenceForBone);
}


BOOL IFXCharacter::CalculateImplantReferenceForBone(IFXCoreNode &node,
													IFXTransform &transform,IFXVariant state)
{
	if(!node.IsBone())
		return false;

	IFXBoneNode &bnode=(IFXBoneNode &)node;

	// requires stored m_transforms
	bnode.CalcImplantReference();
	return false;
}
