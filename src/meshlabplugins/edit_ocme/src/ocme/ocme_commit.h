#ifndef _OCME_COMMIT_
#define _OCME_COMMIT_
#include "ocme_definition.h"
#include "ocme_add.h"
#include "boolvector.h"
#include "../utils/std_util.h"

extern unsigned int lockedMark;



// find the removed elements
template <class MeshType>
void OCME::FindRemovedElements( MeshType & m,
                                typename MeshType::template PerVertexAttributeHandle<GISet> &gPosV,
                                typename MeshType::template PerFaceAttributeHandle<GIndex>& gPosF
                        ){

                typename MeshType::FaceIterator fi;
                typename MeshType::VertexIterator vi;
                 

                std::vector<GIndex> committing_faces;
                std::vector<GISet> committing_vertices;

                /*
                        find the GIndex being committed
                */
                
                for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
                {
                                GISet gposv = gPosV [*vi];
                                if(!gposv.IsUnassigned() )
                                          committing_vertices.push_back(gposv);
                }

                
                for(fi = m.face.begin(); fi != m.face.end(); ++fi ){
                                                GIndex gposf = gPosF [*fi];
                                                if(!gposf.IsUnassigned() )
                                                                committing_faces.push_back(gposf);
                                }

                /*
                        find the GIndex that were taken in edit and are not being committed back
                */

                std::vector<GIndex> deleted_faces;
                SetDifference(edited_faces,committing_faces,deleted_faces);
sprintf(lgn->Buf(),"del faces %d",deleted_faces.size());
lgn->Push();
                for(unsigned int i = 0; i < deleted_faces.size(); ++i){
                                Cell* c = GetCell(deleted_faces[i].ck,false);
                                assert(c);
                                c->ecd->deleted_face.SetAsVectorOfMarked();
                                c->ecd->deleted_face.SetMarked(deleted_faces[i].i,true);
                                toCleanUpCells.push_back(c);
                                }

                std::vector<GISet> deleted_vertices;

                SetDifference(edited_vertices,committing_vertices,deleted_vertices);
                for(unsigned int  i = 0; i < deleted_vertices.size(); ++i)
                        for(GISet::iterator di = deleted_vertices[i].begin(); di != deleted_vertices[i].end();++di )
                            {
                                Cell* c = GetCell((*di).first,false);
                                assert(c);
                                c->ecd->deleted_vertex.SetAsVectorOfMarked();
                                c->ecd->deleted_vertex.SetMarked((*di).second,true);
                                toCleanUpCells.push_back(c);
                                }

                ::RemoveDuplicates(toCleanUpCells);


}

template <class MeshType>
void OCME::Commit(MeshType & m, AttributeMapper attr_map){
	typedef typename MeshType::ScalarType ScalarType; 
	typename MeshType::FaceIterator fi; 
	typename MeshType::VertexIterator vi;


	/* first of all take the vcg::attribute storing the GIndex position of the elements in the ocm.
		If this operation fails it means you are trying to commit something you did not take
		in edit with Edit(). In this case the function return, you should have called AddMesh.
	*/

		//// create an attibute that will store the address in ocme
    typename MeshType::template PerVertexAttributeHandle<GISet> gPosVNew =
              vcg::tri::Allocator<MeshType>::template AddPerVertexAttribute<GISet> (m,"gposNew");


    typename MeshType::template PerVertexAttributeHandle<GISet> gPosV =
                vcg::tri::Allocator<MeshType>::template GetPerVertexAttribute<GISet> (m,"ocme_gindex");
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

	typename MeshType::template PerMeshAttributeHandle<std::vector<CellKey> > sel_cells_attr =  
		vcg::tri::Allocator<MeshType>::template GetPerMeshAttribute<std::vector<CellKey> > (m,"sel_cells");
	assert(vcg::tri::Allocator<MeshType>::IsValidHandle(m,sel_cells_attr));



	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi) 
         gPosVNew[vi].Clear();

    FindRemovedElements(m,gPosV,gPosF);


	RecordCellsSetModification();

	// find which scale interval will be associated with this mesh
	ScaleRange srM = ScaleRangeOfMesh(m);

	// find which scale interval has been stored with the mesh at extraction time
	ScaleRange sr = srE();

	int h;
        ++generic_bool;                                                 // mark the cells whose attributes are aligned with attr_map


	/* Run over all the vertices and move those vertices which are in cells not in the interval sr */
        Cell * c = NULL; // current cell. Cache the last used cell because very often this is coherent from vertex to vertex
		


       // Phase 1: delete the vertices that are marked as deleted in the mesh
       lgn->Append("deleting vertices marked as D in the mesh");
     {
        unsigned int ii = 0;
         for(vi = m.vert.begin(); vi != m.vert.end(); ++vi,++ii)
                if(lockedV[*vi]==0)													//   skip if the vertex is not editable
        {
                 GISet   gposv = gPosV[*vi];

                 if(!gposv.IsUnassigned() && (*vi).IsD()){
                     for(GISet::iterator dvi = gposv.begin(); dvi != gposv.end(); ++dvi){
                         Cell * dvc = GetCell((*dvi).first,false);
                         assert(dvc);
                         dvc->ecd->deleted_vertex.SetAsVectorOfMarked();
                         dvc->ecd->deleted_vertex.SetMarked((*dvi).second,true);
                         toCleanUpCells.push_back(dvc);
                     }
                 }
        }
     }
      
	/* Here the main cycle. for each face of the mesh put it in the hashed multigrid */
	for(fi = m.face.begin(); fi != m.face.end(); ++fi) 
		if( lockedF[*fi]==0)						// skip if not editable
		{
			vcg::Box3<ScalarType> facebox;
			CellKey ck;
            GIndex     gposf = gPosF[*fi]; // note: gPosF[] will not be updated because it is won't be used again
                                               // before it is destroyed

		//if( ((&(*fi) - &(*m.face.begin()))%1000) == 0){
		//	sprintf(lgn->Buf(),"committed %d faces  \r",&(*fi) - &(*m.face.begin()));
		//	lgn->Push();
		//}


				if((*fi).IsD()){                // face is deleted
                    if(!gposf.IsUnassigned()){  // it is in the database
                                                // remove it from the db
                        Cell * c = GetCell(gposf.ck,false);
                        RAssert(c);
                        c->ecd->deleted_face.SetAsVectorOfMarked();
                        c->ecd->deleted_face.SetMarked(gposf.i,true);
                        toCleanUpCells.push_back(c);
                     }
                    // it was not in the database. It has been added and remove during editing...forget it
                    continue;
                }

                // compute the cell where to put this face
				{
                    // find in which level the face should be
                    h = ComputeLevel<MeshType>(*fi,srM);

                    if( sr.Include(h) && !gposf.IsUnassigned()) 		// if	"the level the face should be put" is within the range of the mesh
                            h = gposf.ck.h;					// then let it be in the same level it was at extraction time

                    // 2. find the proper cell in the level h
                    /* very first choice, pick the cell that contains the  min corner of the bounding box */
                    for(int i = 0; i < 3 ; ++i) facebox.Add((*fi).V(i)->P());

                    ck = ComputeCellKey(facebox.min,h);

//ck = gposf.ck; // DEBUGGING: prevent the face from migrating

                    if( (!c) || !(c->key == ck))				// check if the current cell is the right one
                            c = GetCell(ck);				// if not update it
			}



                if(gposf.IsUnassigned() ){ // it is a brand new face											// this means this face has been added now
                                gposf.i  =  c -> AddFace(OFace());			// add the face to it	 and
                                gposf.ck =  ck;						// update the GIndex stored in gPosV
                        }else
                        if( !(ck == gposf.ck) )	{
                                RAssert(c->key==ck);
                                toCleanUpCells.push_back(GetCell(gposf.ck,false));		// put gposf.ck in the set of cells that contain removed elements
                                MoveFace(gposf,ck); // note: this function updates gposf
                            }

		/* Set the references to the vertices
		*/
                int vIndex[3];
                for(int i = 0; i < 3 ; ++i){
                        vIndex[i] = gPosV[(*fi).V(i)].Index(ck);
                        RAssert(c->key == ck);
                        RAssert(vIndex[i] < (int) c->vert->Size());
                        // get the index of the vertex in this cell (or -1)
						if(vIndex[i]==-1){ // this vertex was not in ck at edit time
							 vIndex[i] = gPosVNew[(*fi).V(i)].Index(ck);
							 if(vIndex[i]==-1)// not yet added during this commit
	                             vIndex[i] = c-> AddVertex(OVertex(*(*fi).V(i)) );			// no: add the vertex to it
							 gPosVNew[(*fi).V(i)].Add(GIndex(ck,vIndex[i]));	
                         }
                        
                         (*c->face)[gposf.i][i] = vIndex[i];
                         RAssert(vIndex[i] < (int) c->vert->Size());
                     }

                // update the bounding box of the cell c to contain the bbox of the face
                c->bbox.Add(facebox,sr);
		}

		// import all the attributes specified
		for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)if( lockedV[*vi]==0){
			// for the old copies
			for(GISet::CopiesIterator ci = gPosV[*vi].giset.begin();ci != gPosV[*vi].giset.end();++ci){
				if( (c==NULL) || !(c->key == (*ci).first)) c = GetCell((*ci).first,false);
				RAssert(c);
				(*c->vert)[(*ci).second].P() = (*vi).cP();
				attr_map.ImportVertex(c,*vi,(*ci).second);
			}
			// for the new copies
			for(GISet::CopiesIterator ci = gPosVNew[*vi].giset.begin();ci != gPosVNew[*vi].giset.end();++ci){
				if( (c==NULL) || !(c->key == (*ci).first)) c = GetCell((*ci).first,false);
				RAssert(c);
				(*c->vert)[(*ci).second].P() = (*vi).cP();
				attr_map.ImportVertex(c,*vi,(*ci).second);
			}
		}

		/* update gPosV by removing deleted vertices */
			{
				for(vi = m.vert.begin(); vi != m.vert.end(); ++vi){
					GISet toErase;
					for(GISet::CopiesIterator ci = gPosV[*vi].giset.begin(); ci != gPosV[*vi].giset.end();++ci){
						Cell * c = GetCell( (*ci).first,false);
						assert(c);
						c->ecd->deleted_vertex.SetAsVectorOfBool();
						if(c->ecd->deleted_vertex.IsMarked((*ci).second))
							toErase.Add((*ci));
						}
						gPosV[*vi].sub(toErase);
					}
			}

			/* now gPosV + gPosVNew is the updated set cells referring the vertex.
			   By construction, the intersection of the two is empty
			*/


			// compare gPosV e gPosVNew to update the borders
			for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)if(!(*vi).IsD())
			{
				unsigned int bi;
				GISet fresh_added  = gPosVNew[*vi];	

				if( fresh_added.giset.size() + gPosV[*vi].giset.size()>1){	// it is a border vertex
					if(gPosV[*vi].giset.size() > 1)							// it already was a border vertex
						bi  = gPosV[*vi].BI();								// take its global border index	
					else {
						bi = gbi++;											// it is a new border vertex, create a new gbi

						/*  if the vertex was only in a cell it means that it was NOT a border, 
							so  add it to the border of the cell, because it's become a border vertex
						*/
						if(gPosV[*vi].giset.size() == 1){
							GISet::CopiesIterator ci = gPosV[*vi].giset.begin();
							Cell * c = GetCell((*ci).first);
							RAssert(c);
							c->ecd->deleted_vertex.SetAsVectorOfBool();
							RAssert(!c->ecd-> deleted_vertex.IsMarked((*ci).second));

							if(!BorderExists(c,(*ci).second)){
								c->border->AddElem(BorderIndex((*ci).second, bi ));
								RAssert((*ci).second<c->vert->Size());
							}
						}
					}

					/* add the reference to the border for fresh added cells */
					for(GISet::CopiesIterator ci = fresh_added.giset.begin(); ci != fresh_added.giset.end();++ci){
						Cell * c = GetCell((*ci).first);
						RAssert(c);
						c->ecd->deleted_vertex.SetAsVectorOfBool();
						RAssert(!c->ecd->deleted_vertex.IsMarked((*ci).second));
						RAssert(!BorderExists(c,(*ci).second));
						c->border->AddElem(BorderIndex((*ci).second, bi ));
						RAssert((*ci).second < c->vert->Size());
					}

					// add new cell dependencies
					GISet total = fresh_added; 
					total.giset.insert(gPosV[*vi].giset.begin(),gPosV[*vi].giset.end());
					for(GISet::CopiesIterator ci = total.giset.begin(); ci != total.giset.end();++ci) {
						GISet::CopiesIterator ci1 = ci;++ci1;
						for(; ci1 != total.giset.end();++ci1){
							Cell * c_in_commit = GetCell((*ci).first,false);
							Cell * c1_in_commit = GetCell((*ci1).first,false);
							RAssert(c_in_commit);
							RAssert(c1_in_commit);
							this->CreateDependence(c_in_commit,c1_in_commit);
						}
					}
	
				}
			}
		/* mark unreferenced vertices for deletion	in cells included in the selection at edit time 	*/
			{
				std::vector<bool> todel;
				std::vector<bool> border;
				for(unsigned int i = 0; i < sel_cells_attr().size(); ++i){
					Cell * c = GetCell(sel_cells_attr()[i],false);
					RAssert(c);
					todel.resize(c->vert->Size(),true);
					border.resize(c->vert->Size(),false);
					for(unsigned int ii  = 0; ii < c->border->Size(); ++ii)
						border[(*c->border)[ii].vi] = true;

					for(unsigned int fi = 0; fi < c->face->Size(); ++fi){
						for(unsigned int vi  = 0; vi < 3; ++vi)
							todel[(*c->face)[fi][vi]] = false;
					}
					c->ecd->deleted_vertex.SetAsVectorOfMarked();
					for(unsigned int ii = 0; ii < c->vert->Size(); ++ii)
						if(todel[ii] && border[ii])
							c->ecd->deleted_vertex.SetMarked(ii,true);
				}
			}

		lgn->Append("cleaning up");
        if(!toCleanUpCells.empty()){
                RemoveDuplicates		(toCleanUpCells);
                RemoveDeletedFaces		(toCleanUpCells);
                RemoveDeletedBorder		(toCleanUpCells);
                RemoveDeletedVertices	(toCleanUpCells);
       }
 
		{
			for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
				if(!(*vi).IsD() && gPosV[*vi].IsUnassigned() && gPosVNew[*vi].IsUnassigned() ){
					 CellKey ck = ComputeCellKey((*vi).cP(),srM.max);
					 if( (c==NULL) || !(c->key == ck)) {
						 c = GetCellC(ck);
							if(!c->generic_bool()) {                                            // if it is the first occurrence of the cell
								UpdateCellsAttributes(c,attr_map);                              // make sure it contains all the attributes
								c->generic_bool = FBool(&generic_bool);
								c->generic_bool = true;
							}
					 }
					 int pos = c-> AddVertex(OVertex(*vi) );   
					 attr_map.ImportVertex(c,*vi,pos); 
				}

		}
#ifdef _DEBUG
	// DEBUG - check

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
	toCleanUpCells.clear();

        vcg::tri::Allocator<MeshType>::template DeletePerVertexAttribute<GISet>(m,gPosVNew);
        vcg::tri::Allocator<MeshType>::template DeletePerVertexAttribute<GISet>(m,gPosV);
        vcg::tri::Allocator<MeshType>::template DeletePerVertexAttribute<unsigned char>(m,lockedV);

        vcg::tri::Allocator<MeshType>::template DeletePerFaceAttribute<GIndex>(m,gPosF);
        vcg::tri::Allocator<MeshType>::template DeletePerFaceAttribute<unsigned char>(m,lockedF);

        vcg::tri::Allocator<MeshType>::template DeletePerMeshAttribute<ScaleRange>(m,srE);
        vcg::tri::Allocator<MeshType>::template DeletePerMeshAttribute<std::vector<CellKey> >(m,sel_cells_attr);



	StopRecordCellsSetModification();


	this->ClearImpostors(this->touched_cells);		// clear the part of the hierarchy containing the touched cells
	this->FillSamples(this->touched_cells);				// touched_cells also contains the new cells
	this->BuildImpostorsHierarchyPartial(this->touched_cells);
	generic_bool++;
	sprintf(lgn->Buf(),"EndCommit"); lgn->Push();
}
#endif
