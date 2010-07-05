#include "ocme_disk_loader.h"

#include "../utils/memory_debug.h"

// TODO include must be taken out of the way.....
#include "../ooc_vector/berkeleydb/ooc_chains_berkeleydb.hpp"
//...............................................


#include "ocme_definition.h"

//#include "gindex_chunk.h"

#include "ocme_impostor.h"
#include "import_ocm_ply.h"
#include "../utils/logging.h"
#include "../utils/std_util.h"


unsigned int kernelSetMark;
unsigned int lockedMark;
unsigned int computed_vert2externalsMark;
unsigned int impostor_updated;
unsigned int to_render_fbool;
unsigned int generic_bool;

vcg::Point4f corners[8];
double cellsizes[30];

Logging * olgn;
Logging * lgn;
 
void InitCorners(){
	for(int i = 0 ; i <  8; ++i){
		corners[i][0] = -1.0 + (i%2)*2;
		corners[i][1] = -1.0 + ((i/2)%2)*2;
		corners[i][2] = -1.0 + (i/4)*2;
		corners[i][3] = 1.0;
	}
}

void InitCellSizes(){
	for(int i = 0; i < 15;++i)
		cellsizes[15+i] = 1<<i;
	for(int i = 1; i <= 15;++i)
		cellsizes[15-i] = 1/ (double)(1<<i);
}

double CS(const int & h){ return cellsizes[15+h]; }
int COff(const int & h){ return 15+h; }


std::pair < OCME::CellsIterator ,bool >  CachedMap::insert( const  std::pair< CellKey,Cell*>  &  toinsert){
			return allcells.insert(toinsert);
	}

Cell* CachedMap::find(const CellKey & ck){
				 CellsIterator ci ;

				 cache_access.lock();
				 ci = cache.find(ck);// look in the cache
				 if(ci!=cache.end()){
						 cache_access.unlock();
						 return (*ci).second;     // found! return it
				 }

				 ci = allcells.find(ck);           // look in allcells
				 if(ci==allcells.end()){
						 cache_access.unlock();
						 return 0;     // if there is not, return
				 }
			//	 lgn->Append("in cells");lgn->Push();

				assert(ci != allcells.end());

				 if(cache.size()>100) cache.clear();

				 cache.insert(std::pair<CellKey,Cell*>(ck,(*ci).second));
				 cache_access.unlock();
				 return (*ci).second;
		}
		 void  CachedMap::erase( const CellKey & ck){
				 cache_access.lock();
				 cache.erase(ck);
				 allcells.erase(ck);
				 cache_access.unlock();
		 }

		 OCME::CellsIterator  CachedMap::begin()		{ return allcells.begin();}
		 OCME::CellsIterator  CachedMap::end()			{ return allcells.end();}
		size_t	       CachedMap::size()    { return allcells.size();}




OCME::OCME(){
					lgn = new Logging("edit_ocme_log.txt");
					this->oce.AddNameTypeBound<GIndex>("GIndex");
					this->oce.AddNameTypeBound<OFace>("OFace");
					this->oce.AddNameTypeBound<OVertex>("OVertex");

					this->oce.AddNameTypeBound<vcg::Point3f>("Coord3f");
					this->oce.AddNameTypeBound<vcg::Color4b>("Color4b");
					this->oce.AddNameTypeBound<vcg::Point3f>("Normal3f");

                    InitCellSizes();
                    InitCorners();
//				InitRender();
                    //this->oce.CreateFromString_UserTypes = &ocme_types_allocator;


                    params.side_factor = 50;
                    kernelSetMark = 1;
                    lockedMark = 1;
                    computed_vert2externalsMark = 1;
                    impostor_updated = 1;
                    to_render_fbool = 1;
                    streaming_mode = false;
										record_cells_set_modification = false;
                    renderParams.one_level = false;
                    renderParams.level = 0;
										renderCache.Init(this);
										renderCache.Start();
	}

OCME::~OCME(){
		renderCache.Finish();
		renderCache.controller.caches.clear();
	for(CellsIterator mi = this->cells.begin(); mi != this->cells.end(); ++mi)
		delete (*mi).second;

	//disk_loader->Exit();
	//while(!	disk_loader->isFinished());
	//delete disk_loader;
}


void OCME::RecordCellsSetModification(){
		record_cells_set_modification = true;
		added_cells.clear();
		removed_cells.clear();
		touched_cells.clear();
}
void OCME::StopRecordCellsSetModification(){
		record_cells_set_modification = false;
		::RemoveDuplicates(removed_cells);
		::RemoveDuplicates(added_cells);
		::RemoveDuplicates(touched_cells);

		std::vector<CellKey> output;

		output.reserve(added_cells.size());
		 std::set_difference(added_cells.begin(),added_cells.end(),removed_cells.begin(),removed_cells.end(),std::back_inserter(output));
		added_cells = output;

		output.reserve(touched_cells.size());
		 std::set_difference(touched_cells.begin(),touched_cells.end(),removed_cells.begin(),removed_cells.end(),std::inserter(output,output.end()));
		touched_cells = output;
}

void OCME::MarkTouched(const CellKey & key ){
		if(!record_cells_set_modification) return;
		touched_cells.push_back(key);
}

// if record_cells_set_modification is recording record key ahs been touched and removed
void OCME::MarkRemoved(const CellKey & key ){
		if(!record_cells_set_modification) return;
		removed_cells.push_back(key);
}

// if record_cells_set_modification is recording record key ahs been touched and removed
void OCME::MarkTouchedAdded(const CellKey & key ){
		if(!record_cells_set_modification) return;
		touched_cells.push_back(key);
		added_cells.push_back(key);
}

int OCME::ComputeLevel(const float & l){
	return   (int)( std::log(((float)this->params.side_factor) *  float(l)   ) / log(2.f)); 
}

void OCME::MoveVertex(GIndex & gposv,  Cell *&  c,   Cell *& new_c){
		const unsigned int v_id = gposv.i;
		OVertex   v = (*c->vert)[v_id];
		assert( !v.isExternal);

		/*update the value of the GIndex and add  a copy of the vertex to the destination cell */
		gposv.ck  = new_c->key;
		gposv.i   = new_c->AddVertex(v);

		assert(!(*(new_c->vert))[gposv.i].isExternal);

		/* add the reference to the new vertex */
		int ext_i = c->AddExternalReference(gposv);
		/* set the vertex as external */
		(*c->vert)[v_id].SetIndexToExternal(ext_i);
}

void OCME::MoveFace(GIndex & from, const CellKey &  to){


	Cell * cellFrom = GetCell(from.ck,false);
	Cell * cellTo	= GetCell(to,false);
	assert(cellFrom);
	assert(cellTo);
	
	cellFrom->ecd->deleted_face.SetAsVectorOfMarked();
	cellFrom->ecd->deleted_face.SetMarked(from.i,true);	//delete the copy in the old cell

	from.i = cellTo->AddFace(  OFace(-1,-1,-1));	//add a face, the references will be set later on
	from.ck = to;
	
	cellTo->bbox.sr.Add(to.h);				// extend the scale range of the to cell to include to.h

	/*
	This is an important bit. The scale range of the two cells must be reciprocally extended
	with the scalerange of the other to guarantee the dependence property (see comment of ScaleRange in cell.h)
	*/
 	cellTo->bbox.sr.Add(cellFrom->bbox.sr);
 	cellFrom->bbox.sr.Add(cellTo->bbox.sr);


}

//void OCME::MoveFace(GIndex & gposf,  Cell *&  c,   Cell *& new_c){}	

std::string ToString(CellKey key){
	char n[255];
	sprintf(&n[0],"cell_%d_%d_%d_%d_",key.x,key.y,key.z,key.h);
	return std::string(n);
}


CellKey OCME::Parent(const CellKey & ck){
	return CellKey(ck.x>>1,ck.y>>1,ck.z>>1,ck.h+1);
}

void OCME::Children(const CellKey & ck, std::vector<CellKey> & children){
	for(int i = 0; i < 2; ++i)
		for(int j = 0; j < 2; ++j)
			for(int k = 0; k < 2; ++k)
				children.push_back(CellKey((ck.x<<1)+i,(ck.y<<1)+j,(ck.z<<1)+k,ck.h-1));
}

unsigned char OCME::ChildrenBit(const CellKey & ck){
	static char order_from_delta[2][2][2] = {{{4,0},{7,3}},{{5,1},{6,2}}};
	vcg::Point3i delta = ck.P3i()-Parent(ck).P3i();
	return order_from_delta[delta[0]][delta[1]][delta[2]];
}

void OCME::OverlappingBBoxes4( Cell * cl, const Box4 & b, const int & h,std::vector<CellKey> & overlapping){
	cl = cl;	 
	CellKey minc = ComputeCellKey(b.bbox3.min,h);
	CellKey maxc = ComputeCellKey(b.bbox3.max,h);

 
//	minc = maxc = cl->key;

	/* corners4 will contain the 8 extremes of the bounding box at each level*/
	std::vector<vcg::Box3i > boxes;
//	boxes.resize( (b.sr.max-b.sr.min)+1);
	boxes.resize( 32 );

	/* boxes contains the bounding boxes at the varius levels, expressed in term of cell's position*/
	boxes[COff(h)] = vcg::Box3i(vcg::Point3i(minc.x,minc.y,minc.z), vcg::Point3i(maxc.x,maxc.y,maxc.z)) ;

	/* Fill the lower levels*/
	for( int l = h-1; l >= b.sr.min; --l)
			boxes[COff(l)] = vcg::Box3i(
									vcg::Point3i(boxes[COff(l+1)].min[0] <<1,boxes[COff(l+1)].min[1]<<1,boxes[COff(l+1)].min[2]<<1),
									vcg::Point3i((boxes[COff(l+1)].max[0]<<1)+1,(boxes[COff(l+1)].max[1]<<1)+1,(boxes[COff(l+1)].max[2]<<1)+1)
									);


	/* Fill the upper levels*/
	for( int l = h+1; l <= b.sr.max; ++l)
			boxes[COff(l)] = vcg::Box3i	(
									vcg::Point3i(boxes[COff(l-1)].min[0]>>1,boxes[COff(l-1)].min[1]>>1,boxes[COff(l-1)].min[2]>>1),
									vcg::Point3i(boxes[COff(l-1)].max[0]>>1,boxes[COff(l-1)].max[1]>>1,boxes[COff(l-1)].max[2]>>1)
									);

	/* run among all the levels*/
	for( int l = b.sr.min; l <= b.sr.max; ++l)
		for( int i = boxes[COff(l)].min[0]; i <=  boxes[COff(l)].max[0]; i++)
			for( int j = boxes[COff(l)].min[1]; j <=  boxes[COff(l)].max[1]; j++)
				for( int k = boxes[COff(l)].min[2]; k <=  boxes[COff(l)].max[2]; k++)
					overlapping.push_back(CellKey(i,j,k,l));
}


void OCME::OverlappingBBoxes( const vcg::Box3f & b, const int & h,std::vector<CellKey> & overlapping){
 
	 vcg::Box3f   bo = b;
	 bo.Offset(b.Diag()*0.05);
	CellKey minc = ComputeCellKey(bo.min,h);
	CellKey maxc = ComputeCellKey(bo.max,h);

	for( int i = minc.x; i <=  maxc.x; i++)
		for( int j = minc.y; j <=  maxc.y; j++)
			for( int k = minc.z; k <=  maxc.z; k++)
				overlapping.push_back(CellKey(i,j,k,h));
}

void OCME::CreateDependence(Cell * & c1, Cell * & c2){
	if( c1->dependence_set.find(c2->key) == c1->dependence_set.end()){
		c1->dependence_set.insert(c2->key);
		c2->dependence_set.insert(c1->key);
	}
}

Cell* OCME::NewCell(const CellKey & key){
		std::pair < CellsIterator ,bool > res = cells.insert(std::pair<CellKey,Cell*>(key,new Cell(key)));
		return  (*(res.first)).second;	
}

Cell* OCME::GetCell(const CellKey & key,bool ifnot_create){
		stat.n_getcell++;
//		Cell * c = cells.find(key);
		Cell * c;
		CellsIterator ci = cells.find(key);
		c = (ci == cells.end())?0:(*ci).second;
		if(!c){
			if(ifnot_create) {
				Cell * newc =  NewCell(key);;
				newc->face				= AddElement<OFace>		("f",newc);
				newc->vert				= AddElement<OVertex>	("v",newc);
				newc->externalReferences= AddElement<GIndex>	("e",newc);
				if(this->record_cells_set_modification) {
						this->added_cells.push_back(key);
						this->touched_cells.push_back(key);
				}
				return newc;
			}
			else 
				return NULL;	
		} else { assert(key== c->key);return  c;}
	}

Impostor* GetImpostor(const CellKey & key, bool ifnot_create = true){
		assert(0);
		return 0;// not implemented yet
		ifnot_create = false;
}

Cell* OCME::GetCellC(const CellKey & key,bool ifnot_create){
		stat.n_getcell++;
		CellsIterator ci = cells.find(key);
	//	Cell *  c = cells.find(key);
		Cell *  c = (ci==cells.end())?0:(*ci).second;
		if(c) { assert(key==c->key);return c;}

		if(ifnot_create) {
				Cell * newc =  NewCell(key);;
				newc->face				= AddElement<OFace>		("f",newc);
				newc->vert				= AddElement<OVertex>	("v",newc);
				newc->externalReferences= AddElement<GIndex>	("e",newc);
				newc->impostor->InitDataCumulate(key.BBox3f());
				return newc;
		}

		return NULL;
	}

GIndex OCME::FindExternalVertexGIndex(GIndex gi ){
		RAssert(gi.i>=0);
		Cell * curr_cell = NULL;
		curr_cell = this->GetCell(gi.ck,false);																// find the cell 

                if(curr_cell==NULL)  {
                    sprintf(lgn->Buf(),"g: %d %d %d %d , %d",gi.ck.x,gi.ck.y,gi.ck.z,gi.ck.h,gi.i );lgn->Push();
                    RAssert(curr_cell!=NULL);
                }
		const OVertex  & ov = (*(curr_cell->vert))[gi.i];													// fetch the vertex 
		if (ov.isExternal)																					// if it is external again
		 	 return FindExternalVertexGIndex((*(curr_cell->externalReferences))[(int)ov.GetIndexToExternal()]);	// recur
		  else
			 return gi;																						// otherwise we are done
	}
	GIndex OCME::FindExternalVertexCellIndex(GIndex gi,Cell *& c, int & i){
			GIndex g = FindExternalVertexGIndex( gi );
			c = this->GetCell(g.ck,false);
                        if(c==NULL) {
                            sprintf(lgn->Buf(),"g: %d %d %d %d , %d",g.ck.x,g.ck.y,g.ck.z,g.ck.h,g.i );lgn->Push();
                            RAssert(c!=NULL);
                        }
			i = g.i;
			return g;
	}

	void OCME::Rebind(std::vector<Cell*>   & toRebindCells){

		std::vector<Cell*>   dep_cells;

		// compute the dependent set
		ComputeDependentCells(toRebindCells,dep_cells);
		toRebindCells.insert(toRebindCells.end(),dep_cells.begin(),dep_cells.end());
		RemoveDuplicates(toRebindCells);

		std::vector<Cell*>::iterator ci;
		for(ci  = dep_cells.begin(); ci !=  dep_cells.end(); ++ci) {
			(*ci)->ecd->deleted_vertex.SetAsVectorOfBool();
			for(unsigned int i  = 0 ; i < (*((*ci)->vert)).Size(); ++i)
				if( !(*ci)->ecd->deleted_vertex.IsMarked(i) )
					if( (*(*ci)->vert)[i].isExternal)
						{
                                                    Cell * c; int id;
                                                    int ei = (*(*ci)->vert)[i].GetIndexToExternal();
                                                    GIndex   gi = (*((*ci)->externalReferences))[ei];
                                                    gi = this->FindExternalVertexCellIndex(gi,c,id);
                                                    (*((*ci)->externalReferences))[ei] = gi;

                                                    /* it may happen that a vertex that was moved away from a cell is then moved
                                                    back to it. */
                                                    if(*ci != c)				// a cell is implicitly dependent on itself
                                                            CreateDependence(*ci,c);
						}
				}
	}
void OCME::RebindInternal(std::vector<Cell*>  toRebindCells, std::vector<Cell*>  & toCleanUpCells){
	std::vector<Cell*>::iterator ci;

	for(ci  = toRebindCells.begin(); ci != toRebindCells.end(); ++ci){
		/*	run over all the faces and update those pointers to vertex that are external
			but refer to another vertex in the same cell
		*/
 
		for(unsigned int fi = 0 ; fi < (*ci)->face->Size(); ++fi)
			for(unsigned int vi  = 0 ; vi < 3; ++vi){
				unsigned int viii = (*(*ci)->face)[fi][vi];


                                RAssert(viii < (*ci)->vert->Size());
//DEBUG
if(viii >= (*ci)->vert->Size()){
    sprintf(lgn->Buf(),"%d %d",viii,(*ci)->vert->Size());
    lgn->Push();
}
//...
				OVertex  ov = (*(*ci)->vert)[viii];
					if(ov.isExternal){

                                        unsigned int   fvi = 	(*(*ci)->face)[fi][vi];
//lgn->Append(".2");
					unsigned int ei = ov.GetIndexToExternal();
//lgn->Append(".3");

					RAssert(ei < (*ci)->externalReferences->Size());	
					RAssert(fvi>=0);
					GIndex  gi = (*((*ci)->externalReferences))[ei];
 
					if((*ci)->key == gi.ck){								// is a self external reference
 
						(*ci)->ecd->deleted_vertex.SetMarked(fvi,true);		// mark the vertex as deleted
 
						toCleanUpCells.push_back(*ci);						// the cell will contained deleted elements. remember to clea it up
 
                                                (*(*ci)->face)[fi][vi] =  gi.i;										// update the internal reference to the new value
 
					}
 
				}
			}

			/*
			At this point the vertices that are external, not referred by any face and 
			referring in the same cell should be deleted becuase NO MORE references exist to them
			*/
			for(unsigned int vi  = 0 ; vi < (*ci)->vert->Size(); ++vi){
				OVertex  ov = (*(*ci)->vert)[vi];
				if(ov.isExternal){
					unsigned int ei = ov.GetIndexToExternal();
					RAssert(ei >=0);
                                        RAssert((*ci)->ecd!=NULL);
					GIndex  gi = (*((*ci)->externalReferences))[ei];
                                        if((*ci)->key == gi.ck){					// is a self external reference
						(*ci)->ecd->deleted_vertex.SetMarked(vi,true);		// mark the vertex as deleted
                                                toCleanUpCells.push_back(*ci);				// the cell will contained deleted elements. remember to clean it up
				}
			}
		}
	}
  lgn->Append("B5.3.9");
	RemoveDuplicates(toCleanUpCells);
}

void OCME::Create(const char * name, unsigned int pagesize){
	oce.params.blockSizeBytes = pagesize;
	oce.Create(name);	
	extMemHnd = oce.extMemHnd; 
}
void OCME::Open(const char * name){
	oce.Open(name); // open
	extMemHnd = oce.extMemHnd; // copy the reference to the external memory handler
	unsigned int mem_for_AT;
	assert(MemDbg::CheckHeap(1));
	MemDbg::SetPoint(0);

	mem_for_AT = MemDbg::MemFromPoint(0);
	sprintf(lgn->Buf(),"AT loaded with %d bytes",mem_for_AT);
	lgn->Push();
	assert(MemDbg::CheckHeap(1));
	Load();			// load the multigrdi data structure
}
void OCME::Close(const bool & savebeforeclosing){
	assert( MemDbg::CheckHeap(1));
	lgn->Append("closing database",true);
 	if(savebeforeclosing){
		/* create the impostors */
		// ComputeImpostors();	
		/* Save the multigrid data structure */
		Save();
		/* record statistics */
		RecStats();
	}
	oce.Close(savebeforeclosing);
	lgn->Append("database closed",true);

} 
/* dependences */

void OCME::ComputeDependentCells( const std::vector<Cell*> & kernel_set, std::vector<CellKey> & dep_cells){	
	std::vector<Cell*>::const_iterator ci;
	std::set<CellKey>::iterator cki;
	for(ci = kernel_set.begin(); ci != kernel_set.end(); ++ci)
		for(cki = (*ci)->dependence_set.begin(); cki != (*ci)->dependence_set.end(); ++cki) 
			dep_cells.push_back(*cki);

	/* eliminate duplicates */
	RemoveDuplicates(dep_cells);

};

void OCME::ComputeDependentCells( const std::vector<Cell*> & cells, std::vector<Cell*> & dep_cells){
	std::vector<Cell*>::const_iterator ci;
	std::set<CellKey>::iterator cki;
	for(ci = cells.begin(); ci != cells.end(); ++ci)
		for(cki = (*ci)->dependence_set.begin(); cki != (*ci)->dependence_set.end(); ++cki){
			Cell * c = GetCell(*cki,false);
			if(c)							// TODO: better definition of dependence_set and when it is updated
				dep_cells.push_back(c);
		}

	/* eliminate duplicates */
	RemoveDuplicates(dep_cells);
};

void OCME::GetCellsAttributes(std::vector<Cell*> cells, AttributeMapper & am){
	std::map<std::string, ChainBase *  >::iterator ai;
	std::vector<Cell*>::iterator ci;
	for(ci = cells.begin(); ci != cells.end(); ++ci){
		for( ai = (*ci)->perVertex_attributes.begin(); ai !=(*ci)->perVertex_attributes.end(); ++ai)
			am.vert_attrs.push_back( (*ai).first);
		for( ai = (*ci)->perFace_attributes.begin(); ai !=(*ci)->perFace_attributes.end(); ++ai)
			am.face_attrs.push_back( (*ai).first);
	}

	RemoveDuplicates(am.vert_attrs);
	RemoveDuplicates(am.face_attrs);
}


/*
  NOTE: here we could do the name-type binding like in ooc_chains, but there is no need now...it will be necessary
      when OCME will support user defined attributes
  */
void OCME::UpdateCellsAttributes(Cell * c, const AttributeMapper & attr_map){
    std::vector<std::string>::const_iterator ai;
    std::map<std::string, ChainBase *  >::iterator avi,afi;

    // vertex attributes
    for(ai = attr_map.vert_attrs.begin(); ai != attr_map.vert_attrs.end(); ++ai){
               avi = c->perVertex_attributes.find(*ai);
               if(avi==c->perVertex_attributes.end()){  //the cell does not have this attribute
                   if((*ai) == std::string("Normal3f")) this->AddPerVertexAttribute<vcg::Point3f>(std::string("Normal3f"),c);else
				   if((*ai) == std::string("Color4b")) this->AddPerVertexAttribute<vcg::Color4b>(std::string("Color4b"),c);else
                       {
												sprintf(lgn->Buf(),"Attribute %s not implemented \n",(*ai).c_str());
                        lgn->Push();
                    }
               }
           }
    // face attributes
    for(ai = attr_map.face_attrs.begin(); ai != attr_map.face_attrs.end(); ++ai){
               avi = c->perFace_attributes.find(*ai);
               if(avi==c->perFace_attributes.end()){  //the cell does not have this attribute
                   if((*ai) == std::string("Normal3f")) this->AddPerFaceAttribute<vcg::Point3f>(std::string("Normal3f"),c);else
				   if((*ai) == std::string("Color4b")) this->AddPerFaceAttribute<vcg::Color4b>(std::string("Color4b"),c);else
                       {
                        sprintf(lgn->Buf(),"Attribute %s not implemented \n",*ai);
                        lgn->Push();
                    }
               }
           }
}

void OCME::RecStats(){
	CellsIterator ci;
	this->stat.n_cells = cells.size();
	 this->stat.size_lcm_allocation_table = oce.SizeOfMem();
	for(ci = this->cells.begin(); ci!= this->cells.end(); ++ci){
		stat.n_chains += 3;// TEMPORARY
		stat.n_chunks_external	+=	(*ci).second->externalReferences->chunks.size();
		stat.size_external		+=	(*ci).second->externalReferences->chunks.size()*
									(*ci).second->externalReferences->ChunkSize();
		stat.n_chunks_faces		+=	(*ci).second->face->chunks.size();
		stat.size_faces			+=	(*ci).second->face->chunks.size()*(*ci).second->face->ChunkSize();

		stat.n_chunks_vertex		+=	(*ci).second->vert->chunks.size();
		stat.size_vertex			+=	(*ci).second->vert->chunks.size()*(*ci).second->vert->ChunkSize();
		stat.size_dependences 		+= (*ci).second->dependence_set.size() * sizeof(CellKey);
	}
}


/* serialization */
int		OCME::SizeOf(){
	CellsIterator  i;
	int size =  0;
	for( i = this->cells.begin(); i != this->cells.end(); ++i){
		size+=(*i).second->SizeOf();
	}
	return size + 2 * sizeof(int);// one integer to indicate the number of cells
}

char *	OCME::Serialize (  char * const buf){
	char * ptr = buf;
	CellsIterator  i;
	*((int*)ptr) = (int) this->params.side_factor; ptr+=sizeof(int);
	*((int*)ptr) = (int) this->cells.size(); ptr+=sizeof(int);
	for( i = this->cells.begin(); i != this->cells.end(); ++i)
		ptr = (*i).second->Serialize(ptr);
	return ptr;
}

char *	OCME::DeSerialize ( char * const buf ){
	char *ptr = buf;
	this->params.side_factor = * (int*) ptr; ptr+=sizeof(int);
	int n_cells =  * (int*) ptr; ptr+=sizeof(int);

	MemDbg::SetPoint(0);
	for(int i  = 0 ; i< n_cells;++i){
		 CellKey ck;
		 memcpy(&ck,ptr,sizeof(int)*4);		 
		 Cell & c = *(this->NewCell(ck));
		 ptr = c.DeSerialize(ptr);
		  
		 /* here the attribute names have been loaded and
		 the chains in lmc have been loaded, but all the
		 pair in c.elements have NULL as cell pointer.
		 Here we reconnect them.
		 */
		 std::map<std::string, ChainBase *  >::iterator ai;
		 for( ai = c.elements.begin(); ai != c.elements.end(); ++ai){
			std::string name_of_chain = NameOfChain(c.key,(*ai).first);
			ChainBase * chain = oce.GetChain(name_of_chain);
			assert(chain!=NULL);
			(*ai).second = chain;

			/* set face and vertex if needed */
			if((*ai).first == std::string("f"))				c.face				= (Chain<OFace>*)chain; else
			if((*ai).first == std::string("v"))				c.vert				= (Chain<OVertex>*)chain; else
			if((*ai).first == std::string("e"))				c.externalReferences= (Chain<GIndex>*)chain;
			 
		 }


 		 for( ai = c.perVertex_attributes.begin(); ai != c.perVertex_attributes.end(); ++ai){
			std::string name_of_chain = NameOfChain(c.key,(*ai).first);
			ChainBase * chain = oce.GetChain(name_of_chain);
			assert(chain!=NULL);
			(*ai).second = chain;
		 }

	 }
	 return ptr;
}


std::string
OCME::NameOfChain(const CellKey & key, const std::string & chain_name){
		char pre[255];
		sprintf(pre,"%d.%d.%d.%d.",key.x,key.y,key.z,key.h);
		return  std::string(pre)+ chain_name; 
}

