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
	@file IFXMeshGroup_Impl.h
*/

#ifndef IFXMESHGROUP_IMPL_H
#define IFXMESHGROUP_IMPL_H

#include "IFXMeshInterface.h"
#include "IFXMesh.h"

#define IFXCHEAT_IFXMESH    TRUE    ///< assume layout for speed

/**
	IFXMesh interface implementation  
*/
class IFXMeshGroup_Impl : public IFXMeshInterface
{
public:
	IFXMeshGroup_Impl(void)
	{
		m_meshgroup = NULL;
		m_validmeshgroup = NULL;
	}

	virtual ~IFXMeshGroup_Impl() {};

	IFXMeshGroup* GetMeshGroup(void) { return m_meshgroup; } ///< Simple data getter
	void SetMeshGroup(IFXMeshGroup* set) { m_meshgroup = set; } ///< Simple data setter

	virtual U32 IFXAPI GetNumberMeshes(void) const
	{   
		return (m_meshgroup)? m_meshgroup->GetNumMeshes(): 0; 
	}

	// implementations of virtuals
	virtual void IFXAPI ChooseMeshIndex(U32 set)
	{
		IFXMeshInterface::ChooseMeshIndex(set);

		IFXMesh *mesh = 0;
		m_meshgroup->GetMesh(GetMeshID(), mesh);
		IFXASSERT(mesh);

#ifdef _DEBUG
		IFXRESULT result=
#endif
		mesh->GetVertexIter(m_vertexIndexer);
		IFXASSERT(IFX_OK == result);

#ifdef _DEBUG
		result=
#endif
		mesh->GetFaceIter(m_faceIter);
		IFXASSERT(IFX_OK == result);

		m_validmeshgroup = m_meshgroup;

		IFXRELEASE(mesh);
	}

	virtual void IFXAPI SetNumberVertices(U32 set)
	{
		IFXMesh* pMesh = 0;
		IFXASSERT(m_meshgroup);
		m_meshgroup->GetMesh(GetMeshID(), pMesh);
		IFXASSERT(pMesh);
		pMesh->SetNumVertices(set);
		IFXRELEASE(pMesh);
	}

	virtual U32  IFXAPI GetNumberVertices(void) const
	{
		IFXMesh* pMesh = 0;
		IFXASSERT(m_meshgroup);
		m_meshgroup->GetMesh(GetMeshID(), pMesh);
		IFXASSERT(pMesh);
		U32 uNumVerts = pMesh->GetNumVertices();
		IFXRELEASE(pMesh);
		return uNumVerts;
	}

	virtual U32  IFXAPI GetNumberNormals(void) const
	{
		// each vertex has it's own normal
		return GetNumberVertices();
	}

	virtual U32  IFXAPI GetNumberTexCoords(void) const
	{
		// each vertex has it's own tex coord
		return GetNumberVertices();
	}

	virtual U32  IFXAPI GetNumberFaces(void) const
	{
		IFXASSERT(m_meshgroup);
		IFXMesh* pMesh = 0;
		m_meshgroup->GetMesh(GetMeshID(), pMesh);
		U32 uNumFaces = pMesh->GetNumFaces();
		IFXRELEASE(pMesh);
		return uNumFaces;
	}

	virtual U32  IFXAPI GetMaxNumberVertices(void) const
	{
		IFXASSERT(m_meshgroup);
		IFXMesh* pMesh = 0;
		m_meshgroup->GetMesh(GetMeshID(), pMesh);
		U32 uNumVerts = pMesh->GetMaxNumVertices();
		IFXRELEASE(pMesh);
		return uNumVerts;
	}

	virtual U32  IFXAPI GetMaxNumberNormals(void) const
	{
		return GetMaxNumberVertices();
	}

	virtual U32  IFXAPI GetMaxNumberTexCoords(void) const
	{
		return GetMaxNumberVertices();
	}
	
	virtual U32  IFXAPI GetMaxNumberFaces(void) const
	{
		IFXMesh *pMesh = NULL;
		U32 uNumFaces = 0;

		IFXASSERT(m_meshgroup);
		m_meshgroup->GetMesh(GetMeshID(), pMesh);
		IFXASSERT(pMesh);
		uNumFaces = pMesh->GetMaxNumFaces();
		IFXRELEASE(pMesh);
		
		return uNumFaces;
	}

	virtual IFXVertexIndexer* GetVertexIndexer(void)
	{
		return &m_vertexIndexer;
	}

	virtual const F32* GetVertexConst(U32 index) const
	{
		IFXVector3 *vector=GetVertexVector(index);

#if IFXCHEAT_IFXMESH
		return vector->RawConst();
#else
		static F32 floats[3];

		floats[0]=vector->X();
		floats[1]=vector->Y();
		floats[2]=vector->Z();

		return floats;
#endif
	}

	virtual void IFXAPI SetVertex(U32 index,const F32 vert[3])
	{
		IFXVector3 *vector=GetVertexVector(index);
		vector->Raw()[0]=vert[0];
		vector->Raw()[1]=vert[1];
		vector->Raw()[2]=vert[2];
	}

	virtual const F32* GetNormalConst(U32 index) const
	{
		IFXVector3 *normal=GetNormalVector(index);

#if IFXCHEAT_IFXMESH
		return normal->RawConst();
#else
		static F32 floats[3];

		floats[0]=normal->X();
		floats[1]=normal->Y();
		floats[2]=normal->Z();

		return floats;
#endif
	}

	virtual void IFXAPI SetNormal(U32 index, const F32 vert[3])
	{
		IFXVector3 *normal = GetNormalVector(index);
		normal->Raw()[0] = vert[0];
		normal->Raw()[1] = vert[1];
		normal->Raw()[2] = vert[2];
	}

	virtual const F32* GetTexCoordConst(U32 index) const
	{
		IFXVector2 *texcoord=GetTexCoordVector(index);
		IFXASSERT(texcoord);

#if IFXCHEAT_IFXMESH
		return &(texcoord->Value(0));
#else
		static F32 floats[2];

		floats[0]=texcoord->X();
		floats[1]=texcoord->Y();

		return floats;
#endif
	}

	virtual void IFXAPI SetTexCoord(U32 index,const F32 vert[2])
	{
		IFXVector2 *texcoord=GetTexCoordVector(index);
		texcoord->X()=vert[0];
		texcoord->Y()=vert[1];
	}

	/** @note returns pointer to statically allocated buffer */
	virtual const U32* GetFaceVerticesConst(U32 index) const
	{
		IFXFace *face=GetFace(index);

		static U32 array[3];

		array[0]=face->VertexA();
		array[1]=face->VertexB();
		array[2]=face->VertexC();

		return array;
	}

	virtual const U32* GetFaceNormalsConst(U32 index) const
	{
		return GetFaceVerticesConst(index);
	}

	virtual const U32* GetFaceTexCoordsConst(U32 index) const
	{
		return GetFaceVerticesConst(index);
	}

	virtual void IFXAPI SetFaceVertices(U32 index,const U32 vertID[3])
	{
		IFXASSERT(vertID[0]<GetMaxNumberVertices());
		IFXASSERT(vertID[1]<GetMaxNumberVertices());
		IFXASSERT(vertID[2]<GetMaxNumberVertices());

		IFXFace *face=GetFace(index);

		face->SetA(vertID[0]);
		face->SetB(vertID[1]);
		face->SetC(vertID[2]);
	}

	virtual void IFXAPI SetFaceNormals(U32 index,const U32 normID[3])
	{
	// let SetFaceVertices() do the work
	}

	virtual void IFXAPI SetFaceTexCoords(U32 index,const U32 coordID[3])
	{
	// let SetFaceVertices() do the work
	}

private:
	/** @note const method, changes internal iterator state */
	IFXVector3* GetVertexVector(U32 index) const
	{
		IFXASSERT(m_meshgroup);
		IFXASSERT(index < GetMaxNumberVertices());
		IFXASSERT(m_meshgroup == m_validmeshgroup);

		((IFXVertexIndexer &)m_vertexIndexer).MoveToIndex(index);

		IFXASSERT(((IFXVertexIndexer &)m_vertexIndexer).GetPosition());
		return ((IFXVertexIndexer &)m_vertexIndexer).GetPosition();
	}

	/** @note const method, changes internal iterator state */
	IFXVector3* GetNormalVector(U32 index) const
	{
		IFXASSERT(m_meshgroup);
		IFXASSERT(index < GetMaxNumberVertices());
		IFXASSERT(m_meshgroup==m_validmeshgroup);

		((IFXVertexIndexer &)m_vertexIndexer).MoveToIndex(index);
		IFXASSERT(((IFXVertexIndexer &)m_vertexIndexer).GetNormal());

		return ((IFXVertexIndexer &)m_vertexIndexer).GetNormal();
	}

	/** @note const method, changes internal iterator state */
	IFXVector2* GetTexCoordVector(U32 index) const
	{
		IFXASSERT(m_meshgroup);
		IFXASSERT(index < GetMaxNumberVertices());
		IFXASSERT(m_meshgroup == m_validmeshgroup);

		((IFXVertexIndexer &)m_vertexIndexer).MoveToIndex(index);
		IFXASSERT(((IFXVertexIndexer &)m_vertexIndexer).GetTexCoord());
		return ((IFXVertexIndexer &)m_vertexIndexer).GetTexCoord();
	}

	/** @note const method, but changes internal iterator state */
	IFXFace* GetFace(U32 index) const 
	{
		IFXASSERT(m_meshgroup);
		IFXASSERT(index < GetMaxNumberFaces());
		IFXASSERT(m_meshgroup == m_validmeshgroup);

		IFXASSERT(((IFXFaceIter &)m_faceIter).Index(index));
		return ((IFXFaceIter &)m_faceIter).Index(index);
	}

	IFXVertexIndexer    m_vertexIndexer;
	IFXFaceIter     m_faceIter;
	IFXMeshGroup    *m_meshgroup;
	IFXMeshGroup    *m_validmeshgroup;
};

#endif
