#ifndef _OCME_ADD_
#define _OCME_ADD_

#include "impostor_definition.h"
#include "ocme_definition.h"
#include "cell_attributes.h"


#include "FBool.h"
#include <hash_set>
 
extern unsigned int  generic_bool;

template <class S>
ScaleRange OCME::ScaleRangeOfTris( std::vector<vcg::Point3<vcg::Point3<S> > > & tris){
	float minScale,maxScale;
	vcg::Distribution<float> mHist;

	for(unsigned int y = 0 ; y < tris.size(); ++y)
		for(unsigned int e = 0 ; e < 3; ++e)
			mHist.Add(vcg::Distance(tris[y][e],tris[y][(e+1)%3]));

	minScale = mHist.Percentile(0.5f);
	maxScale = mHist.Percentile(0.95f);
	ScaleRange sr;
	sr.min = ComputeLevel(minScale);
	sr.max = ComputeLevel(maxScale);
	return sr;
}

template <class MeshType>
ScaleRange OCME::ScaleRangeOfMesh( MeshType & m){
	typename MeshType::ScalarType minScale,maxScale;

	vcg::Histogramf mHist;
	ScaleRange sr;
	if(m.fn * 3>  m.vn){ // use the faces
		vcg::tri::Stat<MeshType>::ComputeEdgeHistogram(m,mHist);
		minScale = mHist.Percentile(0.5f);
		maxScale = mHist.Percentile(0.95f);
		
		sr.min = ComputeLevel(minScale);
		sr.max = ComputeLevel(maxScale);
	}
	else{// use the vertices
        mHist.Clear();
        mHist.SetRange( 0, m.bbox.Diag(), 10000);
                typename MeshType::VertexIterator vi  = m.vert.begin();
                typename MeshType::VertexIterator vi1 =  vi; vi++;
		for( ; vi != m.vert.end(); ++vi,++vi1) 
			mHist.Add(vcg::Distance((*vi).P(),(*vi1).P()));
		 
		minScale = mHist.Percentile(0.15f);
		maxScale = mHist.Percentile(0.70f);
		sr.min = ComputeLevel(minScale);
		sr.max = ComputeLevel(maxScale);
	}
	return sr;
}

template <class MeshType>
inline int OCME::ComputeLevel( typename MeshType::FaceType & f){
	typedef typename MeshType::ScalarType S;
	S l = 0;
	for(int i = 0; i < f.VN(); ++i)
		l = std::max< S>((S)l,( f.V(i)->P() - f.V((i+1)%3)->P()).Norm());
	// compute h so that 2^h = SIDE_FACTOR * sqrt(l)
	return ComputeLevel(l);
}

template <class MeshType>
inline int OCME::ComputeLevel( typename MeshType::FaceType & f, ScaleRange  & sr ){
	// compute h so that 2^h = SIDE_FACTOR * sqrt(l)
	return std::max(std::min(this->ComputeLevel<MeshType>(f),sr.max),sr.min);
}




template <class MeshType>
void OCME::AddMesh( MeshType & m, AttributeMapper attr_map){
	if(m.vn==0) return;
	sprintf(lgn->Buf(),"Adding Mesh\n");
	lgn->Push();	

	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::ScalarType ScalarType;
	typename MeshType::FaceIterator fi;
	typename MeshType::VertexIterator vi;

	static unsigned int n_duplicate_removal = 0;
    int n_box_updates = 0,added_vert = 0;
	int h;
	Cell * c = NULL; // current cell. Cache the last used cell because very often this is coherent from face to face

	// create an attibute that will store the address in ocme
    typename MeshType::template PerVertexAttributeHandle<GISet> gPos =
            vcg::tri::Allocator<MeshType>::template AddPerVertexAttribute<GISet> (m,"gpos");

        // initialize to unassigned
	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
                gPos[vi].Clear() ;	// set the vertex as unassigned	(i.e. to process)
	
	// decide which scale interval will be associated with this mesh
	ScaleRange sr = ScaleRangeOfMesh(m);

	/* NOTE NOTE NOTE
	Since it is not really of much use to have multiple scales for a single mesh
	here we put al the mesh at the same scale
	*/
 	sr.min = sr.max;

/* begin davidone patch */
//if( (sr.max>10) || (sr.max<6))//davidone patch
//sr.max = 10;
/* end davidone patch */

	sprintf(lgn->Buf(),"MeshSize face %d  vert%d\n",m.fn,m.vn);
	lgn->Push();	
	sprintf(lgn->Buf(),"ScaleRange %d %d\n",sr.min,sr.max);
	lgn->Push();	



	RecordCellsSetModification();

    bool hasColor = vcg::tri::HasPerVertexColor(m);

	/* Here the main cycle. for each face of the mesh put it in the hashed multigrid */
	if(m.fn)
	for(fi = m.face.begin(); fi != m.face.end(); ++fi) if(!(*fi).IsD()){
		TIM::Begin(20);
		if(added_cells.size() > (n_duplicate_removal+1)*1000){
				::RemoveDuplicates(this->added_cells);
				++n_duplicate_removal;
		}

		// 1: find the proper level
		h = ComputeLevel<MeshType>(*fi,sr);

		// 2. find the proper cell in the level h 
		/* very first choice, pick the cell that contains the  min corner of the bounding box */
		vcg::Box3<ScalarType> facebox;
		for(int i = 0; i < 3 ; ++i) facebox.Add((*fi).V(i)->P()); 
		 
		CellKey ck = ComputeCellKey(facebox.min,h);

		// 3. now we know that the cell where this face must to be put is "ck". Set che vertex pointers. 
		if( (c==NULL) || !(c->key == ck)){		// check if the current cell is the right one
			c = GetCellC(ck);					// if not update it
			added_cells.push_back(ck);
                }

                if(!c->generic_bool()) {                                            // if it is the first occurrence of the cell
                    UpdateCellsAttributes(c,attr_map);                              // make sure it contains all the attributes
                    c->generic_bool = FBool(&generic_bool);
                    c->generic_bool = true;
                }


		int vIndex[3];
		for(int i = 0; i < 3 ; ++i){
                         vIndex[i] = gPos[(*fi).V(i)].Index(ck);                                          // get the index of the vertx in this cell (or -1)
                        if(vIndex[i]==-1){
							 added_vert++;	
                             vIndex[i] = c-> AddVertex(OVertex(*(*fi).V(i)) );                               // no: add the vertex to it
                             gPos[(*fi).V(i)].Add(GIndex(ck,vIndex[i]));                                     // record to index
                         }

                         attr_map.ImportVertex(c,*(*fi).V(i),vIndex[i]);                                      // import all the attributes specified
                        }
		TIM::Begin(22);
        // add the face to the cell
        unsigned int f_pos =  c->AddFace(OFace(vIndex[0],vIndex[1],vIndex[2]));		// assing the face to the cell
        attr_map.ImportFace( c,  *fi  ,f_pos);                                      // import all the attributes specified
		TIM::End(22);

		TIM::Begin(23);
		// rough roughroughourhg adding of samples

		// vcg::Point3f bary   = vcg::Barycenter(*fi);
		 vcg::Point3f bary   = (*fi).V(0)->cP();
         vcg::Color4b color  =  (hasColor)? (*fi).V(0)->cC() : vcg::Color4b::Gray;

         c->impostor->AddSample(bary,vcg::Normal(*fi).Normalize(),color);	// collect a sample for the impostor


		TIM::End(23);
		/////////////////////////////////////////////////////

		/* Handling the bounding boxes of the cell.
		The bounding box 3 of a cell must include all the faces that cross that cell and that are
		assigned to the same level. 
		The fourth dimension of the bounding box encodes the level and it will be used at editing time.
	
		When a cell "c" must be enable for editing, we will have to load all the cells of the same level
		that intersect c. Furthermore we will have to load the upper and lower levels as written in sr (ScaleRange).

		This will guarantee the correct connectivity. For what concerns the geometry, we can decide to load all
		the cells from the set to the root and some more  lower levels. The key idea is that you are not enabled
		to change what you do not see.
		*/

		// update the bounding box of the cell c to contain the bbox of the face	
		const Box4 old_box = c->bbox;
		c->bbox.Add(facebox,sr);
		
		// if the bounding box has enlarged then expand the bounding box of all the cells
		// touched by the bounding box to include the face
		std::vector<CellKey> overlapping;
		std::vector<CellKey>::iterator oi;

		// count the number of times the bounding box is expended (just to know it)
		if(!(old_box == c->bbox) )
			n_box_updates++;

			TIM::Begin(21);
			// get the cells at the same level "h" intersected by the bounding box of the face
			OverlappingBBoxes( c->bbox.bbox3, h,overlapping);

			Cell * ovl_cell = NULL;	// pointer to the overlapping cell
			
			for(oi = overlapping.begin(); oi != overlapping.end(); ++oi){
				/* get the overlapping cell. If there is no such overlapping cell it must be created*/
				ovl_cell = GetCellC((*oi),true);			
				if(c != ovl_cell)
					CreateDependence(c,ovl_cell);		// connect the two cells
			}
			TIM::End(21);
		TIM::End(20);
	}
	{
		// adding unreferenced vertices (the referenced vertices have already been added in the face cycle
		if(added_vert<m.vn) // is there are no deleted and no unreferenced vertices, 
							// this avoids to run over the vertices just to check
			for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)if(!(*vi).IsD()){
				if(gPos[*vi].giset.empty()){// if it was not assigned it means is unreferenced
					 CellKey ck = ComputeCellKey((*vi).cP(),sr.max);
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
					 gPos[ *vi].Add(GIndex(ck,pos));        

					 vcg::Color4b color  =  (hasColor)? (*vi).cC() : vcg::Color4b::Gray;
					 c->impostor->AddSample((*vi).cP(),(*vi).cN(),color);	// collect a sample for the impostor
					 c->bbox.Add((*vi).cP(),sr.max);
				}
		}
	}
	
	StopRecordCellsSetModification();

	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
		if(gPos[*vi].giset.size() > 1){
			for(GISet::CopiesIterator ci = gPos[*vi].giset.begin(); ci != gPos[*vi].giset.end();++ci){
				Cell * c = GetCell((*ci).first);
				assert(c);
				c->border->AddElem(BorderIndex((*ci).second, gbi ));
			}
			gbi++;
		}


	vcg::tri::Allocator<MeshType>::template DeletePerVertexAttribute (m,gPos);

}


template <class ATTR_TYPE>
Chain<ATTR_TYPE>  *  OCME::AddStringChain( Cell::StringChainMap * sce, std::string name, Cell *c){
	std::string name_of_chain = NameOfChain(c->key,name);

	typedef std::map<std::string, ChainBase *>::iterator ChainIterator;
	std::pair<ChainIterator,bool> res;

	Chain<ATTR_TYPE>  *newch = oce.GetChain<ATTR_TYPE>(name_of_chain,true);
	newch->saveOnce = streaming_mode;
	res =  sce->insert(std::pair<std::string,ChainBase*> (name, (ChainBase*)newch) );
	assert(res.second);
	return newch;
}

template <class ATTR_TYPE>
Chain<ATTR_TYPE>  *  OCME::AddElement(  std::string name, Cell *c){
	return AddStringChain<ATTR_TYPE>(&c->elements,name,c);
}

template <class ATTR_TYPE>
Chain<ATTR_TYPE>  *  OCME::AddPerVertexAttribute(  std::string name, Cell *c){
	return AddStringChain<ATTR_TYPE>(&c->perVertex_attributes,name,c);
}

template <class ATTR_TYPE>
Chain<ATTR_TYPE>  *  OCME::AddPerFaceAttribute(  std::string name, Cell *c){
	return AddStringChain<ATTR_TYPE>(&c->perFace_attributes,name,c);
}

#endif
