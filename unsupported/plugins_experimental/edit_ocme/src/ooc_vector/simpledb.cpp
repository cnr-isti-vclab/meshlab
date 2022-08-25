


#include "../utils/release_assert.h"
#include "simpledb.h"
#include "../utils/string_serialize.h"
#include <stdio.h>

extern Logging * lgn;

SimpleDb::SimpleDb(const std::string & _name):	name(_name),safe_writing(false)
	{max_file_size_bytes  = 1<<30;
	};

void SimpleDb::Open(const std::string & _name){
		this->name = NameFromIndex(_name);
		ReadIndex(_name);
		max_file_size_segments = max_file_size_bytes / segment_size;
		block_of_zeroes = new char[segment_size];
		memset(block_of_zeroes,0,segment_size);
		}

void SimpleDb::Create(const std::string & _name, const unsigned int &  pagesize){
		this->name = _name;
		segment_size = pagesize;
		max_file_size_segments = max_file_size_bytes / segment_size;
		block_of_zeroes = new char[segment_size];
		memset(block_of_zeroes,0,segment_size);

		next_creation.SetIFile(0);
		next_creation.SetISeg(0);

		UpdateNextSegment(); // first segment of data
}

void SimpleDb::EnableSafeWriting(){
		first_safe_segment = next_segment;
		safe_writing = true;
}
void SimpleDb::DisableSafeWriting(){
		safe_writing = false;
}

void SimpleDb::ReadIndex(const std::string & idname){
		FILE * f = fopen(idname.c_str(),"r+b");

		fread(&segment_size,sizeof(unsigned int),1,f);
		unsigned int n_segments;
		fread(&n_segments,sizeof(unsigned int),1,f);
		unsigned int n_files;
		fread(&n_files,sizeof(unsigned int),1,f);
		fread(&next_creation,sizeof(Index),1,f);
		fread(&next_segment,sizeof(Index),1,f);
		unsigned long pos = ftell(f);

		files.resize(n_files);
		for(unsigned int i = 0; i < files.size();++i)
				files[i] = fopen(FileName(i).c_str(),"r+b");


		fseek(f,0,SEEK_END);
		unsigned int sizindex = (unsigned int)ftell(f)-pos;
		char * buf = new char[sizindex];
		char * ptr = buf;

		fseek(f,pos,SEEK_SET);
		fread(buf,sizindex,1,f);

		std::string key;
		Index id;
		for(unsigned int i = 0; i < n_segments; ++i){
				ptr = ::DeSerialize(ptr,key);
				ptr = id.DeSerialize(ptr);
				index.insert(std::pair<std::string,Index>(key,id));
		}
		delete [] buf;
		fclose(f);

};

void SimpleDb::WriteIndex(){
		FILE * f = fopen(IndexName().c_str(),"w+b");

		fwrite(&segment_size,sizeof(unsigned int),1,f);
		unsigned int n_segments = index.size();
		fwrite(&n_segments,sizeof(unsigned int),1,f);
		unsigned int n_files = files.size();
		fwrite(&n_files,sizeof(unsigned int),1,f);

		fwrite(&next_creation,sizeof(Index),1,f);
		fwrite(&next_segment,sizeof(Index),1,f);


		// write the index
		for(Index_ite ii = index.begin(); ii!=index.end(); ++ii){
				unsigned int siz = ::SizeOf((*ii).first);
				char * tmp = new char [siz];
				::Serialize((*ii).first,tmp);
				fwrite(tmp,siz,1,f);
				fwrite(&(*ii).second,sizeof(Index),1,f);
				delete [] tmp;
		}
		fclose(f);
};


SimpleDb::~SimpleDb(){
		WriteIndex();
		delete [] block_of_zeroes;
		for(unsigned int i = 0; i < files.size();++i)
				fclose(files[i]);
}


SimpleDb::Index SimpleDb::PutSingle(    std::string  name, Index &  pos, void * buf, unsigned int siz){
				if(safe_writing &&  (pos<first_safe_segment)){
					pos = next_segment;	
					UpdateNextSegment();

					/* the following 3 lines update the value of the index in the volatile copy of the index. */
					Index_ite ii = index.find(name);
					RAssert(ii != index.end());
					(*ii).second = pos;				
				}
                FILE *	f = files[pos.IFile()];
                fseek(f,pos.ISeg()*segment_size,SEEK_SET);
                fwrite(buf,siz,1,f);	// actually write the buffer to the file
                return pos;
}

SimpleDb::Index SimpleDb::PutSingle(std::string name, void * buf, unsigned long siz){
		Index pos;

		Index_ite ii = index.find(name);
		if( (ii == index.end()) || ((safe_writing &&  ((*ii).second<first_safe_segment))))
		{
				pos = next_segment;																			// otherwise take the next empty segment
				if(safe_writing)
					(*ii).second = pos;	
				else
					index.insert(std::pair<std::string,Index>(name,pos));		// add to the index
				UpdateNextSegment();																		// update the next empty segment
		}
		else
			pos = (*ii).second;																			// if it is in the index take the value

		PutSingle(std::string(),pos,buf,siz);
		return pos;
}

SimpleDb::Index SimpleDb::Put(std::string key, void * buf, unsigned long siz){

		if(siz <= segment_size)
				return PutSingle(key,buf, siz);
		else{// break the buffer into pieces
				unsigned int n_pieces = siz / segment_size + 1;
				char * ptr = (char*) buf;
				for(unsigned int i  = 0; i < n_pieces-1; ++i){
								PutSingle(PartName(key, i),ptr, segment_size);
								ptr += segment_size;
						 }
				return PutSingle(PartName(key, n_pieces-1),ptr, siz% segment_size);
		}
		
}


void SimpleDb::UpdateNextSegment(){
		next_segment = AddSegment();  // todo: implement garbage collector
}

SimpleDb::Index  SimpleDb::Get(const Index &  pos, void * buf, const unsigned int & siz){
		FILE * f = files[pos.IFile()];
		fseek(f,pos.ISeg()*segment_size,SEEK_SET);
                fread(buf,siz,1,f);							// actually read from the file
		return pos;
}

SimpleDb::Index  SimpleDb::GetSingle(std::string name, void * buf, unsigned long siz){
		FILE * f;
		Index_ite ii = index.find(name);
		if(ii == index.end()) return Index();

		Index pos = (*ii).second;								// if it is in the index take the value
		f = files[pos.IFile()];
		fseek(f,pos.ISeg()*segment_size,SEEK_SET);
		fread(buf,siz,1,f);											// actually read from the file
		return pos;
}

SimpleDb::Index  SimpleDb::Get(std::string key, void *&  buf){
		Index res;
		std::vector<Index> all_segs;
		Index_ite ii = index.find(key);

		if(ii == index.end()){
				unsigned int i = 0;
				ii = index.find(PartName(key,i));		//try to look if it is partitioned
				if(ii == index.end())
						return Index();												// if no -> fail
				res = (*ii).second;
				while( ii != index.end()){
						all_segs.push_back((*ii).second);
						ii = index.find(PartName(key,++i));
				}
				buf = new char[all_segs.size()*segment_size]; // this must be deleted by the caller
				char * ptr = (char*)buf;
				for(i = 0; i < all_segs.size(); ++i){
						if( Get(all_segs[i],ptr,segment_size).Void() ) {
								const int _MISSING_PIECE_DATABASE_CORRUPTED_ = 0;
								assert(_MISSING_PIECE_DATABASE_CORRUPTED_);
						}
						ptr+= segment_size;
				}
				return res;
			}else{
						buf = new char[segment_size];
					 return Get((*ii).second,buf,segment_size);
					}

}

SimpleDb::Index SimpleDb::Get(std::string key, void * buf, unsigned long siz){
		if(siz <= segment_size)
				return GetSingle(key,buf, siz);
		else{
				unsigned int n_pieces = siz / segment_size + 1;
				char * ptr = (char* ) buf;
				for(unsigned int i  = 0; i < n_pieces-1; ++i){
								if(GetSingle(PartName(key, i),ptr, segment_size).Void())
										return Index();
								ptr += segment_size;
						 }
				return GetSingle(PartName(key, n_pieces-1),ptr, siz% segment_size);
		}

}


void SimpleDb::Del(std::string name){
		std::vector<Index> all_segs;
		Index_ite ii = index.find(name);

		if(ii == index.end()){
				unsigned int i = 0;
				ii = index.find(PartName(name,i));		//try to look if it is partitioned
				if(ii == index.end())
						return;
				while( ii != index.end()){
					if(  ( safe_writing && ( (*ii).second < first_safe_segment))){
						free_segments.push_back((*ii).second);
						index.erase(ii);
					}
					ii = index.find(PartName(name,++i));
				}
		}else{
					if(   ( safe_writing && ( (*ii).second < first_safe_segment))){
						free_segments.push_back((*ii).second);
						index.erase(ii);
					}
		}
}


std::string SimpleDb::FileName(const unsigned int & i){
		char postfix[6];
		sprintf(postfix,"%04d",i);
		return  this->name + std::string("_") + std::string(postfix) + std::string(".sdb");
}

std::string SimpleDb::IndexName(){
		return  this->name + std::string(".socm");
}

std::string SimpleDb::PartName(std::string key, const  unsigned int & i){
		char num[1024]; sprintf(num,"%d",i);
		return  key + std::string("_M") + std::string(num);
}

std::string SimpleDb:: NameFromIndex(  std::string   name){
		name.erase(name.length()-5,5);
		return name; }

SimpleDb::Index SimpleDb::AddSegment(){

		const Index res = next_creation;
		if(next_creation.IFile() >= files.size())// a new file must be created
		{
			 std::string filename = FileName(next_creation.IFile());
			 FILE * f = fopen(filename.c_str(),"w+b");
			 assert(f);
			 files.push_back(f);
			 fseek (files[next_creation.IFile()],0,SEEK_SET);
			 fwrite(block_of_zeroes,segment_size,1,files[next_creation.IFile()]);

			 next_creation.SetIFile(files.size()-1);
			 next_creation.SetISeg(1);
		}
		else{
				fseek (files[next_creation.IFile()],0,SEEK_END);
				fwrite(block_of_zeroes,segment_size,1,files[next_creation.IFile()]);

				if(next_creation.ISeg() == max_file_size_segments){
								next_creation.SetIFile(next_creation.IFile()+1);
								next_creation.SetISeg(0);
						}else
								next_creation.SetISeg(next_creation.ISeg()+1);
				}

		return res;
}

