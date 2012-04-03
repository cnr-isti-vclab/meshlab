#ifndef STD_UTIL
#define STD_UTIL
#include <algorithm>
#include <vcg/space/point3.h>

/* sort the container and remove duplicates*/
template <class STD_CONTAINER>
void RemoveDuplicates(STD_CONTAINER & cont){
        if(cont.empty()) return;
	std::sort(cont.begin(),cont.end());
	typename STD_CONTAINER::iterator newend = std::unique(cont.begin(),cont.end());
	cont.erase(newend,cont.end());
}

template <class STD_CONTAINER>
void SetDifference(STD_CONTAINER & A,STD_CONTAINER & B,STD_CONTAINER & A_B){
A_B.clear();
A_B.reserve(A.size());
std::sort(A.begin(),A.end());
std::sort(B.begin(),B.end());
std::set_difference(A.begin(),A.end(),B.begin(),B.end(),std::back_inserter(A_B));
}


template <class CONT>
void Delete(CONT & todelete){  ;CONT  tmp; todelete.clear();tmp.swap(todelete);}

template <class S>
vcg::Point3<char> Clamp2Char(vcg::Point3<S> v ) {
		return vcg::Point3<char> ( (char) std::min(127.f,std::max(-126.f,v[0])),
															 (char) std::min(127.f,std::max(-126.f,v[1])),
															(char)  std::min(127.f,std::max(-126.f,v[2])));

}


// CLUSTERING DI UN VETTORE DI FLOAT DATA UNA TOLLERANZA
//struct Node{
//	int min,max;
//	float v;
//};
//
//void ClusterVectorFloat( std::vector<float> & vec, float thr){
//	std::vector<Node> work[2];
//	work[0].resize(vec.size());
//	work[1].resize(vec.size());
//	for(unsigned int i  = 0; i < vec.size(); ++i){ work[0][i].min = work[0][i].max = i;   work[0][i].v= vec[i];}
//
//	int lev = 0;
//	bool onemore;
//	do{
//	onemore= false;
//	unsigned int id = 0;
//	for(unsigned int i  = 0; i+1 < work[lev].size(); ++i)
//		if(fabs( work[lev][i].v -  work[lev][i+1].v) < thr){
//			onemore=true;
//			work[ ( lev+1)%2][id].v		=  (work[lev][i].v+ work[lev][i+1].v) * 0.5;
//			work[ ( lev+1)%2][id].min	=  work[lev][i].min;
//			work[ ( lev+1)%2][id].max	=  work[lev][i+1].max;
//			++id;
//		}
//		work[ ( lev+1)%2].resize(id);
//		lev=(lev+1)%2;
//	}while(onemore);
//	
//	for(unsigned int i = 0 ; i < work[ ( lev+1)%2].size(); ++i)
//		for(unsigned int j = work[ ( lev+1)%2][i].min; j <=  work[ ( lev+1)%2][i].max; ++j)
//			vec[j] = work[ ( lev+1)%2][i].v;
//
//}

#endif
