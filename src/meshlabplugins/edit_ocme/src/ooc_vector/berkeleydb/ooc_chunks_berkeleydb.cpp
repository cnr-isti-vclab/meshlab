
#ifndef NO_BERKELEY

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "berkeleydb.hpp"
#include "../ooc_chains.h"
#include "../utils/timing.h"

extern Logging * lgn;


void OOCEnv:: 
Create( const char * name ){
	is_file_owned = true;	 
	remove(name);
	std::string iDbName(name); // name of the database
		extMemHnd = new BerkeleyDb(std::string(""), iDbName,DB_CREATE,false,this->params.blockSizeBytes  );
}

void OOCEnv:: 
Create( void * handle ){
	is_file_owned = false;	 
	extMemHnd = (BerkeleyDb*) handle;
}

void OOCEnv:: 
Open( const char * name ){
	is_file_owned = true;
	std::string iDbName(name); // name of the database
	extMemHnd = new BerkeleyDb(std::string(""), iDbName,0,false,this->params.blockSizeBytes);
	LoadAT();
}

void OOCEnv:: 
Open( void * handle ){
	is_file_owned = false;
 	extMemHnd = (BerkeleyDb*) handle; 
	LoadAT();
}

void OOCEnv:: 
Close( bool andsave){
	if(andsave){
		SaveData();
		SaveAT();
	}
	lgn->Append("deleting berkeleydb (with resync)");
	delete (BerkeleyDb*)extMemHnd;
	lgn->Append("done");

}


int OOCEnv::
SaveAT( ){

	int siz = this->SizeOfMem();			// size of the Allocation Table 
	char * buf = new char[siz];			// allocate memory
#ifdef _DEBUG
	char * res = 
#endif
	this->Serialize(buf);	// serialize
#ifdef _DEBUG
	RAssert(res-buf == siz);
#endif

	char title[65];
	sprintf(&title[0],"%s","CHAINMEM_ALLOCATION_TABLE");
	Dbt key(&title[0], (u_int32_t)strlen("CHAINMEM_ALLOCATION_TABLE") + 1);
    Dbt data(buf, siz);
#ifdef _DEBUG
    int result = 
#endif
	((BerkeleyDb*)extMemHnd)->getDb().put(NULL, &key, &data, 0);
	delete [] buf;
#ifdef _DEBUG
	RAssert(result==0);
#endif
	sprintf(lgn->Buf(),"Allocation Table Size: %d",siz);
	lgn->Push();
	 
	sprintf(lgn->Buf() ,"mem size:%d",siz);
	lgn->Push();

	return siz;
}

void OOCEnv::
LoadAT( ){
	Dbt data;
	char title[65];
	sprintf(&title[0],"%s","CHAINMEM_ALLOCATION_TABLE");
	Dbt key(&title[0], (u_int32_t)strlen("CHAINMEM_ALLOCATION_TABLE") + 1);
    int res = ((BerkeleyDb*)extMemHnd)->getDb().get(NULL, &key, &data, 0);
	if(res == DB_NOTFOUND)
	{
		lgn->Append("CHAINMEM_ALLOCATION_TABLE not found");
		return ;
	}
	
	int size = this->SizeOfMem();
	char * buf = (char*)data.get_data();
#ifdef _DEBUG	
	char * ptr = 
#endif
	this->DeSerialize(buf);
	size = this->SizeOfMem();
#ifdef _DEBUG
	RAssert(ptr-buf==size);
#endif
}

#else


#include "../ooc_chains.h"
#include "../utils/timing.h"
#include "../simpledb.h"

extern Logging * lgn;


void OOCEnv::
Create( const char * name ){
	is_file_owned = true;

	std::string iDbName(name); // name of the database
	extMemHnd = new SimpleDb(iDbName);
	((SimpleDb*)extMemHnd)->Create(iDbName,this->params.blockSizeBytes);
}

void OOCEnv::
Create( void * handle ){
	is_file_owned = false;
	extMemHnd = (SimpleDb*) handle;
}

void OOCEnv::
Open( const char * name ){
	is_file_owned = true;
	std::string iDbName(name); // name of the database
	extMemHnd = new SimpleDb(name);
	((SimpleDb*)extMemHnd)->Open(name);
	LoadAT();
}

void OOCEnv::
Open( void * handle ){
	is_file_owned = false;
	extMemHnd = (SimpleDb*) handle;
	LoadAT();
}

void OOCEnv::
Close( bool andsave){
	if(andsave){
		SaveData();
		((SimpleDb*)extMemHnd)->DisableSafeWriting();
		SaveAT();
	}
	lgn->Append("deleting simpledb ");
	delete (SimpleDb*)extMemHnd;
	lgn->Append("done");

}


int OOCEnv::
SaveAT( ){

	int siz = this->SizeOfMem();		// size of the Allocation Table
	char * buf = new char[siz];			// allocate memory
#ifdef _DEBUG
	char * res =
#endif
	this->Serialize(buf);	// serialize
#ifdef _DEBUG
	RAssert(res-buf == siz);
#endif

	std::string key("CHAINMEM_ALLOCATION_TABLE");
 
	((SimpleDb*)extMemHnd)->Put(key,buf,siz);
	delete [] buf;
 
	sprintf(lgn->Buf(),"Allocation Table Size: %d",siz);
	lgn->Push();

	sprintf(lgn->Buf() ,"mem size:%d",siz);
	lgn->Push();

	return siz;
}

void OOCEnv::
LoadAT( ){
	std::string key("CHAINMEM_ALLOCATION_TABLE");

	unsigned int size;
	void * buf;
	SimpleDb::Index res = ((SimpleDb*)extMemHnd)->Get(key, buf);
	RAssert(!res.Void());

 
	this->DeSerialize((char*)buf);
	size = this->SizeOfMem();
 
	delete [] (char*) buf;
}

#endif
