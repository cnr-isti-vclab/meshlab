#ifndef _OCME_EXTRACT_
#define _OCME_EXTRACT_


#include "impostor_definition.h"
#include "ocme_definition.h"
#include "../ooc_vector/berkeleydb/ooc_chains_berkeleydb.hpp"

extern unsigned int generic_bool;

template <class MeshType>
void OCME::ExtractVerticesFromASingleCell( CellKey ck , MeshType & m){

	//// phase 1. Copy all the vertices
	Cell* c = GetCell(ck,false);

	typename MeshType::VertexIterator vi;
	Chain<OVertex> * chain = c->vert;
	RAssert(chain != NULL);
	vi  = vcg::tri::Allocator<MeshType>::AddVertices(m,chain->Size());

	chain->LoadAll();
	for(unsigned int i = 0; i < chain->Size(); ++i,++vi) 
		if((*chain)[i].isExternal )  vcg::tri::Allocator<MeshType>::DeleteVertex(m,*vi);
		else
		(*vi).P() = (*chain)[i].P();
 	chain->FreeAll();
}

template <class MeshType>
void OCME::ExtractContainedFacesFromASingleCell( CellKey ck , MeshType & m, bool loadall, bool includeExternals){

	//// phase 1. Copy all the vertices
	Cell* c = GetCell(ck,false);
	sprintf(lgn->Buf(),"start adding vertices clock(): %d ", clock());

	if(ck==CellKey(-2,-4,15,1))
			ck =CellKey(-2,-4,15,1);
	if(loadall){
		c->face->LoadAll();
 		c->vert->LoadAll();
	}


	typename MeshType::VertexIterator vi;
	Chain<OVertex> * chain = c->vert;
	RAssert(chain != NULL);
	Chain<GIndex> * extref = c->externalReferences;

	vi  =vcg::tri::Allocator<MeshType>::AddVertices(m,chain->Size());

	for(unsigned int i = 0; i < chain->Size(); ++i,++vi)
		if((*chain)[i].isExternal)
		{
			if(includeExternals){
				Cell * cell;   int index;
				FindExternalVertexCellIndex( (*extref)[(*chain)[i].GetIndexToExternal()],cell,index);
				RAssert(cell);

				(*vi).P() = (*(cell->vert))[index].P();
				// export ...
			}
			else
				vcg::tri::Allocator<MeshType>::DeleteVertex(m,*vi);
		}
		else
			{
	 			/* Here there should be the importer from OVertex to vcgVertex */
				(*vi).P() = (*chain)[i].P();
				// export ...
			}
 
	//// phase 2. Copy all the faces
	Chain<OFace> *  &	face_chain		= c->face;				// get the face chain
	Chain<OVertex> * &  vertex_chain	= c->vert;				// get the vertex chain
	RAssert(face_chain!=NULL);
	RAssert(vertex_chain!=NULL);

	typename MeshType::VertexPointer vp[3];
	for(unsigned int i = 0; i < face_chain->Size(); ++i){
		OFace of = (*face_chain)[i];
		int vpi;
		for(vpi = 0; vpi < 3; ++vpi){
			if(includeExternals || !((*vertex_chain)[ of[vpi] ]).isExternal)
				vp[vpi] = & m.vert[ of[vpi] ];
			else
				break;
		}
		if(vpi==3){
			typename MeshType::FaceIterator fi = vcg::tri::Allocator<MeshType>::AddFaces(m,1);
			(*fi).V(0) = vp[0];(*fi).V(1) = vp[1];(*fi).V(2) = vp[2];
		}	
	}

	if(loadall){
		c->face->FreeAll();
 		c->vert->FreeAll();
	}

}

template <class MeshType>
void OCME::Extract(   std::vector<Cell*> & sel_cells, MeshType & m, AttributeMapper attr_map){


	//// phase 1. Copy all the vertices

	///* for each cell, keep the position of the beginng of its copy in the mesh vertices m.vert*/
	std::map<Cell*,int> vertOffsets;

	RemoveDuplicates(sel_cells);
	sprintf(lgn->Buf(),"start adding vertices clock(): %d ", static_cast<int>(clock()));
        lgn->Push();

	// create an attibute that will store the address in ocme for the vertex
	typename MeshType::template PerVertexAttributeHandle<GIndex> gPosV =
		vcg::tri::Allocator<MeshType>::template GetPerVertexAttribute<GIndex> (m,"ocme_gindex");

	if(!vcg::tri::Allocator<MeshType>::IsValidHandle(m,gPosV))
		gPosV = vcg::tri::Allocator<MeshType>::template AddPerVertexAttribute<GIndex> (m,"ocme_gindex");

	// create an attibute that will store if the  vertex is locked or not
	typename MeshType::template PerVertexAttributeHandle<unsigned char> lockedV =
		vcg::tri::Allocator<MeshType>::template GetPerVertexAttribute<unsigned char> (m,"ocme_locked");

	if(!vcg::tri::Allocator<MeshType>::IsValidHandle(m,lockedV))
		lockedV = vcg::tri::Allocator<MeshType>::template AddPerVertexAttribute<unsigned char> (m,"ocme_locked");

	// create an attibute that will store the address in ocme for the face
	typename MeshType::template PerFaceAttributeHandle<GIndex> gPosF =  
		vcg::tri::Allocator<MeshType>::template GetPerFaceAttribute<GIndex> (m,"ocme_gindex");

	if(!vcg::tri::Allocator<MeshType>::IsValidHandle(m,gPosF) )
		gPosF = vcg::tri::Allocator<MeshType>::template AddPerFaceAttribute<GIndex> (m,"ocme_gindex");

	// create an attibute that will store if the  face is locked or not
	typename MeshType::template PerFaceAttributeHandle<unsigned char> lockedF =  
		vcg::tri::Allocator<MeshType>::template GetPerFaceAttribute<unsigned char> (m,"ocme_locked");

	if(!vcg::tri::Allocator<MeshType>::IsValidHandle(m,lockedF) )
		lockedF = vcg::tri::Allocator<MeshType>::template AddPerFaceAttribute<unsigned char> (m,"ocme_locked");

	// create an attibute that will store the ScaleRange of the mesh
	typename MeshType::template PerMeshAttributeHandle<ScaleRange> range =  
		vcg::tri::Allocator<MeshType>::template GetPerMeshAttribute<ScaleRange> (m,"ocme_range");

	if(!vcg::tri::Allocator<MeshType>::IsValidHandle(m,range))
		range = vcg::tri::Allocator<MeshType>::template AddPerMeshAttribute<ScaleRange> (m,"ocme_range");


	ScaleRange sr; 
	int curr_pos = 0;
	std::vector<Cell*>::iterator ci;

//DEBUG ///////////////////////////////////////////////////////
#ifdef _DEBUG
	for(ci  = sel_cells.begin(); ci != sel_cells.end(); ++ci)
		this->CheckExternalRefAreExternal(*ci);
#endif
//////////////////////////////////////////////////////////////

	bool locked;
	typename MeshType::VertexIterator vi;
	for(ci  = sel_cells.begin(); ci != sel_cells.end(); ++ci){
			sprintf(lgn->Buf(),"loadall %d %d %d %d\n",(*ci)->key.x,(*ci)->key.y,(*ci)->key.z,(*ci)->key.h);
			lgn->Push();
			(*ci)->vert->LoadAll();
			(*ci)->face->LoadAll();
	}

	for(ci  = sel_cells.begin(); ci != sel_cells.end(); ++ci)
		if ( !(*ci)->IsEmpty())
		{
			sr.Add((*ci)->key.h);
			Chain<OVertex> * chain = (*ci)->vert;
			RAssert(chain != NULL);
			vertOffsets.insert ( std::pair<Cell*,int>(*ci,curr_pos));
	 		vi  = vcg::tri::Allocator<MeshType>::AddVertices(m,chain->Size());

			locked = (*ci)->ecd->locked() ;


			// The vertex that was allocated to store a vertex that is not pointed by anything
			// is deleted (refer to the header of ocme_definition.h for more explanation)
			for(unsigned int i = 0; i < chain->Size(); ++i,++vi)
				if((*chain)[i].isExternal){
					gPosV[*vi].SetExternal();
					vcg::tri::Allocator<MeshType>::DeleteVertex(m,*vi);

					lockedV[*vi] = 1;		// set it as locked..It is onlyfor consistency in the debug
				}
				else
					{	
						/* associate the global index to the vertex beign copied */
						gPosV[*vi] = GIndex((*ci)->key,i); 
						/* Here there should be the importer from OVertex to vcgVertex */
						(*vi).P() = (*chain)[i].P();

						/*  export all the attributes specified */
						attr_map.ExportVertex(*ci,*vi,i);

						/* mark with the "editable" flag */
						lockedV[*vi] = locked? 1 : 0 ; /* TODO: just to avoid the not-so-tested class for vector of bool in simple_temporary_data.h*/
					}
			curr_pos += chain->Size();
		}
        lgn->Append("phase 2");

	//// phase 2. Copy all the faces
	//// Here the real work, convert the integer pointer of the faces to pointer to the mesh vertices	
	int ici ;
	for(ici = 0,ci  = sel_cells.begin(); ci != sel_cells.end(); ++ci,++ici)
		if ( !(*ci)->IsEmpty())
	{
		Chain<OFace> *  &	face_chain		= (*ci)->face;					// get the face chain
		Chain<OVertex> * &  vertex_chain	= (*ci)->vert;					// get the vertex chain
		Chain<GIndex> * &   external_chain  = (*ci)->externalReferences;	// get the external_references chain
		RAssert(face_chain!=NULL);
		RAssert(vertex_chain!=NULL);
		RAssert(external_chain!=NULL);

		std::map<Cell*,int>::iterator cell_off = vertOffsets.find(*ci);	// get the offset of the cell
		RAssert(cell_off != vertOffsets.end());
		int offset =  (*cell_off).second;
		locked = (*ci)->ecd->locked();

		unsigned int vp[3];
		for(unsigned int i = 0; i < face_chain->Size(); ++i){
			OFace of = (*face_chain)[i];
			Cell * ext_Cell = NULL;
			int ext_i,vpi=0;

			if(locked)
			for( vpi = 0; vpi < 3; ++vpi){
				
				if( ((*vertex_chain)[ of[vpi] ]).isExternal) // the vertex is external (i.e. stored in another cell)
				{
					GIndex ext_gi = 	(* external_chain )[ (int)(*vertex_chain)[ of[vpi] ].GetIndexToExternal() ];

					// fetch the cell and the index where the vertex is actually stored 
					ext_gi = FindExternalVertexCellIndex(ext_gi , ext_Cell,ext_i);

					// find the offset of that cell
					std::map<Cell*,int>::iterator ext_cell_off = vertOffsets.find(ext_Cell);

					if( (ext_cell_off != vertOffsets.end()) && (!(*ext_cell_off).first->ecd->locked()))
						break;								//it means at least one vertex in inside the cell..
				}
			}
			
			if(!locked ||(vpi<3)) 		// if at least one vertex is in the edit region the face MUST be created
			{
			for( vpi = 0; vpi < 3; ++vpi) 
				if( ((*vertex_chain)[ of[vpi] ]).isExternal) // the vertex is external (i.e. stored in another cell)
				{
					GIndex ext_gi = 	(* external_chain )[ (int)(*vertex_chain)[ of[vpi] ].GetIndexToExternal() ];

					// fetch the cell and the index where the vertex is actually stored 
					ext_gi = FindExternalVertexCellIndex(ext_gi , ext_Cell,ext_i);

					// find the offset of that cell
					std::map<Cell*,int>::iterator ext_cell_off = vertOffsets.find(ext_Cell);

					if((ext_cell_off == vertOffsets.end()))
						{
	 						vi  = vcg::tri::Allocator<MeshType>::AddVertices(m,1);
							lockedV[*vi] = 1;
							gPosV[*vi] = ext_gi;
							(*vi).P() = (*(ext_Cell->vert))[ext_i].P();
							attr_map.ExportVertex(ext_Cell,*vi,ext_i);

							vp[vpi] = m.vert.size()-1;
						}
					else
						vp[vpi] =  (*ext_cell_off).second + ext_i;
					RAssert(!m.vert[vp[vpi]].IsD());
				}
				else
				{
					RAssert(! m.vert[ offset + of[vpi] ].IsD());
					// set the the pointer to it
					vp[vpi] =  offset + of[vpi] ;
				}
				{ 
					/* Add the face  and copy the pointer to the vertices*/				 
					typename MeshType::FaceIterator  fi = vcg::tri::Allocator<MeshType>::AddFaces(m,1);

					/* associate the global index to the face beign copied	*/
					gPosF[*fi] = GIndex((*ci)->key,i);

					(*fi).V(0) = &m.vert[vp[0]]; 
					(*fi).V(1) = &m.vert[vp[1]];
					(*fi).V(2) = &m.vert[vp[2]];

					/* export the sdelected attributes */
					attr_map.ExportFace(*ci, *fi ,i);

					/* mark the face with "locked" flag */
					lockedF[*fi] = locked?1:0;/* TODO: just to avoid the not-so-tested class for vector of bool in simple_temporary_data.h*/
				}		
			}
		}
		}
	sprintf(lgn->Buf(),"end adding faces clock(): %d ", static_cast<int>(clock()));
	lgn->Push(true);

	range() = sr;

	for(ci  = sel_cells.begin(); ci != sel_cells.end(); ++ci){
			(*ci)->vert->FreeAll();
			(*ci)->face->FreeAll();
	}

	{

	typename MeshType::FaceIterator fi;
	typename MeshType::VertexIterator vi;
	unsigned int i;
	// trace the elements that have been taken for editing
	edited_faces.clear();
	edited_vertices.clear();

	i = 0;
	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi,++i)
			if(!(*vi).IsD()  && !lockedV[i])
					edited_vertices.push_back(gPosV[i]);

	i = 0;
	for(fi = m.face.begin(); fi != m.face.end(); ++fi,++i)
			if(!(*fi).IsD()  && !lockedF[i])
					edited_faces.push_back(gPosF[i]);
	}
}

template <class MeshType>
void OCME::Edit(   std::vector<Cell*> & sel_cells, MeshType & m, AttributeMapper attrMap){
	std::vector<Cell*>     dep_cells,to_add;
	std::vector<Cell*>::iterator ci; 

	++generic_bool; // mark cells taken for editing

	/*	extend the set of cells to edit with those in the same 3D space
		ar different levels
	*/
	std::set<CellKey>::iterator cki;
	for(ci = sel_cells.begin(); ci != sel_cells.end(); ++ci)
		for(cki = (*ci)->dependence_set.begin(); cki != (*ci)->dependence_set.end(); ++cki)
			if((*cki).h != (*ci)->key.h){	// it is at a different level
				Cell *  c = GetCell((*cki),false);
				if(c)
					to_add.push_back(c);
			}
	sel_cells.insert(sel_cells.end(),to_add.begin(),to_add.end());
	RemoveDuplicates(sel_cells);

	for(ci = sel_cells.begin(); ci != sel_cells.end(); ++ci)
		if(!(*ci)->generic_bool()){
			(*ci)->generic_bool = FBool(&generic_bool);
			(*ci)->generic_bool = true;
		}


	ComputeDependentCells( sel_cells , dep_cells);
	/* mark the cells NOT sel_cells as locked 
	1) mark all the cells in dep_cells as locked
	2) mark the subset in sel_cells as not locked
	UGLY but practical
	*/
	++lockedMark;
	for(std::vector<Cell*>::iterator ci = dep_cells.begin(); ci != dep_cells.end(); ++ci){
		(*ci)->ecd->locked = FBool(&lockedMark);
		(*ci)->ecd->locked = true;
	}
	for(std::vector<Cell*>::iterator ci = sel_cells.begin(); ci != sel_cells.end(); ++ci) 
		(*ci)->ecd->locked = false;
	
	 dep_cells.insert(dep_cells.end(), sel_cells.begin(),sel_cells.end());
	 Extract(dep_cells,m,attrMap);
}

template <class MeshType>
void OCME::ExtractMesh(MeshType & m){
  std::vector<Cell*>  sel_cells;

  int i=0;

	for( CellsIterator ci = cells.begin(); ci != cells.end(); ++ci,++i)
		sel_cells.push_back((*ci).second);

 Edit(sel_cells,m);
}

#endif
