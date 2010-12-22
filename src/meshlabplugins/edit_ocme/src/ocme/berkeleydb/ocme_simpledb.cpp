#include "../ocme_disk_loader.h"
#include "../utils/memory_debug.h"

// File: excxx_example_database_read.cpp

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "../ocme_definition.h"
#include "../impostor_definition.h"
#include "../../ooc_vector/simpledb.h"



void
OCME::Save( ){
	int siz = this->SizeOf();			// size of the Multigrid Table 
	char * buf = new char[siz];			// allocate memory
#ifdef _DEBUG
	char * res = 
#endif
	this->Serialize(buf);	// serialize
#ifdef _DEBUG	
	RAssert(res-buf == siz);
#endif
	char title[65];
	sprintf(&title[0],"%s","MULTIGRID_TABLE");

	TIM::Begin(11);
	((SimpleDb*)extMemHnd)->Put(std::string(title), buf,siz) ;
	delete [] buf;

	sprintf(lgn->Buf(),"ocme table:%d",siz);
	lgn->Push();

	stat.size_ocme_table = siz;

	sprintf(lgn->Buf(),"saving the impostors data..");
	lgn->Push();
	unsigned int start =clock();
	CellsIterator ci;
	for(ci = cells.begin(); ci != cells.end();++ci)
	{
		(*ci).second->impostor->SetCentroids();
		SaveImpostor((*ci).second);
	}
	sprintf(lgn->Buf(),"..done in %f",(clock()-start)/float(CLOCKS_PER_SEC));
	lgn->Push();

	TIM::End(11);


}

void OCME::Load( ){
	void * data;
	char title[65];
	RAssert(MemDbg::CheckHeap(1));

	((SimpleDb*)extMemHnd)->Get("MULTIGRID_TABLE", data);
//	if( ((SimpleDb*)extMemHnd)->Get(std::string(title), data)==0)
//		printf("MULTIGRID_TABLE not found");

	TIM::Begin(10);

	this->DeSerialize((char*)data);
	delete [] data;

	
	// load the impostors
	for(CellsIterator ci = this->cells.begin(); ci != this->cells.end(); ++ci){
		sprintf(&title[0],"%s_impostor",ToString((*ci).second->key).c_str());
		SimpleDb::Index res = ((SimpleDb*)extMemHnd)->Get(title, data);
	
		if(!res.Void()){
			(*ci).second->impostor = new Impostor();
			(*ci).second->impostor->DeSerialize((char*)data);
			delete [] data;
		}
	}
	
	TIM::End(10);
}

void OCME::LoadImpostor(Cell * c){
		void * data;
		char title[65];
		sprintf(&title[0],"%s_impostor",ToString(c->key).c_str());
		SimpleDb::Index res = ((SimpleDb*)extMemHnd)->Get(title, data);

		if(!res.Void()){
			c->impostor = new Impostor();
			c->impostor->DeSerialize((char*)data);
			delete [] data;
		}
}
void OCME::SaveImpostor(Cell * c){
		static int n=0;
		char title[255];
		RAssert(MemDbg::CheckHeap(0));
		int siz =c->impostor->SizeOf();
		if(siz == 32 ) return; // it means that only vn and fn have been written->no impostor
		char * buf = new char[siz];
		c->impostor->Serialize(buf);
		sprintf(&title[0],"%s_impostor",ToString(c->key).c_str());
		((SimpleDb*)extMemHnd)->Put(title,buf,siz);
		delete [] buf;
		stat.size_impostors+=siz;
}

void OCME::RemoveImpostor(const CellKey & key){
		char title[255];
		RAssert(MemDbg::CheckHeap(0));
		sprintf(&title[0],"%s_impostor",ToString(key).c_str());
		((SimpleDb*)extMemHnd)->Del(title);
}


