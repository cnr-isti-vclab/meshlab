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

