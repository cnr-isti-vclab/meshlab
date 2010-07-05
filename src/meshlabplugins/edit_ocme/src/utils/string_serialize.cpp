#include "../utils/memory_debug.h"
#include "string_serialize.h"
#include <assert.h>
#include <string.h>


int SizeOf( const std::string &s){return (int)strlen(s.c_str())+sizeof(int);}

char * Serialize(const std::string & name, char * buffer){
	char * ptr = buffer;
	int namelength = (int)strlen(name.c_str());

	// write how many characters in the name
	memcpy(ptr,&namelength,sizeof(int));
	ptr += sizeof(int);	
	
	// write the characters of the name
	assert(MemDbg::CheckHeap(1));
	memcpy(ptr, name.c_str(),namelength);
	ptr+= namelength;

	return ptr;
}

char * DeSerialize(char * buffer, std::string & name){
	// read how many characters in the name
	char * ptr = buffer;
	int namelength;
	memcpy(&namelength, ptr, sizeof(int) );
	ptr+= sizeof(int);

	// read the characters of the name
	char *buf = new char[namelength+1];
	memcpy(buf,ptr,namelength);ptr+=namelength;
	buf[namelength]='\0';
	name = std::string(buf);
        delete []buf;

	return ptr;
}
