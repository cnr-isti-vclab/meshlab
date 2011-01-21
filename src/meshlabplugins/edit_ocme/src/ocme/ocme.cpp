#include "ocme_disk_loader.h"

#include "../utils/memory_debug.h"

// TODO include must be taken out of the way.....
#ifdef SIMPLE_DB
#include "../ooc_vector/io/ooc_chains.hpp"
#else
#include "../ooc_vector/io/ooc_chains_kcdb.hpp"
#endif
//...............................................


#include "ocme_definition.h"

//#include "gindex_chunk.h"

#include "ocme_impostor.h"
#include "import_ocm_ply.h"
#include "../utils/logging.h"
#include "../utils/std_util.h"


unsigned int kernelSetMark;
unsigned int lockedMark;
unsigned int impostor_updated;
unsigned int to_render_fbool;
unsigned int generic_bool;

vcg::Point4f corners[8];
double cellsizes[256];

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
	for(int i = 0; i < 127;++i)
		cellsizes[127+i] = 1<<i;
	for(int i = 1; i <= 127;++i)
		cellsizes[127-i] = 1/ (double)(1<<i);
}

double CS(const int & h){ return cellsizes[127+h]; }
int COff(const int & h){ return 127+h; }






OCME::OCME(){
					lgn = new Logging("edit_ocme_log.txt");
					this->oce.AddNameTypeBound<GIndex>("GIndex");
					this->oce.AddNameTypeBound<OFace>("OFace");
					this->oce.AddNameTypeBound<OVertex>("OVertex");
					this->oce.AddNameTypeBound<BorderIndex>("BIndex");

					this->oce.AddNameTypeBound<vcg::Point3f>("Coord3f");
					this->oce.AddNameTypeBound<vcg::Color4b>("Color4b");
					this->oce.AddNameTypeBound<vcg::Point3f>("Normal3f");

                    InitCellSizes();
                    InitCorners();
//				InitRender();
                    //this->oce.CreateFromString_UserTypes = &ocme_types_allocator;

					gbi = 1;

                    params.side_factor = 50;
                    kernelSetMark = 1;
                    lockedMark = 1;
                   
                    impostor_updated = 1;
                    to_render_fbool = 1;
                    streaming_mode = false;
										record_cells_set_modification = false;
                    renderParams.one_level = false;
                    renderParams.level = 0;
					renderCache.Init(this);
					renderCache.Start();
					splat_renderer.Clear();
	}

OCME::~OCME(){
		renderCache.Finish();
		renderCache.controller.caches.clear();
	for(CellsIterator mi = this->cells.begin(); mi != this->cells.end(); ++mi)
		delete (*mi).second;
	delete lgn;	
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
		::RemoveDuplicates(removed_cells);std::sort(removed_cells.begin(),removed_cells.end());
		::RemoveDuplicates(added_cells);std::sort(added_cells.begin(),added_cells.end());
		::RemoveDuplicates(touched_cells);std::sort(touched_cells.begin(),touched_cells.end());

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

void OCME::DropEdited(  ){
		++generic_bool; // mark cells taken for editing
}
int OCME::ComputeLevel(const float & l){
	return   (int) floor( std::log(((float)this->params.side_factor) *  float(l)   ) / log(2.f)); 
}



void OCME::MoveFace(GIndex & from, const CellKey &  to){
	Cell * cellFrom = GetCell(from.ck,false);
	Cell * cellTo	= GetCell(to,true);
	RAssert(cellFrom);
	RAssert(cellTo);
	
	cellFrom->ecd->deleted_face.SetAsVectorOfMarked();
	cellFrom->ecd->deleted_face.SetMarked(from.i,true);	//delete the copy in the old cell

    from.i = cellTo->AddFace(  OFace(0,0,0));            //add a face, the real references will be set later on
	from.ck = to;
	
	cellTo->bbox.sr.Add(to.h);				// extend the scale range of the to cell to include to.h

	/*
	This is an important bit. The scale range of the two cells must be reciprocally extended
	with the scalerange of the other to guarantee the dependence property (see comment of ScaleRange in cell.h)
	*/
 	cellTo->bbox.sr.Add(cellFrom->bbox.sr);
 	cellFrom->bbox.sr.Add(cellTo->bbox.sr);

	this->CreateDependence(cellTo,cellFrom);
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

void OCME::CreateDependence(Cell * & c1_CD, Cell * & c2_CD){
	RAssert(c1_CD);
	RAssert(c2_CD);
	RAssert(c1_CD != c2_CD);
	if( c1_CD->dependence_set.find(c2_CD->key) == c1_CD->dependence_set.end()){
		c1_CD->dependence_set.insert(c2_CD->key);
		c2_CD->dependence_set.insert(c1_CD->key);
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
				newc->face				= AddElement<OFace>			("f",newc);
				newc->vert				= AddElement<OVertex>		("v",newc);
				newc->border			= AddElement<BorderIndex>	("b",newc);
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

Impostor* GetImpostor(const CellKey & , bool ifnot_create = true){
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
				newc->face				= AddElement<OFace>			("f",newc);
				newc->vert				= AddElement<OVertex>		("v",newc);
				newc->border			= AddElement<BorderIndex>	("b",newc);
				newc->impostor->InitDataCumulate(key.BBox3f());
				if(this->record_cells_set_modification) {
						this->added_cells.push_back(key);
						this->touched_cells.push_back(key);
				}
				return newc;
		}

		return NULL;
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
	return size + 3 * sizeof(int);// side factor, number of cells, gbi
}

char *	OCME::Serialize (  char * const buf){
	char * ptr = buf;
	CellsIterator  i;
	*((int*)ptr) = (int) this->params.side_factor;		ptr+=sizeof(int);
	*((int*)ptr) = (int) this->cells.size();			ptr+=sizeof(int);
	*((int*)ptr) = (int) gbi;							ptr+=sizeof(int);

	for( i = this->cells.begin(); i != this->cells.end(); ++i)
		ptr = (*i).second->Serialize(ptr);
	return ptr;
}

char *	OCME::DeSerialize ( char * const buf ){
	char *ptr = buf;
	this->params.side_factor = * (int*) ptr; ptr+=sizeof(int);
	int n_cells =  * (int*) ptr; ptr+=sizeof(int);
	gbi =  * (int*) ptr; ptr+=sizeof(int);

	MemDbg::SetPoint(0);
	for(int i  = 0 ; i< n_cells;++i){
		 CellKey ck;
		 memcpy(&ck,ptr,sizeof(int)*4);		 
		 Cell & c = *(this->NewCell(ck));
		 ptr = c.DeSerialize(ptr);
		  
		 /* here the attribute names have been loaded and
		 the chains in lmc have been loaded, but all the
		 pair in c.elements have NULL as chain pointer.
		 Here we reconnect them.
		 */
		 std::map<std::string, ChainBase *  >::iterator ai;
		 for( ai = c.elements.begin(); ai != c.elements.end(); ++ai){
			std::string name_of_chain = NameOfChain(c.key,(*ai).first);
			ChainBase * chain = oce.GetChain(name_of_chain);
			assert(chain!=NULL);
			(*ai).second = chain;

			/* set face, vertex and border if needed */
			if((*ai).first == std::string("f"))				c.face				= (Chain<OFace>*)chain; else
            if((*ai).first == std::string("v"))				c.vert				= (Chain<OVertex>*)chain;else
            if((*ai).first == std::string("b"))				c.border			= (Chain<BorderIndex>*)chain; 

		 }


 		 for( ai = c.perVertex_attributes.begin(); ai != c.perVertex_attributes.end(); ++ai){
			std::string name_of_chain = NameOfChain(c.key,(*ai).first);
			ChainBase * chain = oce.GetChain(name_of_chain);
			assert(chain!=NULL);
			(*ai).second = chain;
		 }
 		 for( ai = c.perFace_attributes.begin(); ai != c.perFace_attributes.end(); ++ai){
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

