#ifndef _STD_MATRIX_VCL
#define _STD_MATRIX_VCL

#include <vector>

template <class T>
struct stdMatrix3{
	stdMatrix(const unsigned int  & nx,const unsigned int  & ny, const unsigned int  & nz){
		data.resize(nx);
		for(unsigned int i = 0; i < nx; ++i){
			data[i].resize(ny); 
			for(unsigned int j = 0; j < ny; ++j)
				data[i][j].resize(nz);
		}
	}
	 
	std::vector< std::vector< std::vector < T> > > data;

	T & At(const unsigned int * i, const unsigned int * j, const unsigned int * k){
		return data[i][j][k];
	}
}

#endif