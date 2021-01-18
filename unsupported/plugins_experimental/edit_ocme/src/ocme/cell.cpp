
#include "../utils/memory_debug.h"
// TODO include must be taken out of the way.....
#ifdef SIMPLE_DB
#include "../ooc_vector/io/ooc_chains.hpp"
#else
#include "../ooc_vector/io/ooc_chains_kcdb.hpp"
#endif
//...............................................

#include "cell.h"
#include "impostor_definition.h"

Cell::~Cell(){ 
		if(impostor) 
			delete impostor;

#ifndef _TEMP_BUILDER_MODE_
		ClearEditCommitAuxData();
		ClearRenderAuxData();
#endif
}
Cell::Cell(CellKey ck):key(ck),impostor(0),rd(0),ecd(0){
		int mem;
		MemDbg::SetPoint(1);
		impostor = new Impostor();
		mem = MemDbg::MemFromPoint(1);
#ifndef _TEMP_BUILDER_MODE_
		InitEditCommitAuxData();
		InitRenderAuxData();
#endif
}

void Cell::InitEditCommitAuxData() { if(!ecd) ecd = new EditCommitAuxData();}
void Cell::ClearEditCommitAuxData(){ if(ecd) delete ecd; ecd = 0;}
void Cell::InitRenderAuxData()     { if(!rd) rd = new RenderAuxData();}
void Cell::ClearRenderAuxData()    { if(rd) delete rd; rd = 0;} 
int	 Cell::SizeInRAM()			   {
									int siz = 0;
									siz += face->Size()*sizeof(vcgFace) + vert->Size() * sizeof(vcgVertex);
									return siz;
									}

/// Vertex attributes

void Cell::GetVertexAttribute(std::string name, const unsigned int & pos, void * dst){
    std::map<std::string, ChainBase *  > ::iterator ii = perVertex_attributes.find(name); // find the chain
    RAssert(ii != perVertex_attributes.end());
    (*ii).second->GetValue(pos,dst);
}
void Cell::SetVertexAttribute(std::string name, const unsigned int & pos, void * src){
    std::map<std::string, ChainBase *  > ::iterator ii = perVertex_attributes.find(name); // find the chain
    RAssert(ii != perVertex_attributes.end());
    (*ii).second->SetValue(pos,src);
}

/// Face attributes
void Cell::GetFaceAttribute(std::string name, const unsigned int & pos, void * dst){
    std::map<std::string, ChainBase *  > ::iterator ii = perFace_attributes.find(name); // find the chain
    RAssert(ii != perFace_attributes.end());
    (*ii).second->GetValue(pos,dst);
}
void Cell::SetFaceAttribute(std::string name, const unsigned int & pos, void * src){
    std::map<std::string, ChainBase *  > ::iterator ii = perFace_attributes.find(name); // find the chain
    RAssert(ii != perFace_attributes.end());
    (*ii).second->SetValue(pos,src);
}

void Cell::GetPerVertexAttributeList(std::vector<std::string> & attr_list){
	std::map<std::string, ChainBase *  >::iterator fai;
	for(fai = perVertex_attributes.begin(); fai != perVertex_attributes.end(); ++fai) attr_list.push_back((*fai).first);
}
void Cell::GetPerFaceAttributeList(std::vector<std::string> & attr_list){
	std::map<std::string, ChainBase *  >::iterator fai;
	for(fai = perFace_attributes.begin(); fai != perFace_attributes.end(); ++fai) attr_list.push_back((*fai).first);
}


void Cell::RemoveFaces(std::vector<unsigned int> toDelete){
	std::map<std::string, ChainBase *  >::iterator ai;
	face->Compact(toDelete);
	for(ai = perVertex_attributes.begin(); ai != perVertex_attributes.end();++ai) (*ai).second->Compact(toDelete);
}

void Cell::RemoveVertices(std::vector<unsigned int> toDelete){
	std::map<std::string, ChainBase *  >::iterator ai;
	vert->Compact(toDelete);
	for(ai = perFace_attributes.begin(); ai != perFace_attributes.end();++ai) (*ai).second->Compact(toDelete);
}


int Cell::SizeOf(){
	std::map<std::string, ChainBase *  >::iterator ai;

	int size = sizeof(CellKey);
	size+= sizeof( Box4);

	size+= sizeof(unsigned int );						// size of the dependence set
	size+= sizeof(CellKey) * dependence_set.size();		//dependence set

	size+= sizeof(int); // elements.size
	for(ai = elements.begin(); ai != elements.end(); ++ai){
		size += ::SizeOf((*ai).first);
	}
	size+= sizeof(int); // perVertex_attributes.size
	for(ai = perVertex_attributes.begin(); ai != perVertex_attributes.end(); ++ai){
		size += ::SizeOf((*ai).first);
	}
	size+= sizeof(int); // perFace_attributes.size
	for(ai = perFace_attributes.begin(); ai != perFace_attributes.end(); ++ai){
		size += ::SizeOf((*ai).first);
	}

	return size;
}
char * Cell::Serialize (char * ptr){
	std::map<std::string, ChainBase *  >::iterator ai;
	std::set<CellKey >::iterator ci;

	memcpy(ptr,&(this->key),sizeof(int)*4);					ptr+=sizeof(int)	*4	;
	memcpy(ptr,& this->bbox,sizeof(Box4) );					ptr+=sizeof(Box4);

	*(unsigned int* )ptr =  dependence_set.size();			ptr+=sizeof(unsigned int);
	for(ci = dependence_set.begin(); ci != dependence_set.end(); ++ci)
	{memcpy(ptr,&(*ci),sizeof(CellKey));	ptr+=sizeof(CellKey);}

	*((int*)ptr) = (int) elements.size();				ptr+=sizeof(int);
	for(ai = elements.begin(); ai != elements.end(); ++ai ){
		ptr = ::Serialize((*ai).first,ptr);}

	*((int*)ptr) = (int) perVertex_attributes.size();	ptr+=sizeof(int);
	for(ai = perVertex_attributes.begin(); ai != perVertex_attributes.end(); ++ai ){
		ptr = ::Serialize((*ai).first,ptr);}

	*((int*)ptr) = (int) perFace_attributes.size();		ptr+=sizeof(int);
	for(ai = perFace_attributes.begin(); ai != perFace_attributes.end(); ++ai ){
		ptr = ::Serialize((*ai).first,ptr);}

	return ptr;
}
char * Cell::DeSerialize (char * buffer ){
	std::string attr_name;
	char * ptr = buffer;
	memcpy(&(this->key),ptr,sizeof(int)*4);					ptr+=sizeof(int)	*4	;
	memcpy(&(this->bbox),ptr,sizeof(Box4));					ptr+=sizeof(Box4)		;

	unsigned int ds_size = *(unsigned int* )ptr;			ptr+=sizeof(unsigned int);
	for(unsigned int i = 0; i < ds_size; ++i){
		CellKey ck;
		memcpy(&ck,ptr,sizeof(CellKey));
		dependence_set.insert(ck);
		ptr+=sizeof(CellKey);
	}

	int n_attr = *((int*)ptr);								ptr+=sizeof(int);
	for(int i = 0; i < n_attr; ++i){
		ptr = ::DeSerialize(ptr,attr_name);	// read the attribute name
		elements.insert( std::pair<std::string, ChainBase *>(attr_name,NULL) );
	}

	n_attr = *((int*)ptr);									ptr+=sizeof(int);
	for(int i = 0; i < n_attr; ++i){
		ptr = ::DeSerialize(ptr,attr_name);	// read the attribute name
		perVertex_attributes.insert( std::pair<std::string, ChainBase *>(attr_name,NULL) );
	}

	n_attr = *((int*)ptr);									ptr+=sizeof(int);
	for(int i = 0; i < n_attr; ++i){
		ptr = ::DeSerialize(ptr,attr_name);	// read the attribute name
		perFace_attributes.insert( std::pair<std::string, ChainBase *>(attr_name,NULL) );
	}
	return ptr;
}

bool Cell::IsEmpty(){ 
	for(std::map<std::string, ChainBase *  >::iterator mi =  this->elements.begin(); mi != this->elements.end();++mi)
				if( (*mi).second->Size() > 0) return false;
	return true;
}

int Cell::AddFace(OFace  f){
	// for each attribute add an element to the corresponding chain
	for(StringChainMap::iterator i = perFace_attributes.begin();  i != perFace_attributes.end(); ++i)
		(*i).second->Resize(vert->Size() + 1 );

	return face->AddElem(f);
}

unsigned int Cell::AddVertex(OVertex  v){
	bbox.bbox3.Add(v.P()); 

	// for each attribute add an element to the corresponding chain
	for(StringChainMap::iterator i = perVertex_attributes.begin();  i != perVertex_attributes.end(); ++i)
		(*i).second->Resize(vert->Size() + 1 );

	return vert->AddElem(v);

}
