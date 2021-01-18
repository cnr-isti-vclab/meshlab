#ifndef SIMPLEDB_H
#define SIMPLEDB_H

#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits>

struct SimpleDb{
	struct Index{ Index(){ value = std::numeric_limits<unsigned int>::max();}
		bool Void(){return value == std::numeric_limits<unsigned int>::max();}
		void SetVoid(){  value = std::numeric_limits<unsigned int>::max();}

		void SetIFile(const unsigned int & i){ value &= 0x00FFFFFF; value |= i << 24; }
		void SetISeg(const unsigned int & i) { value &= 0xFF000000; value |= i; }

		unsigned int  IFile() const { return value >> 24; }
		unsigned int  ISeg() const {  return (value & 0x00FFFFFF); }

//		char * Serialize(char * buf){ *(unsigned int*)buf = value; return buf+sizeof(unsigned int);}
		char * DeSerialize(char * buf){value =  *(unsigned int*)buf  ; return buf+sizeof(unsigned int);}

		const bool operator <(const Index& o)const{return value < o.value;}
		const bool operator ==(const Index& o)const{return value == o.value;}
		const bool operator >(const Index& o)const{return value > o.value;}
//private:
		unsigned int 	value;

	};

	SimpleDb(const std::string & _name);
	~SimpleDb();

	Index		PutSingle(std::string, Index &  pos, void * buf, unsigned int siz);
	Index		PutSingle(std::string  ,void * ,unsigned long);
	Index		Put(std::string  ,void * ,unsigned long);

	Index		GetSingle(std::string  ,void *, unsigned long siz);
	Index		Get(std::string key ,void *, unsigned long siz);
	Index		Get(std::string key, void *&  buf);
    Index		Get(const Index &  id, void * buf, const unsigned int &);

	void		Del(std::string);
	void		Open(const std::string & _name);
	void		Create(const std::string & _name, const unsigned int &  pagesize);
	void		EnableSafeWriting();
	void		DisableSafeWriting();

private:
	unsigned int segment_size;				// length of a segment in bytes
	unsigned int max_file_size_bytes;		// max admitted file size
	unsigned int max_file_size_segments;	// max admitted file size
	unsigned int used_segments;				// number of used segments

	bool safe_writing;						// safe writing enabled (default false)
	Index first_safe_segment;				// first segment beyond the safe database

	Index next_segment;						// the index to the next segment to save to
	Index next_creation;					// the next segment to be created
	Index index_pos;						// where the index itself is written into the file

	std::map<std::string,Index> index;		// index of the database
	typedef std::map<std::string,Index>::iterator Index_ite;		// index of the database

	std::vector<Index> free_segments;							// reusable segments
	std::vector<FILE * > files;
	std::string name;

	FILE *  SetTo(const Index & id );						// returns a pointer to the position indicated by id

	std::string FileName(const unsigned int & i);
	std::string IndexName( );
	std::string PartName(std::string key, const  unsigned int & i);
	std::string NameFromIndex(  std::string   name);

	Index AddSegment();
	void CompactDatabase(){};
	void UpdateNextSegment();

	void ReadIndex(const std::string & idname);
	void WriteIndex();


	char * block_of_zeroes;

};

#endif // SIMPLEDB_H
