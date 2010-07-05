#ifndef _OCME_COMMIT_
#define _OCME_COMMIT_
#include "ocme_definition.h"
#include "ocme_add.h"
#include "boolvector.h"
#include "../utils/std_util.h"

extern unsigned int lockedMark;
extern unsigned int computed_vert2externalsMark;


template <class MeshType>
void OCME::Commit(MeshType & m, AttributeMapper attr_map){
	typedef typename MeshType::ScalarType ScalarType; 
	typename MeshType::FaceIterator fi; 
	typename MeshType::VertexIterator vi;


	/* first of all take the vcg::attribute storing the GIndex position of the elements in the ocm.
		If this operation fails it means you are trying to commit something you did not take
		in edit with Edit(). In this case the function return, you should have called AddMesh.
	*/
	typename MeshType::template PerVertexAttributeHandle<GIndex> gPosV =  
		vcg::tri::Allocator<MeshType>::template GetPerVertexAttribute<GIndex> (m,"ocme_gindex");
	assert(vcg::tri::Allocator<MeshType>::IsValidHandle(m,gPosV));

	typename MeshType::template PerFaceAttributeHandle<GIndex> gPosF =  
		vcg::tri::Allocator<MeshType>::template GetPerFaceAttribute<GIndex> (m,"ocme_gindex");
	assert(vcg::tri::Allocator<MeshType>::IsValidHandle(m,gPosF));

	typename MeshType::template PerMeshAttributeHandle<ScaleRange> srE =  
		vcg::tri::Allocator<MeshType>::template GetPerMeshAttribute<ScaleRange> (m,"ocme_range");

	assert(vcg::tri::Allocator<MeshType>::IsValidHandle(m,srE));

	typename MeshType::template PerVertexAttributeHandle<unsigned char> lockedV =  
		vcg::tri::Allocator<MeshType>::template GetPerVertexAttribute<unsigned char> (m,"ocme_locked");
	assert(vcg::tri::Allocator<MeshType>::IsValidHandle(m,lockedV));

	typename MeshType::template PerFaceAttributeHandle<unsigned char> lockedF =  
		vcg::tri::Allocator<MeshType>::template GetPerFaceAttribute<unsigned char> (m,"ocme_locked");
	assert(vcg::tri::Allocator<MeshType>::IsValidHandle(m,lockedF));

	/* we will temporarily need a copy of the vertex position */
	vcg::SimpleTempData<typename MeshType::VertContainer,GIndex> gPosVOld(m.vert);	// create the temporary data
	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi) gPosVOld[vi] = gPosV[vi];	// copy the attribute		
	
	/* we will (temporarily) need to know the global index of new external vertices.*/

	typedef std::pair<GIndex,GIndex> R2E;				// reference 2 externals/ The first GIndex is to externalReferences, the second to vert
	std::map<GIndex,GIndex> :: iterator r2e_i;		
	std::map<GIndex,GIndex> ref_2_ext;					// store all the new reference to externals

	std::vector<Cell*>	toCleanUpCells,					// cells that contain deleted elements
						toRebind,						// cells that contain moved vertices
						toCleanUpCells_vert2externals;	// cells than contain temporary map from vert to externals

	RecordCellsSetModification();
	// find which scale interval will be associated with this mesh
	ScaleRange srM = ScaleRangeOfMesh(m);

	// find which scale interval has been stored with the mesh at extraction time
	ScaleRange sr = srE();

	int h;

		++generic_bool;                 // mark the cells whose attributes are aligned with attr_map
	++computed_vert2externalsMark ;

	// Phase 1: update all the vertices

	lgn->Append("committing vertices");
	/* Run over all the vertices and move those vertices which are in cells not in the interval sr */
	Cell * c = NULL;														// current cell. Cache the last used cell because very often this is coherent from vertex to vertex
		
#ifdef _DEBUG
	for(CellsIterator ci = cells.begin(); ci != cells.end(); ++ci)
		RAssert((*ci).second->ecd->deleted_face.Empty());


		for(fi = m.face.begin(); fi != m.face.end(); ++fi)
			if( lockedF[*fi]==0){
				if(!(*fi).IsD())
				{
					RAssert(!(*fi).V(0)->IsD());
					RAssert(!(*fi).V(1)->IsD());
					RAssert(!(*fi).V(2)->IsD());
				}
			}else{
					RAssert(lockedV[(*fi).V(0)]!=0);
					RAssert(lockedV[(*fi).V(1)]!=0);
					RAssert(lockedV[(*fi).V(2)]!=0);
			}
#endif

	unsigned int ii = 0;
	 for(vi = m.vert.begin(); vi != m.vert.end(); ++vi,++ii)
		if(lockedV[*vi]==0)													//   skip if the vertex is not editable
	{
                GIndex   gposv = gPosV[*vi];

		if(gposv.IsUnassigned() ){											// this means this vertex has been added 
			if(!(*vi).IsD()){												// if it was deleted before committ just skip it
				CellKey ck( (*vi).P(), sr.min );							// get the CellKey where it should go (no specific reason for sr.min, except lazyness)
				Cell * newc = GetCell(ck);									// get the cell
				if(!newc->generic_bool()){
					this->UpdateCellsAttributes(newc,attr_map);
					newc->generic_bool = FBool(&generic_bool);
				}
				unsigned int pos = gPosV[*vi].i = newc -> AddVertex(OVertex(*vi ) );	// add the vertex to it
				attr_map.ImportVertex(newc,*vi,pos);									// import all the attributes
				gPosV[*vi].ck = ck;														// update the GIndex stored in gPosV
			}
		}	
		else																			// it is not a new vertex
		{
			if(  gposv.IsExternal() )													// if the vertex is external we do not need to do anything
					continue;															// skip to the next vertex

			if( (!c) || (!(c->key ==  gposv.ck)) )										// check if the current cell is the right one
				c = GetCell(gposv.ck);													// if not update it

			assert(!(*(c->vert))[gposv.i].isExternal);
			/*	the vertex was not external and was already in the database				*/	
			if((*vi).IsD())	{						
				/* if it has been deleted then add it to the list of deleted vertices	*/
				c->ecd->deleted_vertex.SetAsVectorOfMarked();
				c->ecd->deleted_vertex.SetMarked(gposv.i,true);
				/* insert the cell among whose have to be cleaned after the commit		*/
				toCleanUpCells.push_back(c);
				continue;
			}
			else{											// the vertex is not new and has not been deleted
				/* There are two reasons for having to move the vertex in another cell : */
				/* A) the range of levels has changed and the level of the cell currently storing the vertex 
				is outside the new range */
				/* OR */
				/* B) the vertex position has been changed */

				/* Find the cell where the vertex should be */
				// check the level
				int new_h = std::max( std::min(srM.min,gposv.ck.h),srM.max);
				/* find out his new CellKey					*/
				CellKey new_cell_key = ComputeCellKey ((*vi).P(),new_h);		

				if(!( gposv.ck  == new_cell_key) ){			// the vertex must be moved
					Cell * new_c = GetCell(new_cell_key);	// find out the new Cell
					assert(!(*(c->vert))[gposv.i].isExternal);
					MoveVertex(gposv,c,new_c);				// move the vertex from cell c to cell new_c and update gposv
					gPosV[*vi] = gposv;                     // gposv has been chaned by MoveVertex
					CreateDependence(c,new_c);				// create a dependence between the cells
					toRebind.push_back(c);					// put c in the set of cells whose vertex have been moved away
					toCleanUpCells.push_back(c);			// put c in the set of cells that contain removed elements
					c = new_c;								// update the pointer of the current cell (we'll see what's better)
					assert(!(*(c->vert))[gposv.i].isExternal);
				}
			}
		}

		/* Here the components of the vertex can be copied */
		assert(!(*(c->vert))[gposv.i].isExternal);
		(*(c->vert))[gposv.i].P() = (*vi).P();

		if(!c->generic_bool()){
			this->UpdateCellsAttributes(c,attr_map);
			c->generic_bool = FBool(&generic_bool);
		}
		attr_map.ImportVertex(c,*vi,gposv.i);			// import all the attributes
	}

	lgn->Append("committing faces");
	
	// Phase 2. update the faces

	/* Here the main cycle. for each face of the mesh put it in the hashed multigrid */
	for(fi = m.face.begin(); fi != m.face.end(); ++fi) 
		if( lockedF[*fi]==0)						// skip if not editable
	{
		CellKey ck;
		vcg::Box3<ScalarType> facebox;

		GIndex     gposf = gPosF[*fi];

		if( ((&(*fi) - &(*m.face.begin()))%1000) == 0){
			sprintf(lgn->Buf(),"committed %d faces  \r",&(*fi) - &(*m.face.begin()));
			lgn->Push();
		}

		if(!(*fi).IsD()){							// phase 1-2 only for non deleted faces
			// 1: find in which level the face should be
			h = ComputeLevel<MeshType>(*fi,srM);

			if( sr.Include(h) ) 					// if	"the level the face should be put" is within the range of the mesh 
				h = gposf.ck.h;						// then let it be in the same level it was at extraction time
				
			// 2. find the proper cell in the level h 
			/* very first choice, pick the cell that contains the  min corner of the bounding box */
			for(int i = 0; i < 3 ; ++i) facebox.Add((*fi).V(i)->P()); 
			 
			ck = ComputeCellKey(facebox.min,h);

			if( (!c) || !(c->key == ck))				// check if the current cell is the right one
				c = GetCell(ck);						// if not update it
		}

		// 3: put the face in the cell (just the object OFace, without specifying the references
		// and update is global index. Consider all the cases (it has been fresh added, it has been deleted, it did not change)
		 
		/* The face is going to be added to cell c (key ck)*/
		if(gposf.IsUnassigned() ){											// this means this face has been added now
			if(!(*fi).IsD()){												// if it was deleted before committ just skip it
				gposf.i  = gPosF[*fi].i  = c -> AddFace(OFace());			// add the face to it	 and
				gposf.ck = gPosF[*fi].ck = ck;								// update the GIndex stored in gPosV
			}else
				continue;													// it was added and deleted before the commit, just skip it
		}else{																// The face was already in the database			
			if( (*fi).IsD() ){
				Cell * oldfCellF = GetCell(gposf.ck,false);					// get the cell where the face was
				RAssert(oldfCellF != NULL);									// it has to exists	
				RAssert(oldfCellF->face->Size() > gposf.i);
				oldfCellF->ecd->deleted_face.SetAsVectorOfMarked();
				oldfCellF->ecd->deleted_face.SetMarked(gposf.i,true);		// mark as deleted
				/* insert the cell among whose have to be cleaned after the commit		*/
				toCleanUpCells.push_back(oldfCellF);
				continue;
			}else
				if( !(ck == gposf.ck) )	{	
					RAssert(c->key==ck);
					toCleanUpCells.push_back(GetCell(gposf.ck,false));		// put gposf.ck in the set of cells that contain removed elements
					MoveFace(gposf,ck);
					gPosF[*fi] = gposf;
				}
		}


		/* Set the references to the vertices
		*/

		// 4.  Set the face-to-vertex references 
		typename MeshType::VertexType ov;
                unsigned int vIndex[3];
                for(int i = 0; i < 3 ; ++i){
			GIndex vp  = gPosV[(*fi).V(i)];	// put in vp the GIndex of the vertex i 	
			
			if(vp.ck  == c->key ){			// the vertex is in the same cell as the face
				vIndex[i] = vp.i;			// just assign the order
			}else{
				/*
					the vertex is not in the same cell as the face but in another cell: vp.ck. 
				*/
				int ext_pos = c->GetExternalReference(vp,false);							// check if the cell already contain an external reference to the vertex
				if(ext_pos == -1){															// no:
 					ext_pos = c->GetExternalReference(vp,true);								//  create a new external reference
					OVertex ov;
					ov.SetIndexToExternal( ext_pos ) ;										//  set the reference to the externals	

 					vIndex[i] = c->AddVertex(ov);											//  add the vertex
					ref_2_ext.insert(R2E(GIndex(c->key,ext_pos),GIndex(vp.ck,vIndex[i])));	//  remember which vertex refers to this new external references

					Cell * extc = GetCell((*c->externalReferences)[ext_pos].ck,false);		//  set the dependency between the cells
					assert(extc);
					CreateDependence(c,extc);
 				}
				else
				{ 
					r2e_i = ref_2_ext.find(GIndex(c->key,ext_pos));						// yes: find which vertex refers to the external reference	
					if(r2e_i==ref_2_ext.end())											// if it fails, it means there was already such a vertex
					{
						if( !c->ecd->computed_vert2externals()){
							c->ComputeVert2Externals();
							c->ecd->computed_vert2externals = FBool(&computed_vert2externalsMark);
							toCleanUpCells_vert2externals.push_back(c);
						}
							vIndex[i] = c->GetVertexPointingToExternalReference(ext_pos);	// Find which vertex refers to ext_pos
					}
					else
						vIndex[i] = (*r2e_i).second.i;									// yes: assign it	
				}
			}
			RAssert(vIndex[i]<c->vert->Size());

			(*c->face)[gposf.i].v[i] = vIndex[i];
		}
		// update the bounding box of the cell c to contain the bbox of the face	
		const Box4 old_box = c->bbox;
		c->bbox.Add(facebox,sr);
	}


	lgn->Append("B5.");
//	PrintCellsWithMarkedFacesTMP();

	// Phase 5. rebind externals so that the external references do not point to external vertex
	// (i.e. single step)

	if(!toRebind.empty()){
		RemoveDuplicates(toRebind);
		Rebind(toRebind);
		std::vector<Cell*> internalRebounded;
		RebindInternal(toRebind, internalRebounded);
		Rebind(internalRebounded);
		toCleanUpCells.insert(toCleanUpCells.end(),internalRebounded.begin(), internalRebounded.end());
	}
        if(!toCleanUpCells_vert2externals.empty()){
		RemoveDuplicates(toCleanUpCells_vert2externals);
		std::vector<Cell*>::iterator ci;
		for(ci = toCleanUpCells_vert2externals.begin(); ci != toCleanUpCells_vert2externals.end();++ci)
                        (*ci)->ecd->vert2externals.clear();

	}

	if(!toCleanUpCells.empty()){
		RemoveDuplicates(toCleanUpCells);
		RemoveDeletedFaces(toCleanUpCells);
		RemoveDeletedVertices(toCleanUpCells);

//DEBUG /////////////////////////////////////////////////////////
#ifdef _DEBUG
		std::vector<Cell*>::iterator ci;
for(ci  = toCleanUpCells.begin(); ci != toCleanUpCells.end(); ++ci)
this->CheckExternalRefAreExternal(*ci);
#endif
// ///////////////////////////////////////////////////////////////

	}
#ifdef _DEBUG
	// DEBUG - check
	for(std::vector<Cell*>::iterator ci = toCleanUpCells.begin(); ci != toCleanUpCells.end(); ++ci)
		if( !CheckFaceVertexAdj(*ci))
			CheckFaceVertexAdj(*ci);

	for(std::vector<Cell*>::iterator ci = toCleanUpCells.begin(); ci != toCleanUpCells.end(); ++ci)
		RAssert((*ci)->ecd->deleted_face.Empty());

	for(CellsIterator ci = cells.begin(); ci != cells.end(); ++ci)
		RAssert((*ci).second->ecd->deleted_face.Empty());

#endif
	// Phase 7. remove empty cells
	/* only a subset of the cells in toCleanUpCells may possibly have become empty */
	for(std::vector<Cell*>::iterator ci = toCleanUpCells.begin(); ci != toCleanUpCells.end(); ++ci)
		if( (*ci)->IsEmpty()){
			sprintf(lgn->Buf(), "remove cell %d %d %d %d\n",(*ci)->key.x,(*ci)->key.y,(*ci)->key.z,(*ci)->key.h);
			lgn->Push();
			RemoveCell((*ci)->key);
		}

	StopRecordCellsSetModification();

	this->ClearImpostors(this->touched_cells);		// clear the part of the hierarchy containing the touched cells
	this->FillSamples(this->touched_cells);				// touched_cells also contains the new cells
	this->BuildImpostorsHierarchyPartial(this->touched_cells);

}
#endif
