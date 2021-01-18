#ifndef _LOGGING_
#define _LOGGING_

#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>

struct Logging{

public:
	bool off;
	Logging(const char * filename):off(false){ 
		Buf() = new char[65536];Buf()[0]= '\0'; 		 
		if(off) return;
		LF() = false;
		if((filename!=NULL)){
			LF() = true;	
			F() = fopen(filename,"a+");
			fclose(F());
			FN() = std::string(filename);
		}
	}
	~Logging(){
		if( Buf())
			{delete [] Buf(); Buf()=0;}
		if(off) return;
		J().clear();
	}
	void Append(const char * msg,bool onscreen = true){
		if(off) return;
		J().push_back(std::string(msg));
		if(LF())
			Flush(std::string(msg));
		if(onscreen)
			printf("%s\n",msg);
	}
	
	char *&  Buf(){static char * buf; return buf;}
	
	std::string  & FN(){  return logfilename;}
	void Push(bool onscreen= false){
			if(off) return;
			Append(Buf());if(onscreen) printf("%s",Buf());}

	bool & LF() {  return logf;}
private:
	FILE * logfile;
	std::string logfilename; 
	std::vector<std::string> journal;
	bool logf;

	FILE *& F(){	  return logfile;}
	std::vector<std::string> & J(){  return journal;}

	void Flush(std::string line){
		if(off) return;

		F() = fopen(FN().c_str(),"a+");
		assert(F());
		fprintf(F(),"%s\n",line.c_str());
		fclose(F());
	}
};

#endif
