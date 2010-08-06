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
	vcg::tri::Stat<MeshType>::ComputeEdgeHistogram(m,mHist);
	minScale = mHist.Percentile(0.5f);
	maxScale = mHist.Percentile(0.95f);
	ScaleRange sr;
       
        sr.min = ComputeLevel(minScale);
	sr.max = ComputeLevel(maxScale);
	return sr;
}

template <class MeshType>
inline int OCME::ComputeLevel( typename MeshType::FaceType & f){
	typedef typename MeshType::ScalarType S;
	S l = 0;
	for(int i = 0; i < f.VN(); ++i)
		l = std::max< S>((S)l,( f.V(i)->P() - f.V((i+1)%3)->P()).SquaredNorm());
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


	sprintf(lgn->Buf(),"Adding Mesh\n");
	lgn->Push();	

	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::ScalarType ScalarType;
	typename MeshType::FaceIterator fi;
	typename MeshType::VertexIterator vi;

	static unsigned int n_duplicate_removal = 0;

	int h;
	Cell * c = NULL; // current cell. Cache the last used cell because very often this is coherent from face to face

	// create an attibute that will store the address in ocme
	typename MeshType::template PerVertexAttributeHandle<GIndex> gPos =  
		vcg::tri::Allocator<MeshType>::template AddPerVertexAttribute<GIndex> (m,"gpos");

        // initialize to unassigned
	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
		gPos[vi].SetUnassigned() ;	// set the vertex as unassigned	(i.e. to process)	
	
	// decide which scale interval will be associated with this mesh
	ScaleRange sr = ScaleRangeOfMesh(m);

	/* NOTE NOTE NOTE
	Since it is not really of much use to have multiple scales for a single mesh
	here we put al the mesh at the same scale
	*/
 	sr.min = sr.max;

	sprintf(lgn->Buf(),"MeshSize face %d  vert%d\n",m.fn,m.vn);
	lgn->Push();	
	sprintf(lgn->Buf(),"ScaleRange %d %d\n",sr.min,sr.max);
	lgn->Push();	


	int n_box_updates = 0;
	std::vector<Cell*> toCleanUp;

	RecordCellsSetModification();

	/* Here the main cycle. for each face of the mesh put it in the hashed multigrid */
	for(fi = m.face.begin(); fi != m.face.end(); ++fi) if(!(*fi).IsD()){

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


		/*  The vertices can be internal to the cell or external									*/
		/*  In the type OVertex there is bool to indicate this (OVertex::isExternal)			*/
		/*  If it is external its global index is put in the vector Cell::externalReferences		*/
		/*  and the position in Cell::externalReferences is stored as the first coordinate of vertex    */
		/*	position. Amen.																			*/

		int vIndex[3];
		for(int i = 0; i < 3 ; ++i){
			GIndex vp  = gPos[(*fi).V(i)];										
			if(  vp.IsUnassigned()){												// check if it has already been assigned
                                CellKey ckv = ComputeCellKey((*fi).V(i)->P(),h);                                    // no: find in which cell is should be stored
                                Cell* ext_c = ( ckv == c->key)?c:	GetCellC(ckv);                              // no: get this cell (caching: try if is the one already in c)
                                added_cells.push_back(ckv);

                                if(!ext_c->generic_bool()) {                                                   // if it is the first occurrence of the cell
                                    UpdateCellsAttributes(ext_c,attr_map);                                          // make sure it contains all the attributes
                                    ext_c->generic_bool = FBool(&generic_bool);
                                }

                                int vpos =  ext_c-> AddVertex(OVertex(*(*fi).V(i)) );                               // no: add the vertex to it
                                attr_map.ImportVertex(ext_c,*(*fi).V(i),vpos);                                      // import all the attributes specified
				
                                vp = gPos[(*fi).V(i)] = GIndex(ckv,vpos);                                           // no: get the GIndex
			}

			// here the vertex has been assigned globally for sure
			if (vp.ck == ck)														// if it has been assigned to the the face cell
				vIndex[i] = vp.i;													// just copy its order
			else{
                                Cell * ext_c = GetCell(vp.ck,false);
                                assert(ext_c != NULL);
                                CreateDependence(c,ext_c);
				int ext_pos = c->GetExternalReference(vp,false);					// check if  the reference to this vertex has already been created
				if(ext_pos!=-1)														// yes
					vIndex[i] =  c->GetVertexPointingToExternalReference(ext_pos);	//   yes: find out the corresponding vertex and assign it
				else{																// no
					ext_pos = c->AddExternalReference(vp);							//    no: add the external references						
					vIndex[i] = c->AddExternalVertex(ext_pos);						//	  no: add the corresponding vertex
					c->ecd->vert2externals.insert(std::pair<unsigned int,unsigned int>(ext_pos,vIndex[i]));
					toCleanUp.push_back(c);
				}
			}

		//	assert(vIndex[i]<c->vert->Size());
			
		}
        // add the face to the cell
                if(!c->generic_bool()) {                                                   // if it is the first occurrence of the cell
                    UpdateCellsAttributes(c,attr_map);                                          // make sure it contains all the attributes
                    c->generic_bool = FBool(&generic_bool);
                }

                unsigned int f_pos =  c->AddFace(OFace(vIndex[0],vIndex[1],vIndex[2]));		// assing the face to the cell
                attr_map.ImportFace( c,  *fi  ,f_pos);                                          // import all the attributes specified

		// rough roughroughourhg adding of samples
		vcg::Point3f bary   = vcg::Barycenter(*fi);
		vcg::Point3f pp[3];
		for(int i  = 0; i < 3; ++i) pp[i]  = ((*fi).V(i))->P();
		c->impostor->AddSample(bary,vcg::Normal(*fi).Normalize());	// collect a sample for the impostor
		for(int i = 0; i < 3; ++i) c->impostor->AddSample(bary*0.5+pp[i]*0.5,vcg::Normal(*fi).Normalize());	// collect a sample for the impostor
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
		if(!(old_box == c->bbox))	n_box_updates++;

		// get the cells at the same level "h" intersected by the bounding box of the face
		OverlappingBBoxes( c->bbox.bbox3, h,overlapping);

		Cell * ovl_cell = NULL;	// pointer to the overlapping cell
		
		for(oi = overlapping.begin(); oi != overlapping.end(); ++oi){
			/* get the overlapping cell. If there is no such overlapping cell it must be created*/
			ovl_cell = GetCellC((*oi),true);			
			added_cells.push_back(*oi);
			//ovl_cell->bbox.Add(facebox, sr);		// update its bounding box

			if(c != ovl_cell)
				CreateDependence(c,ovl_cell);		// connect the two cells
		}
	}


	StopRecordCellsSetModification();

	/* clear the temporary data of the cells*/
	RemoveDuplicates(toCleanUp);
	for(std::vector<Cell*>::iterator i=toCleanUp.begin();i != toCleanUp.end();++i)
                (*i)->ecd->vert2externals.clear();

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
