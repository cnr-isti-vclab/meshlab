#ifndef _STD_MATRIX_VCL
#define _STD_MATRIX_VCL

#include <vector>
#include <map>

template <class T>
struct stdMatrix2: public std::vector< std::vector<T >  >{
	stdMatrix2(){}
	~stdMatrix2(){std::vector< std::vector<T > > tmp; tmp.swap(*this);}

	void Resize(const unsigned int  & nx,const unsigned int  & ny){
		this->resize(nx);
		for(unsigned int i = 0; i < nx; ++i)
			(*this)[i].resize(ny); 
	}
};


template <class T>
struct stdMatrix3: public std::vector< stdMatrix2<T >  >{
	stdMatrix3(){}
	~stdMatrix3(){ std::vector< stdMatrix2<T > > tmp; tmp.swap(*this);}

	void Resize(const unsigned int  & nx,const unsigned int  & ny, const unsigned int  & nz){
		this->resize(nx);
		for(unsigned int i = 0; i < nx; ++i){
			(*this)[i].resize(ny); 
			for(unsigned int j = 0; j < ny; ++j)
				(*this)[i][j].resize(nz);
		}
	}
};

template <class TYPE, int SIZE>
struct stdMatrix3Sparse{



	// the type of index should be relative to SIZE,i.e. the minimum
	// type necessary to encode SIZE^3
	// SIZE < 6 unsigned char
	// SIZE < 40 unsigned short..
	typedef unsigned short  IndexType;
	typedef typename std::map<IndexType,TYPE>::iterator It;
	typedef std::pair<It,bool> InsIt;

	std::map<IndexType,TYPE> data;


	TYPE & At(unsigned char i, unsigned char j,unsigned char k){
		InsIt np;
		short int id = k*SIZE*SIZE + j * SIZE + i;
		It res = data.find(id);
		if(res == data.end()){
			np = data.insert(std::pair<IndexType,TYPE>(id,TYPE()));
			 (*(np.first)).second = TYPE();
			return (*np.first).second;
		}
		return (*res).second;
	}
	
	const TYPE & cAt(unsigned char i, unsigned char j,unsigned char k){
		InsIt np;
		IndexType id = k*SIZE*SIZE + j * SIZE + i;
		It res = data.find(id);
		if(res == data.end()) 
			 return TYPE();
		return (*res).second;
	}
	
	 bool IsOn(unsigned char i, unsigned char j,unsigned char k){
		IndexType id = k*SIZE*SIZE + j * SIZE + i;

		return (data.find(id) != data.end());
	}

	static void Index(unsigned short id, unsigned char &i,unsigned char &j,unsigned char &k){
		k = id/ (SIZE*SIZE);
		j = (id - k *SIZE * SIZE)/SIZE;
		i = id % SIZE;
	}

	void Resize(const unsigned int  & nx,const unsigned int  & ny, const unsigned int  & nz){};

	/* temp:: transition compilation */
	void clear(){data.clear();}
	void swap(stdMatrix3Sparse<TYPE,8>){}
};
#endif

