

#include "../ooc_chains.h"
#include "../utils/timing.h"


extern Logging * lgn;

#include <vector>
#include <unordered_map>
#include <kcpolydb.h>




void OOCEnv::
Create( const char * name ){
	is_file_owned = true;

	std::string iDbName(name); // name of the database
	extMemHnd = new kyotocabinet::PolyDB();	
	if(!((kyotocabinet::PolyDB*)extMemHnd)->open(iDbName, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE))
		{
			printf("Failed to create databse: %s ",((kyotocabinet::PolyDB*)extMemHnd)->error().name());
			exit(0);
		}
	
}

void OOCEnv::
Create( void * handle ){
	is_file_owned = false;
	extMemHnd = (kyotocabinet::PolyDB*) handle;
}

void OOCEnv::
Open( const char * name ){
	is_file_owned = true;
	extMemHnd = new kyotocabinet::PolyDB();
	((kyotocabinet::PolyDB*)extMemHnd)->open(name,kyotocabinet::PolyDB::OWRITER|kyotocabinet::PolyDB::OREADER);
	LoadAT();
}

void OOCEnv::
Open( void * handle ){
	is_file_owned = false;
	extMemHnd = (kyotocabinet::PolyDB*) handle;
	LoadAT();
}

void OOCEnv::
Close( bool andsave){
	if(andsave){
		SaveData();
	//	((SimpleDb*)extMemHnd)->DisableSafeWriting();
		SaveAT();
	}
        lgn->Append("closing and deleting kcdb ");
        ((kyotocabinet::PolyDB*)extMemHnd)->close();
	delete (kyotocabinet::PolyDB*)extMemHnd;
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

	 
 
	((kyotocabinet::PolyDB*)extMemHnd)->set("CHAINMEM_ALLOCATION_TABLE",25,buf,siz);
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
	size_t size_buf; 
	char * buf = ((kyotocabinet::PolyDB*)extMemHnd)->get("CHAINMEM_ALLOCATION_TABLE",25, &size_buf);
	RAssert(buf);

 
	this->DeSerialize((char*)buf);
	size = this->SizeOfMem();
 
	delete [] (char*) buf;
}

