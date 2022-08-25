#ifndef _PLANE_BOX_INTERSECTION
#define _PLANE_BOX_INTERSECTION

#include <vcg/space/plane3.h>

/*
right hand frame;
vertices of the cube are numbered as:
0 (-1,-1,1)
1 ( 1,-1,1)
2 ( 1,1,1)
3 (-1,1,1)

4 (-1,-1,-1)
5 ( 1,-1,-1)
6 ( 1,1,-1)
7 (-1,1,-1)
*/
struct Cube{

	
	static int Edges(const int & i, const int & j){

	static	int edges[12][2]={

		{0,1}, //0
		{4,5}, //1
		{7,6}, //2
		{3,2}, //3

		{0,3}, //4
		{1,2}, //5
		{5,6}, //6
		{4,7}, //7

		{4,0}, //8
		{5,1}, //9
		{6,2}, //10
		{7,3}  //11
	};
	return edges[i][j];
	}
	static int FacesEdges(const int & i, const int & j){
		static	int facesedges[6][4]={
			{4,7,8,11},//4 X=-5
			{5,6,9,10},//5 X= 5
			{0,1,8,9}, //6 Y=-5
			{3,2,10,11}, //7 Y= 5
			{1,2,7,6}, //0 Z=-5
			{0,3,4,5}  //1 Z= 5
			};
		return facesedges[i][j];
	}
	static int EdgesFaces(const int & i, const int & j){
		static	int edgesfaces[12][2]={

			{2,5},  //0
			{4,2},  //1
			{3,4},  //2
			{5,3},  //3

			{5,0},//4
			{1,5},//5
			{4,1},//6
			{0,4},//7

			{2,0}, //8
			{1,2}, //9
			{3,1}, //10
			{0,3}  //11
			};
		return edgesfaces[i][j];
	}


	// given a face and an edge, tell the other face adjacent to the edge
	static int AdjFace(const int & i, const int & j){
		static	int adjface[6][4]={
			{5,4,2,3}, //0
			{5,4,2,3}, //1
			{5,4,0,1}, //2
			{5,4,1,0}, //3
			{2,3,0,1}, //4
			{2,3,0,1}  // 5
			};
		return adjface[i][j];
	}

	/*
	for each edge, the other 3 cubes sharing it in the form
	(dx,dy,dz),edge in the arriving cube (i+dx,j+dy,k+dz)
	*/
	static void EdgesCubes(const unsigned int & ei,const unsigned int & i, unsigned int & dx, unsigned int & dy, unsigned int &dz, unsigned int & e){

		static int edgescubes[12][3][4]=
		{
			// X edges
			{//0
				{0, 0, 1, 1},
				{0,-1, 1, 2},
				{0,-1, 0, 3}
			},
			{//1
				{0,-1, 0,2},
				{0,-1,-1,3},
				{0, 0,-1,0},
			},
			{//2
				{0, 0,-1,3},
				{0, 1,-1,0},
				{0, 1, 0,1},
			},
			{//3
				{0, 1, 0,0},
				{0, 1, 1,1},
				{0, 0, 1,2},
			},

			// Y edges
			{// 4
				{-1,0, 0,5},
				{-1,0, 1,6},
				{ 0,0, 1,7},
			},
			{// 5
				{ 0,0, 1,6},
				{ 1,0, 1,7},
				{ 1,0, 0,4},
			},
			{// 6
				{ 1,0,  0, 7},
				{ 1,0, -1, 4},
				{ 0,0, -1, 5},
			},
			{// 7
				{ 0,0, -1, 4},
				{-1,0, -1, 5},
				{-1,0,  0, 6},
			},

			// Z edges
			{//8
				{-1, 0, 0, 9},
				{-1,-1, 0,10},
				{ 0,-1, 0,11},
			},
			{//9
				{ 0,-1, 0,10},
				{ 1,-1, 0,11},
				{ 1, 0, 0, 8},
			},
			{//10
				{ 1, 0, 0,11},
				{ 1, 1, 0, 8},
				{ 0, 1, 0, 9},
			},
			{//11
				{ 0, 1, 0, 8},
				{-1, 1, 0, 9},
				{-1, 0, 0,10},
			},
		};
		dx =  edgescubes[ei][i][0];
		dy =  edgescubes[ei][i][1];
		dz =  edgescubes[ei][i][2];
		e  =  edgescubes[ei][i][3];
	}



	template <class T>
	static vcg::Point3<T> V(const int & i){
		return   vcg::Point3<T> ( -1.0+2.0*(((i+1)/2)%2)  ,-1.0+2*((i%4)/2),1.0-2.0*(i/4));
	}

	// given a direction 0,1,2 and a cube vertex, return the delta (0 or 1) between the bottom corner and the vertex
	static unsigned int DeltaV(const int & dir,const int & i){
		return	(dir==0)? ((i+1)/2)%2:(dir==1)? ((i%4)/2):((i/4)==0);
	}

};



template <class T> 
void ComputeIntersections_UCube(vcg::Plane3<T> &  p, float * inters){
	float dist[8];

	vcg::Point3<T> c = p.Projection(vcg::Point3<T>(0.0,0.0,0.0));
	for(unsigned int i =0; i < 8 ; ++i)
		dist[i] = (Cube::template V<T>(i) - c)*p.Direction();

	// store the intersections
	for(unsigned int i = 0 ; i < 12; ++i)
		if( ((dist[Cube::Edges(i,0)]>0)-(dist[Cube::Edges(i,1)]>0)) != 0)//opposed in sign
			inters[i] =  dist[Cube::Edges(i,0)] / (dist[Cube::Edges(i,0)]-dist[Cube::Edges(i,1)]);
}

template <class T> 
void ComputeIntersections_Cube(  vcg::Plane3<T>    p, vcg::Box3<T> &  cube,float * inters){
 	T scale =  (cube.max[0]-cube.min[0])/2.0;
	p.SetOffset (( p.Offset() - p.Direction()*cube.Center())/scale );
	ComputeIntersections_UCube(p,inters);
}

template <class T>
vcg::Point3<T> BoxV(const vcg::Box3<T>  & b, const unsigned int & i){
	return vcg::Point3<T> ( (((i+1)/2)%2)?b.max[0]:b.min[0],((i/2)%2)?b.max[1]:b.min[1],(i/4)?b.min[2]:b.max[2]);
}



template <class T> 
void ComputePolygonFromIntersections(  vcg::Point3<T> normal, vcg::Box3<T> &  cube,float * inters,std::vector< vcg::Point3<T> > & points){

	unsigned int first = points.size();

	unsigned int ei = 0;
	for( ; ei < 12; ++ei) if(inters[ei]!=-1) break;
	if(ei==12) return;

	unsigned int orient = (normal[ei/4]>0.0)?0:1;
	unsigned int  fi = Cube::EdgesFaces(ei,orient);
	unsigned int s_ei = ei;

	std::vector<std::pair<unsigned int,float> >   ints;
	do{
		for(int ne = 0; ne < 4; ++ne) 
			if(Cube::FacesEdges(fi,ne) != ei)
				if(inters[Cube::FacesEdges(fi,ne)]!=-1){
					ei = Cube::FacesEdges(fi,ne);

					ints.push_back( std::pair<unsigned int,float> (ei, inters[ei] ));
					fi  = Cube::AdjFace(fi,ne);	
					break;	
				}
			
	}while(ei!=s_ei);

 //	T scale =  (cube.max[0]-cube.min[0])/2.0;

	for(int i = 0; i < ints.size(); ++i)
		points.push_back(BoxV<T>(cube,Cube::Edges(ints[i].first,0))*(1-ints[i].second) + BoxV<T>(cube,Cube::Edges(ints[i].first,1))* ints[i].second );

	//for(int i = first; i < points.size(); ++i)
	//	points[i] =points[i]*scale+cube.Center();
}



template <class T>
void Intersection_Plane_UCube(vcg::Plane3<T> &  p,std::vector<std::pair<unsigned int,float> > & ints){
	
	float inters[12]= {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	float dist[8];
	float edges_int[12];
	vcg::Point3<T> c = p.Projection(vcg::Point3<T>(0.0,0.0,0.0));
	for(unsigned int i =0; i < 8 ; ++i)
		dist[i] = (Cube::template V<T>(i) - c)*p.Direction();

	//for(unsigned int i = 0 ; i < 12; ++i)
	//	if( ((dist[Cube::Edges(i,0)]>0)-(dist[Cube::Edges(i,1)]>0)) != 0)//opposed in sign
	//		ints.push_back(
	//		std::pair<unsigned int,float> (i,edges_int[i] =  dist[Cube::Edges(i,0)] / (dist[Cube::Edges(i,0)]-dist[Cube::Edges(i,1)])));

	// store the intersections
	for(unsigned int i = 0 ; i < 12; ++i)
		if( ((dist[Cube::Edges(i,0)]>0)-(dist[Cube::Edges(i,1)]>0)) != 0)//opposed in sign
			inters[i] =  dist[Cube::Edges(i,0)] / (dist[Cube::Edges(i,0)]-dist[Cube::Edges(i,1)]);

	unsigned int ei = 0;
	for( ; ei < 12; ++ei) if(inters[ei]!=-1) break;
	if(ei==12) return;

	unsigned int orient = (p.Direction()[ei/4]>0.0)?0:1;
	unsigned int  fi = Cube::EdgesFaces(ei,orient);
	unsigned int s_ei = ei;

	do{
		for(int ne = 0; ne < 4; ++ne) 
			if(Cube::FacesEdges(fi,ne) != ei)
				if(inters[Cube::FacesEdges(fi,ne)]!=-1){

					ei = Cube::FacesEdges(fi,ne);
					ints.push_back(
	 				std::pair<unsigned int,float> (ei, dist[Cube::Edges(ei,0)] / (dist[Cube::Edges(ei,0)]-dist[Cube::Edges(ei,1)])));
				
					fi  = Cube::AdjFace(fi,ne);	
					break;	
				}
			
	}while(ei!=s_ei);
}

template <class T>
void Intersection_Plane_Cube(vcg::Plane3<T>   p, vcg::Box3<T> &  cube,std::vector< vcg::Point3<T> > & points){

	unsigned int first = points.size();

	T scale =  (cube.max[0]-cube.min[0])/2.0;
	p.SetOffset (( p.Offset() - p.Direction()*cube.Center())/scale );

	std::vector<std::pair<unsigned int,float> >  ints;
	Intersection_Plane_UCube(p,ints);

	for(int i = 0; i < ints.size(); ++i)
		points.push_back( Cube::V<T>(Cube::Edges(ints[i].first,0))*(1-ints[i].second) + Cube::V<T>(Cube::Edges(ints[i].first,1))* ints[i].second );

	for(int i = first; i < points.size(); ++i)
		points[i] =points[i]*scale+cube.Center();

}

template <class T>
void Intersection_Plane_Cube(vcg::Plane3<T>   p, vcg::Box3<T> &  cube,std::vector<std::pair<unsigned int,float> > & ints){

	T scale =  (cube.max[0]-cube.min[0])/2.0;
	p.SetOffset (( p.Offset() - p.Direction()*cube.Center())/scale );
	Intersection_Plane_UCube(p,ints);
}


#endif
