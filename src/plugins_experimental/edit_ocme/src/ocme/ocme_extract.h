#ifndef _OCME_EXTRACT_
#define _OCME_EXTRACT_


#include "impostor_definition.h"
#include "ocme_definition.h"


extern unsigned int generic_bool;


template <class MeshType>
struct Joiner{

    typedef typename MeshType::VertexPointer VertexPointer;
    typedef typename MeshType::VertexIterator VertexIterator;
    typedef typename MeshType::FaceIterator FaceIterator;

class RemoveDuplicateVert_Compare{
public:
		typename MeshType::template PerVertexAttributeHandle<unsigned int >    * biV;
        inline bool operator()(VertexPointer const &a, VertexPointer const &b)
        {
			return (*biV)[a] < (*biV)[b];
        }
};


static void JoinBorderVertices( MeshType & m, 
								typename MeshType::template PerVertexAttributeHandle<GISet>  & gPosV,
								typename MeshType::template PerVertexAttributeHandle<unsigned int >  & biV,
								typename MeshType::template PerVertexAttributeHandle<unsigned char >  & lockedV)
{
		
        if(m.vert.size()==0 || m.vn==0) return;

        VertexPointer base = &*m.vert.begin();
		static std::vector< int> remap;
		remap.reserve(2000000);
		
		remap.resize(m.vert.size(),-1);


        size_t i,j;
        VertexIterator vi;
        int deleted=0;
        int k=0;
        size_t num_vert = m.vert.size();
        std::vector<VertexPointer> perm;
        for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi, ++k)
			if(biV[*vi] != 0)
                perm.push_back(&(*vi));

		if(perm.empty()) return;

        RemoveDuplicateVert_Compare c_obj;
		c_obj.biV = & biV;

        std::sort(perm.begin(),perm.end(),c_obj);

        j = 0;
        i = j;
         
        ++i;
        for(;i!=perm.size();)
        {
                if( (! (*perm[i]).IsD()) &&
                        (! (*perm[j]).IsD()) &&
                        (biV[*perm[i]] == biV[*perm[j]]) )
                {
                        VertexPointer t = perm[i];
						remap[t-base ] = perm[j]-base;// remap
                        gPosV[*perm[j]].Add(gPosV[*t]);
						if (lockedV[*t]) lockedV[*perm[j]] = 1;
						gPosV[*perm[j]].BI() = biV[*perm[j]]; //store in gPosV the global border index
                        vcg::tri::Allocator<MeshType>::DeleteVertex(m,*t);
                        ++i;
                        deleted++;
                }
                else
                {
                        j = i;
                        ++i;
                }
        }

        FaceIterator fi;
        for(fi = m.face.begin(); fi!=m.face.end(); ++fi)
                if( !(*fi).IsD() )
                        for(k = 0; k < 3; ++k)
 							{
								int rm = remap[(*fi).V(k) - base];
								if(rm!=-1)
									(*fi).V(k)= &m.vert[rm];
							}

        vcg::tri::Allocator<MeshType>::CompactVertexVector(m);
		remap.clear();
    }
};


template <class MeshType>
void OCME::ExtractVerticesFromASingleCell( CellKey ck , MeshType & m){
	Cell* c = GetCell(ck,false);
	typename MeshType::VertexIterator vi;
	Chain<OVertex> * vert = c->vert;
	RAssert(vert != NULL);
	vi  = vcg::tri::Allocator<MeshType>::AddVertices(m,vert->Size());
	vert->LoadAll();
	for(unsigned int i = 0; i < vert->Size(); ++i,++vi)(*vi).P() = (*vert)[i].P();
	vert->FreeAll();
}

template <class MeshType>
 void OCME::ExtractContainedFacesFromASingleCell( CellKey ck , MeshType & m){

    Cell * c = GetCell(ck,false);
    assert(c);

    c->vert->LoadAll();
    c->face->LoadAll();

	Chain<vcg::Color4b> * color_chain = 0;
	std::map<std::string, ChainBase *  >::iterator ai = c->perVertex_attributes.find("Color4b");
	bool hasColor = (ai!=c->perVertex_attributes.end());
	if(hasColor){
		m.vert.EnableColor();
		color_chain = (Chain<vcg::Color4b>*) (*ai).second; 
		RAssert(color_chain!=NULL);
	}
     if ( !c->IsEmpty()){
            Chain<OFace> *  	face_chain	= c->face;
            Chain<OVertex> * vertex_chain = c->vert;

            RAssert(face_chain!=NULL);
            RAssert(vertex_chain!=NULL);

            typename MeshType::VertexIterator  vi  = vcg::tri::Allocator<MeshType>::AddVertices(m,vertex_chain->Size());
            for(unsigned int i = 0; i < vertex_chain->Size(); ++i,++vi)
                    {
                        /* Here there should be the importer from OVertex to vcgVertex */
                        (*vi).P() = (*vertex_chain)[i].P();

						if(hasColor) 
							(*vi).C() = (*color_chain)[i];
						 
                    }


            typename MeshType::FaceIterator fi  = vcg::tri::Allocator<MeshType>::AddFaces(m,face_chain->Size());
            for(unsigned int ff = 0; ff < face_chain->Size();++ff,++fi){
                for(unsigned int i = 0; i < 3; ++i){
                        RAssert((*face_chain)[ff][i] < m.vert.size());
	                    (*fi).V(i) = &m.vert[(*face_chain)[ff][i]];
                   }
            }

          }

           c->vert->FreeAll();
           c->face->FreeAll();
}

template <class MeshType>
void OCME::Extract(   std::vector<Cell*> & sel_cells, MeshType & m, AttributeMapper attr_map){
m.face.EnableColor();
		m.Clear();
		RemoveDuplicates(sel_cells);
		sprintf(lgn->Buf(),"start adding vertices clock(): %d ", static_cast<int>(clock()));
		lgn->Push();

        // create an attibute that will store the address in ocme for the vertex
        typename MeshType::template PerVertexAttributeHandle<GISet> gPosV =
                vcg::tri::Allocator<MeshType>::template GetPerVertexAttribute<GISet> (m,"ocme_gindex");

        if(!vcg::tri::Allocator<MeshType>::IsValidHandle(m,gPosV))
                gPosV = vcg::tri::Allocator<MeshType>::template AddPerVertexAttribute<GISet> (m,"ocme_gindex");

        // create an attibute that will store the address in ocme for the vertex
        typename MeshType::template PerVertexAttributeHandle<unsigned int> biV =
                vcg::tri::Allocator<MeshType>::template GetPerVertexAttribute<unsigned int> (m,"bi");
        if(!vcg::tri::Allocator<MeshType>::IsValidHandle(m,biV))
                biV = vcg::tri::Allocator<MeshType>::template AddPerVertexAttribute<unsigned int> (m,"bi");

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


		typename MeshType::template PerMeshAttributeHandle<std::vector<CellKey> > sel_cells_attr =
			vcg::tri::Allocator<MeshType>::template GetPerMeshAttribute< std::vector<CellKey   > > (m,"sel_cells");
		if(!vcg::tri::Allocator<MeshType>::IsValidHandle(m,sel_cells_attr))
			sel_cells_attr = vcg::tri::Allocator<MeshType>::template AddPerMeshAttribute<std::vector<CellKey> > (m,"sel_cells");



	ScaleRange sr; 
        int offset = 0;
	std::vector<Cell*>::iterator ci;


	bool locked;
	typename MeshType::VertexIterator vi;
	sel_cells_attr().clear();

	for(ci  = sel_cells.begin(); ci != sel_cells.end(); ++ci){
			(*ci)->vert->LoadAll();
			(*ci)->face->LoadAll();
			sel_cells_attr().push_back((*ci)->key);
	}

	sprintf(lgn->Buf(),"All Loaded");
	lgn->Push();

	for(ci  = sel_cells.begin(); ci != sel_cells.end(); ++ci)
		if ( !(*ci)->IsEmpty())
		{
			sr.Add((*ci)->key.h);
			Chain<OVertex> * chain = (*ci)->vert;
			Chain<BorderIndex>  * chain_bi = (*ci)->border;
			RAssert(chain != NULL);


			unsigned int first_added_v =  m.vert.size();
	 		vi  = vcg::tri::Allocator<MeshType>::AddVertices(m,chain->Size());
	

			locked = (*ci)->ecd->locked() ;


			for(unsigned int i = 0; i < chain->Size(); ++i,++vi)
                {
                    gPosV[*vi].Clear();
                    int ind = gPosV[*vi].Index((*ci)->key);
                    assert(ind == -1);

                    /* add this cells to the cells storing this vertices */
                    gPosV[*vi].Add(GIndex((*ci)->key,i));

                    /* Here there should be the importer from OVertex to vcgVertex */
                    (*vi).P() = (*chain)[i].P();
 
                    /*  export all the attributes specified */
                    attr_map.ExportVertex(*ci,*vi,i);

					/* mark with the "editable" flag */
                    lockedV[*vi] = locked? 1 : 0 ; /* TODO: just to avoid the not-so-tested class for vector of bool in simple_temporary_data.h*/
					
					/* initialize the external counter to 0 [maybe unnecessary]*/ 
					biV[*vi] = 0;
			}


			
			/* assign the border index to the border vertices */
			for(unsigned int i = 0; i < chain_bi->Size(); ++i){
				unsigned int _tmp = (*chain_bi)[i].vi;
				RAssert(first_added_v+_tmp <  m.vert.size());
				biV[m.vert[first_added_v+ (*chain_bi)[i].vi] ] = (*chain_bi)[i].bi;
			}

				Chain<OFace> * face_chain	= (*ci)->face;		// get the face chain
				RAssert(face_chain!=NULL);



				typename MeshType::FaceIterator fi  = vcg::tri::Allocator<MeshType>::AddFaces(m,face_chain->Size());
	

				
				for(unsigned int ff = 0; ff < face_chain->Size();++ff,++fi){
vcg::Color4b c; 
c = c.Scatter(255,(*ci)->key.h+127);
(*fi).C() = c;

					gPosF[*fi] = GIndex((*ci)->key,ff);
					lockedF[*fi] = locked? 1 : 0 ;
					for(unsigned int i = 0; i < 3; ++i){
							int iii = ((*face_chain)[ff][i]);
							assert(iii+offset < m.vert.size());
						   (*fi).V(i) = &m.vert[iii+offset];
					   }
				}
				offset = m.vert.size();
				
		}
		
	range() = sr;

	
		for(ci  = sel_cells.begin(); ci != sel_cells.end(); ++ci){
				(*ci)->vert->FreeAll();
				(*ci)->face->FreeAll();
		}
		

		 
        Joiner<MeshType>::JoinBorderVertices(m,gPosV,biV,lockedV);
		 

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
            std::sort(edited_vertices.begin(),edited_vertices.end());

            i = 0;
            for(fi = m.face.begin(); fi != m.face.end(); ++fi,++i)
                            if(!(*fi).IsD()  && !lockedF[i])
                                            edited_faces.push_back(gPosF[i]);
            std::sort(edited_faces.begin(),edited_faces.end());
				
        }

		 
		vcg::tri::Allocator<MeshType>::template DeletePerVertexAttribute (m,biV);
		 
}


template <class MeshType>
bool OCME::Edit(   std::vector<Cell*> & sel_cells, MeshType & m, unsigned int max_size,AttributeMapper attrMap){
	std::vector<Cell*>     dep_cells,to_add,to_add_1;
	std::vector<Cell*>::iterator ci; 

	
	++generic_bool; // mark cells taken for editing

	/*	extend the set of cells to edit with those in the same 3D space
		at different levels
	*/
	std::set<CellKey>::iterator cki;
	for(ci = sel_cells.begin(); ci != sel_cells.end(); ++ci)
		for(cki = (*ci)->dependence_set.begin(); cki != (*ci)->dependence_set.end(); ++cki)
			if((*cki).h != (*ci)->key.h)// it is at a different level
			{	
				Cell *  c = GetCell((*cki),false);
				RAssert(c);
				to_add.push_back(c);
			}
	for(ci = to_add.begin(); ci != to_add.end(); ++ci)
		for(cki = (*ci)->dependence_set.begin(); cki != (*ci)->dependence_set.end(); ++cki)
			if((*cki).h != (*ci)->key.h)// it is at a different level
			{	
				Cell *  c = GetCell((*cki),false);
				RAssert(c);
				to_add_1.push_back(c);
			}

	sel_cells.insert(sel_cells.end(),to_add.begin(),to_add.end());
	sel_cells.insert(sel_cells.end(),to_add_1.begin(),to_add_1.end());
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
	for(std::vector<Cell*>::iterator ci = sel_cells.begin(); ci != sel_cells.end(); ++ci) {
		(*ci)->ecd->locked = false;
	}
	
	 dep_cells.insert(dep_cells.end(), sel_cells.begin(),sel_cells.end());
	 {
		unsigned int to_load = 0;
		for(std::vector<Cell*>::iterator ci = dep_cells.begin(); ci != dep_cells.end(); ++ci) 
			to_load+=(*ci)->face->Size()+(*ci)->vert->Size();
		if( to_load > max_size) 
			return false;
		 
	 }
	

	 Extract(dep_cells,m,attrMap);
	 return true;
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
