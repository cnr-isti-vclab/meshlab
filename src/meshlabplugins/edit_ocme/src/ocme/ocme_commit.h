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
		// create an attibute that will store the address in ocme
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

	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
                gPosVNew[vi].Clear() ;	// set the vertex as unassigned	(i.e. to process)

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
       vcg::Box3<ScalarType> facebox;
	/* Here the main cycle. for each face of the mesh put it in the hashed multigrid */
	for(fi = m.face.begin(); fi != m.face.end(); ++fi) 
		if( lockedF[*fi]==0)						// skip if not editable
	{
		CellKey ck;
                GIndex     gposf = gPosF[*fi]; // note: gPosF[] will not be updated because it is won't be used again
                                               // before it is destroyed

		if( ((&(*fi) - &(*m.face.begin()))%1000) == 0){
			sprintf(lgn->Buf(),"committed %d faces  \r",&(*fi) - &(*m.face.begin()));
			lgn->Push();
		}


                if((*fi).IsD()){                // face is deleted
                    if(!gposf.IsUnassigned()){  // it is in the database
                                                // remove it from the db
                        Cell * c = GetCell(gposf.ck,false);
                        assert(c);
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
                    //ck = gposf.ck; // debugging: let's see if it is only when the face migrates

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
                        vIndex[i] = gPosVNew[(*fi).V(i)].Index(ck);
                        assert(c->key == ck);
                        assert(vIndex[i] < (int) c->vert->Size());
                        // get the index of the vertex in this cell (or -1)
                        if(vIndex[i]==-1){ // this vertex was not in ck at edit time
                             vIndex[i] = c-> AddVertex(OVertex(*(*fi).V(i)) );     // no: add the vertex to it
                             gPosVNew[(*fi).V(i)].Add(GIndex(ck,vIndex[i]));          // record to index
                         }
                         attr_map.ImportVertex(c,*(*fi).V(i),vIndex[i]);           // import all the attributes specified
                         (*c->face)[gposf.i][i] = vIndex[i];
                         assert(vIndex[i] < (int) c->vert->Size());
                     }

                // update the bounding box of the cell c to contain the bbox of the face
                c->bbox.Add(facebox,sr);
                }

		// compare gPosV e gPosVNew to update the borders
		for(vi = m.vert.begin(); vi != m.vert.end(); ++vi){
			if(gPosVNew[*vi].giset.size() > 1)	{		// it is a border vertex
				unsigned int bi;
				GISet fresh_added  = gPosVNew[*vi];

				if(gPosV[*vi].giset.size() > 1)	{		// it also WAS a border vertex
					fresh_added.sub(gPosV[*vi]);		// copies added by this commit
					bi  = gPosV[*vi].BI();				// take its global border index	
 				}
				else
					bi = gbi++;							// it is a new border vertex, create a new gbi

				for(GISet::CopiesIterator ci = fresh_added.giset.begin(); ci != fresh_added.giset.end();++ci){
					Cell * c = GetCell((*ci).first);
					assert(c);
					c->border->AddElem(BorderIndex((*ci).second, bi ));
					assert((*ci).second < c->vert->Size());
				}
			}
			if(gPosV[*vi].giset.size() > 1)	{		// it WAS a border vertex
				GISet removed  = gPosV[*vi];
				removed.sub(gPosVNew[*vi]);			// cells for which vi USED to be a border vertex
				unsigned int bi = removed.BI();
				for(GISet::CopiesIterator ci = removed.giset.begin(); ci != removed.giset.end();++ci){
					Cell * c = GetCell((*ci).first);
					toCleanUpCells.push_back(c);
					assert(c);
					for(unsigned int ii  = 0; ii < c->border->Size(); ++ii)// linear search! [to redo faster]
						if((*(c->border))[ii].bi == bi)
							{
								c->ecd->deleted_border.SetAsVectorOfMarked();
								c->ecd->deleted_border.SetMarked(ii,true);
								break;
							}
				}
			}
		}

        if(!toCleanUpCells.empty()){
                RemoveDuplicates(toCleanUpCells);
                RemoveDeletedBorder(toCleanUpCells);
                RemoveDeletedFaces(toCleanUpCells);
                RemoveDeletedVertices(toCleanUpCells);
        }

        for(std::vector<Cell*>::iterator ci = toCleanUpCells.begin(); ci != toCleanUpCells.end(); ++ci)
                if( !CheckFaceVertexAdj(*ci))
                        CheckFaceVertexAdj(*ci);

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

	vcg::tri::Allocator<MeshType>::template DeletePerVertexAttribute (m,gPosVNew);

	StopRecordCellsSetModification();

	this->ClearImpostors(this->touched_cells);		// clear the part of the hierarchy containing the touched cells
	this->FillSamples(this->touched_cells);				// touched_cells also contains the new cells
	this->BuildImpostorsHierarchyPartial(this->touched_cells);

}
#endif
