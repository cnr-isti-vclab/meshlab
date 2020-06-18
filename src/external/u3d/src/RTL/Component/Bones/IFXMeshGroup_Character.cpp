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
	@file IFXMeshGroup_Character.cpp
*/

#include <string.h>
#include "IFXMeshGroup_Character.h"
#include "IFXVertexMapGroup.h"
#include "IFXCoreCIDs.h"

#define IFX_MGCHAR_PRINT_IMESH      FALSE
#define IFX_MGCHAR_REMAP_DEBUG      FALSE
#define IFX_MGCHAR_SEPARATE       TRUE  /// independent output mesh

#define IFX_MGCHAR_SMOOTH_ITERATIONS_REGEN  3
#define IFX_MGCHAR_SMOOTH_ITERATIONS_FINAL  10

IFXRESULT IFXMeshGroup_Character::AdoptMeshGroup(IFXMeshGroup *mg,
												 IFXVertexMapGroup *vmg)
{
	IFXRESULT result=IFX_OK;
	IFXASSERT(mg);

	FreeInMesh();
	m_share->m_inmeshgroup=mg;

	IFXVertexMapGroup *oldmap=m_vertexmapgroup;

	// NOTE if vm is NULL, don't replace
	if(vmg)
		m_vertexmapgroup=vmg;

	AttachMeshes();
	if(m_vertexmapgroup!=oldmap)
	{
		result=AdoptVertexMap();

		CalculateImplantReferences();
	}
	return result;
}

void IFXMeshGroup_Character::AttachMeshes(void)
{
	m_share->m_impl_in.SetMeshGroup(m_share->m_inmeshgroup);
	GetSkin()->SetInputMesh(&m_share->m_impl_in);

	#if IFX_MGCHAR_SEPARATE
		AllocOutMesh();
	#else
		m_share->m_outmeshgroup=m_share->m_inmeshgroup;
	#endif

	m_share->m_impl_out.SetMeshGroup(m_share->m_outmeshgroup);
	GetSkin()->SetOutputMesh(&m_share->m_impl_out);
}

void IFXMeshGroup_Character::AllocInMesh(void)
{
	FreeInMesh();

	IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)&m_share->m_inmeshgroup);
	IFXASSERT(m_share->m_inmeshgroup);

	m_vertexmapgroup=new IFXVertexMapGroup;
	IFXASSERT(m_vertexmapgroup);

	m_share->m_in_allocated=true;
}

void IFXMeshGroup_Character::FreeInMesh(void)
{
	if(m_share->m_in_allocated)
	{
		IFXASSERT(m_share->m_inmeshgroup);
		m_share->m_inmeshgroup=NULL;

		IFXASSERT(m_vertexmapgroup);
		delete m_vertexmapgroup;
		m_vertexmapgroup=NULL;

		m_share->m_in_allocated=false;
	}
}



/******************************************************************************
void IFXMeshGroup_Character::AllocOutMesh(void)

******************************************************************************/
void IFXMeshGroup_Character::AllocOutMesh(void)
{
	/// @todo: use a proper abstraction (COM operations)

	FreeOutMesh();
	IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)&m_share->m_outmeshgroup);
	IFXASSERT(m_share->m_outmeshgroup);
	m_share->m_out_allocated=true;

	IFXASSERT(m_share->m_inmeshgroup);

	m_share->m_outmeshgroup->TransferData(*m_share->m_inmeshgroup,
		~IFXMeshAttributes() ^ (IFXMeshAttributes(IFX_MESH_POSITION) | IFXMeshAttributes(IFX_MESH_NORMAL)),
		IFXMeshAttributes(IFX_MESH_POSITION) | IFXMeshAttributes(IFX_MESH_NORMAL),
		TRUE);
}

void IFXMeshGroup_Character::FreeOutMesh(void)
{
	if(m_share->m_out_allocated)
	{
		IFXASSERT(m_share->m_outmeshgroup);
		m_share->m_out_allocated=false;
	}
}


/******************************************************************************
IFXRESULT IFXMeshGroup_Character::CleanupWeights(bool regenerate,
bool removerogueweights,bool smooth,
F32 threshhold,F32 weldmax,F32 modelsize)

FUTURE don't repeat redundant FindNeighbors() and MapWeights() in functions

******************************************************************************/
IFXRESULT IFXMeshGroup_Character::CleanupWeights(bool regenerate,
												 bool removerogueweights,bool smooth,
												 F32 threshhold,F32 weldmax,F32 modelsize)
{
	if(!m_share->m_inmeshgroup->GetNumMeshes())
	{
		IFXTRACE_GENERIC(L"IFXMeshGroup_Character::CleanupWeights() no meshes\n");
		return IFX_E_NOT_INITIALIZED;
	}

	StateProgress("Adopt VertexMap");
	IFXTRACE_GENERIC(L"AdoptVertexMap()\n");
	IFXRESULT result=AdoptVertexMap();
	if(result!=IFX_OK)
		return result;

	StateProgress("Find Overlap");
	IFXTRACE_GENERIC(L"FindOverlap()\n");
	FindOverlap(weldmax*modelsize);

	StateProgress("Filter Weights");
	IFXTRACE_GENERIC(L"FilterWeights()\n");
	GetSkin()->FilterWeights();

	if(regenerate)
	{
		IFXTRACE_GENERIC(L"RegenerateWeights()\n");
		StateProgress("Regenerate Weights 1");
		CalculateImplantReferences();
		GetSkin()->RegenerateWeights(false,false);

		StateProgress("Regenerate Joints 1");
		IFXTRACE_GENERIC(L"Calc Joints\n");
		GetSkin()->CalculateJointCrossSections(true,false); // very crude

		StateProgress("Regenerate Smooth");
		GetSkin()->SmoothWeights(IFX_MGCHAR_SMOOTH_ITERATIONS_REGEN,
			threshhold,weldmax,modelsize);
		StateProgress("Regenerate Joints 2");
		GetSkin()->CalculateJointCrossSections(true,true);  // approximate

		StateProgress("Regenerate Weights 2");
		GetSkin()->RegenerateWeights(true,false);
	}
	else if(removerogueweights)
	{
		StateProgress("Pre-Rogue Calc Joints");
		GetSkin()->CalculateJointCrossSections(true,true);

		StateProgress("Pre-Rogue Proximities");
		GetSkin()->RegenerateWeights(true,true);      // use existing weights
	}

	if(removerogueweights)
	{
		StateProgress("Remove Rogue Weights");
		GetSkin()->RemoveRogueWeights();

		if(!regenerate)
		{
			StateProgress("Refilter Weights");
			IFXTRACE_GENERIC(L"Re FilterWeights()\n");
			GetSkin()->FilterWeights();
		}
	}

	if(smooth)
	{
		StateProgress("Pre-Smooth Calc Joints");
		IFXTRACE_GENERIC(L"Calc Joints\n");
		GetSkin()->CalculateJointCrossSections(true,false); // more accurate

		StateProgress("Smooth Weights");
		IFXTRACE_GENERIC(L"SmoothWeights()\n");
		GetSkin()->SmoothWeights(IFX_MGCHAR_SMOOTH_ITERATIONS_FINAL,
			threshhold,weldmax,modelsize);
	}

	StateProgress("Cache Vertices");
	GetSkin()->ComputeVertexOffsets();
	CalculateImplantReferences();

	StateProgress("");
	IFXTRACE_GENERIC(L" done\n");

	return IFX_OK;
}



/******************************************************************************
IFXRESULT IFXMeshGroup_Character::GetMasterMeshIndex(U32 mesh,U32 vertex,
U32 *mesh0,U32 *vertex0)

******************************************************************************/
IFXRESULT IFXMeshGroup_Character::GetMasterMeshIndex(U32 mesh,U32 vertex,
													 U32 *mesh0,U32 *vertex0)
{
	U32 originalmesh,originalvertex;
	GetBackmapIndex(mesh,vertex,&originalmesh,&originalvertex);

	IFXASSERT((U32)originalmesh<m_vertexmapgroup->GetNumMaps());
	IFXVertexMap *vertexmap=m_vertexmapgroup->GetMap(originalmesh);
	//I32 copies=vertexmap->GetNumVertexCopies(originalvertex);

	U32 mesh2=0,vert2=0;
	IFXRESULT result=vertexmap->GetVertexCopy(originalvertex,0,&mesh2,&vert2);

	*mesh0=mesh2;
	*vertex0=vert2;
	return result;
}



/******************************************************************************
IFXRESULT IFXMeshGroup_Character::AdoptVertexMap(void)

creates m_backmap, replicant list, and master vertices

******************************************************************************/
IFXRESULT IFXMeshGroup_Character::AdoptVertexMap(void)
{
	IFXMeshInterface *inmesh=GetSkin()->GetInputMesh();
	IFXArray<IFXMeshVertexArray> &mastervertices=GetSkin()->GetMasterVertices();
	IFXArray<IFXMeshVertexMap> &replicants=GetSkin()->GetReplicants();

	mastervertices.Clear();
	replicants.Clear();
	m_backmap.Clear();

	I32 meshid,vertex=0;
	U32 meshid2,vertex2;

	//* allocate and set defaults
	I32 meshes=inmesh->GetNumberMeshes();
	mastervertices.ResizeToAtLeast(meshes);
	replicants.ResizeToAtLeast(meshes);
	m_backmap.ResizeToAtLeast(meshes);
	for(meshid=0;meshid<meshes;meshid++)
	{
		inmesh->ChooseMeshIndex(meshid);
		I32 vertices=inmesh->GetMaxNumberVertices();

		mastervertices[meshid].ResizeToAtLeast(vertices);
		replicants[meshid].ResizeToAtLeast(vertices);
		m_backmap[meshid].ResizeToAtLeast(vertices);

		for(vertex=0;vertex<vertices;vertex++)
		{
			m_backmap[meshid][vertex].SetMeshVertex(0,0);
			mastervertices[meshid][vertex].SetMeshVertex(meshid,vertex);
		}
	}

	if(!m_vertexmapgroup)
	{
		IFXTRACE_GENERIC(L"No VertexMapGroup\n");
		return IFX_OK;
	}

	//* fill in data
	IFXMeshVertexList *replist = NULL;
	IFXMeshVertex mv;
	I32 map,maps=m_vertexmapgroup->GetNumMaps();
	for(map=0;map<maps;map++)
	{
		IFXVertexMap *vertexmap=m_vertexmapgroup->GetMap(map);
		I32 original,originals=vertexmap->GetNumMapEntries();
		for(original=0;original<originals;original++)
		{
			I32 n,copies=vertexmap->GetNumVertexCopies(original);
			//      IFXASSERT(copies>0);
			for(n=0;n<copies;n++)
			{
				IFXRESULT result=vertexmap->GetVertexCopy(original,n,
					&meshid2,&vertex2);
				IFXASSERT(result==IFX_OK);

				// replicants and mastervertices should be same size
				if(result!=IFX_OK ||
					meshid2>=(U32)m_backmap.GetNumberElements() ||
					vertex2>=(U32)m_backmap[meshid2].GetNumberElements())
				{
					IFXTRACE_GENERIC(L"IFXMeshGroup_Character::AdoptVertexMap"
						L" vertex out of range\n");
					return IFX_E_INVALID_RANGE;
				}

				if(n==0)
				{
					m_backmap[meshid2][vertex2].SetMeshVertex(map,original);

					replist=&replicants[meshid2][vertex2];

					meshid=meshid2;
					vertex=vertex2;
				}
				else
				{
					m_backmap[meshid2][vertex2].SetMeshVertex(map,-original-1);

					mv.GetMeshIndex()=meshid2;
					mv.GetVertexIndex()=vertex2;
					**(replist->Append(new IFXMeshVertex))=mv;

					mastervertices[meshid2][vertex2].SetMeshVertex(meshid,
						vertex);
				}
			}
		}
	}
	return IFX_OK;
}



/******************************************************************************
void IFXMeshGroup_Character::FindOverlap(F32 maxweld)

mark very-near vertices as replicants

******************************************************************************/
void IFXMeshGroup_Character::FindOverlap(F32 maxweld)
{
	if(maxweld<0.0f)
		return;

	IFXMeshInterface *inmesh=GetSkin()->GetInputMesh();
	IFXArray<IFXMeshVertexArray> &mastervertices=GetSkin()->GetMasterVertices();
	IFXArray<IFXMeshVertexMap> &replicants=GetSkin()->GetReplicants();
	IFXVertexWeights weights; // reuse type (has id's and offset)

	I32 meshid,meshes=inmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
	{
		inmesh->ChooseMeshIndex(meshid);
		I32 vertex,vertices=inmesh->GetMaxNumberVertices();

		for(vertex=0;vertex<vertices;vertex++)
		{
			IFXVertexWeight &weight=weights.CreateNewElement();
			weight.SetMeshIndex(meshid);
			weight.SetVertexIndex(vertex);
			weight.Offset()=inmesh->GetVertexConst(vertex);
		}
	}

	weights.SortOnZ();

	I32 m,n,length=weights.GetNumberElements();
	for(m=0;m<length;m++)
	{
		n=m+1;
		while(n<length-1 && (weights[n].Offset()[2]-
			weights[m].Offset()[2])<maxweld)
		{
			if(weights[n].Offset().CalcDistanceFrom(weights[m].Offset())
				< maxweld)
			{
				I32 meshid1=weights[m].MeshIndex();
				I32 vertex1=weights[m].VertexIndex();
				I32 meshid2=weights[n].MeshIndex();
				I32 vertex2=weights[n].VertexIndex();

				(**replicants[meshid1][vertex1].Append(new IFXMeshVertex))
					.SetMeshVertex(meshid2,vertex2);
				(**replicants[meshid2][vertex2].Append(new IFXMeshVertex))
					.SetMeshVertex(meshid1,vertex1);

				if(meshid1<=meshid2 && vertex1<vertex2)
					mastervertices[meshid1][vertex1]
					.SetMeshVertex(meshid2,vertex2);
				else
					mastervertices[meshid2][vertex2]
					.SetMeshVertex(meshid1,vertex1);
			}
			n++;
		}
	}
}

void IFXMeshGroup_Character::PrintIFXMeshGroup(IFXMeshGroup_Impl *img)
{
	I32 mesh,meshes=img->GetNumberMeshes();
	for(mesh=0;mesh<meshes;mesh++)
	{
		img->ChooseMeshIndex(mesh);

		I32 vert,verts=img->GetMaxNumberVertices();
		for(vert=0;vert<verts;vert++)
		{
			IFXTRACE_GENERIC(L"mesh %d vert %d/%d",mesh,vert,verts);
			IFXTRACE_GENERIC(L" position %.6G %.6G %.6G",
				img->GetVertexConst(vert)[0],
				img->GetVertexConst(vert)[1],
				img->GetVertexConst(vert)[2]);
			IFXTRACE_GENERIC(L" normal %.6G %.6G %.6G\n",
				img->GetNormalConst(vert)[0],
				img->GetNormalConst(vert)[1],
				img->GetNormalConst(vert)[2]);

			IFXVector3 normal=img->GetNormalConst(vert);
			if(normal.CalcMagnitude()>1.1f)
				IFXTRACE_GENERIC(L"  EXCESSIVE NORMAL\n");
		}

		I32 face,faces=img->GetMaxNumberFaces();
		for(face=0;face<faces;face++)
		{
			IFXTRACE_GENERIC(L"mesh %d face %d/%d",mesh,face,faces);
			IFXTRACE_GENERIC(L" position %d %d %d\n",
				img->GetFaceVerticesConst(face)[0],
				img->GetFaceVerticesConst(face)[1],
				img->GetFaceVerticesConst(face)[2]);
		}
	}
}
