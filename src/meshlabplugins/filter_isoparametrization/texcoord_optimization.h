// to be included in the library

#ifndef __VCGLIB__TEXTCOOORD_OPTIMIZATION
#define __VCGLIB__TEXTCOOORD_OPTIMIZATION


#include <vcg/container/simple_temporary_data.h>
#ifdef _USE_OMP
#include <omp.h>
#endif
/*

SINGLE PATCH TEXTURE OPTIMIZATIONS

A set of classes to perform optimizations of disk->disk parametrizations.

Requires texture coords to be defined per vertex (do replicate seams!).

*/


namespace vcg
{
namespace tri
{

// helper function (checks that coords are inside -1..+1)
template <class ScalarType>
bool testParamCoordsPoint(const vcg::Point2<ScalarType> &p)
{
	ScalarType eps=(ScalarType)0.00001;
	if (!((p.X()>=-1-eps)&&(p.X()<=1+eps)&&
		   (p.Y()>=-1-eps)&&(p.Y()<=1+eps)))
			return (false);
	return true;
}

/* Base class for all Texture Optimizers*/
template<class MESH_TYPE> 
class TexCoordOptimization{
protected:
  MESH_TYPE &m;
  SimpleTempData<typename MESH_TYPE::VertContainer, int > isFixed;
public:

  /* Tpyes */
  typedef MESH_TYPE MeshType;
  typedef typename MESH_TYPE::VertexIterator VertexIterator;
  typedef typename MESH_TYPE::FaceIterator FaceIterator;
  typedef typename MESH_TYPE::VertexType VertexType;
  typedef typename MESH_TYPE::FaceType FaceType;
  typedef typename MESH_TYPE::ScalarType ScalarType;
  
  
  /* Access functions */
  const MeshType & Mesh() const {return m;}
  MeshType & Mesh() {return m;}
   
  /* Constructior */
  TexCoordOptimization(MeshType &_m):m(_m),isFixed(_m.vert){
   /* assert(m.HasPerVertexTexture());*/
  }
  
  // initializes on current geometry 
  virtual void TargetCurrentGeometry()=0;
  
  // performs an interation. Returns largest movement.
  virtual ScalarType Iterate()=0;
  
  // performs an iteration (faster, but it does not tell how close it is to stopping)
  virtual void IterateBlind()=0;
  
  // performs <steps> iteration
  virtual ScalarType IterateN(int step){
    for (int i=0; i<step-1; i++) {
      this->IterateBlind();
    }
    if (step>1) return this->Iterate(); else return 0;
  }
 
  // performs iterations until convergence.
  virtual int IterateUntilConvergence(ScalarType threshold=0.0001, int maxite=5000){
    int i=0;
    while (Iterate()>threshold) {
	  if (i++>maxite) return i;
    }
    return i;
  }
  
  // desctuctor: free temporary field
  ~TexCoordOptimization(){
    /*isFixed.Stop();*/
  };
  
  // set the current border as fixed (forced to stay in position during text optimization)
  void SetBorderAsFixed(){
    /*isFixed.Start();*/
    for (VertexIterator v=m.vert.begin(); v!=m.vert.end(); v++) {
		  isFixed[v]=(v->IsB())?1:0; 
	  }  
  }
  
  // everything moves, no vertex must fixed during texture optimization)
  void SetNothingAsFixed(){
    //isFixed.Start();
    for (VertexIterator v=m.vert.begin(); v!=m.vert.end(); v++) {
		  isFixed[v]=0; 
	  }  
  }
  
  // fix a given vertex
  void FixVertex(const VertexType *v, bool fix=true){
    isFixed[v]=(fix)?1:0;
  }

  bool IsFixed(const VertexType *v)
  {
	return (isFixed[v]);
  }

  bool Fixed(const FaceType* f)
  {
	return ((isFixed[f->V(0)])&&(isFixed[f->V(1)])&&(isFixed[f->V(2)]));
  }

  virtual void SetSpeed(ScalarType){
    //assert(0); // by default, no speed to set
  }

  virtual ScalarType GetSpeed(){
    //assert(0); // by default, no speed to set
    return 0;
  }
   
  virtual void SetTheta(int){
    assert(0);
  }

  virtual int GetTheta(){
    assert(0);
    return 0;
  }
  
};

/*
AREA PRESERVING TEXTURE OPTIMIZATION

as in: Degener, P., Meseth, J., Klein, R. 
       "An adaptable surface parameterization method."
       Proc. of the 12th International Meshing oundtable, 201–213 [2003].

Features:
  
:) - Balances angle and area distortions (best results!).
:) - Can choose how to balance area and angle preservation (see SetTheta)
       theta=0 -> pure conformal (use MIPS instead!)
       theta=3 -> good balance between area and angle preservation
       theta>3 -> care more about area than about angles
:( - Slowest method.
:( - Requires a fixed boundary, else expands forever in texture space (unless theta=0).
:( - Diverges in presence of flipped faces (unless theta=0).
:( - Requires a speed parameter to be set. 
       Speed too large => when close, bounces back and forth around minimum, w/o getting any closer.
       Lower speed => longer convercence times
*/

template<class MESH_TYPE> 
class AreaPreservingTexCoordOptimization:public TexCoordOptimization<MESH_TYPE>{
public:
  /* Types */
  typedef MESH_TYPE MeshType;
  typedef typename MESH_TYPE::VertexIterator VertexIterator;
  typedef typename MESH_TYPE::FaceIterator FaceIterator;
  typedef typename MESH_TYPE::VertexType VertexType;
  typedef typename MESH_TYPE::FaceType FaceType;
  typedef typename MESH_TYPE::ScalarType ScalarType;
  typedef typename MESH_TYPE::CoordType CoordType;
  

private:
  typedef TexCoordOptimization<MESH_TYPE> Super; // superclass (commodity)
  
  // extra data per face: [0..3] -> cotangents. [4] -> area*2
  SimpleTempData<typename MESH_TYPE::FaceContainer, Point4<ScalarType> > data;
  SimpleTempData<typename MESH_TYPE::VertContainer, Point2<ScalarType> > sum;
  
  std::vector<CoordType> sumX;
  std::vector<CoordType> sumY;

  SimpleTempData<typename MESH_TYPE::VertContainer, Point2<ScalarType> > lastDir;
  /*SimpleTempData<typename MESH_TYPE::VertContainer,omp_lock_t> lck;*/
  SimpleTempData<typename MESH_TYPE::VertContainer, ScalarType > vSpeed;
  
  ScalarType totArea;
  ScalarType speed;
  
  int theta;
  
public:
  
   
  // constructor and destructor
  AreaPreservingTexCoordOptimization(MeshType &_m):Super(_m),data(_m.face),sum(_m.vert),lastDir(_m.vert),vSpeed(_m.vert,1){
    speed=(ScalarType)0.00005;
    theta=3;
	
	/*for (int i=0;i<m.vert.size();i++)
		omp_init_lock(&lck[i]);*/

  }
  
  ~AreaPreservingTexCoordOptimization(){
   /* data.Stop();
    sum.Stop();
    Super::isFixed.Stop();*/
  }
  
  void SetSpeed(ScalarType _speed){
    speed=_speed;
  }

  ScalarType GetSpeed(){
    return speed;
  }
  
  // sets the parameter theta:
  // good parameters are in 1..3
  //  0 = converge to pure conformal, ignore area preservation
  //  3 = good balance between area and conformal
  // >3 = area more important, angle preservation less important
  void SetTheta(int _theta){
    theta=_theta;
  }

  int GetTheta(){
    return theta;
  }
  
  void IterateBlind(){
    /* todo: do as iterate, but without */ 
    Iterate();
  }

  ScalarType Area(int i)
  {
         FaceType *f=&(Super::m.face[i]);
	 double val=0;
	 if (!(Super::isFixed[f->V(0)]&& Super::isFixed[f->V(1)] && Super::isFixed[f->V(2)]))
		val=(f->V(1)->T().P()-f->V(0)->T().P())^(f->V(2)->T().P()-f->V(0)->T().P());

	/* bool b0=testParamCoords(f->V(0));
	 bool b1=testParamCoords(f->V(1));
	 bool b2=testParamCoords(f->V(2));*/
			
	 if(!((fabs(val)<3.14)&&(fabs(val)>=0.0)))
	 {
		 printf("v0 %lf,%lf \n",f->V(0)->T().U(),f->V(0)->T().V());
		 printf("v1 %lf,%lf \n",f->V(1)->T().U(),f->V(1)->T().V());
		 printf("v2 %lf,%lf \n",f->V(2)->T().U(),f->V(2)->T().V());
		 printf("Area Value %lf \n",val);
		 //system("pause");
	 }

	 return fabs(val);
  }

 void InitSum()
 {
	 int k;
         int n=Super::m.vert.size();
         int n1=Super::m.face.size();
#ifdef _USE_OMP
	 #pragma omp parallel for default (none) shared(n) private(k)
#endif
	 for (k=0;k<n;k++) 
	 {
	   sum[k]=Point2<ScalarType>(0,0); 
	 }
#ifdef _USE_OMP
	 #pragma omp parallel for default (none) shared(n1) private(k)
#endif
	 for (k=0;k<n1;k++)
	 {
	   sumX[k].X()=0;
	   sumX[k].Y()=0;
	   sumX[k].Z()=0;
	   sumY[k].X()=0;
	   sumY[k].Y()=0;
	   sumY[k].Z()=0;
	 }
#ifdef _USE_OMP
	 #pragma omp barrier
#endif
 }

ScalarType getProjArea()
{
	  int k;
          int n=Super::m.face.size();
	  ScalarType tot_proj_area=0;
	 //# pragma omp parallel for 
#ifdef _USE_OMP
	  #pragma omp parallel for default (none) shared(n) private(k) reduction(+: tot_proj_area)
#endif
	  for (k=0;k<n; k++) {
	      tot_proj_area+=Area(k);
	  }
#ifdef _USE_OMP
	  #pragma omp barrier
#endif
	  return (tot_proj_area);
}

vcg::Point2<ScalarType> VertValue(const int &face,const int &vert,const double &scale)
{
         FaceType *f=&Super::m.face[face];
	 /*int i=0;*/
	 vcg::Point2<ScalarType> t0=(f->V0(vert)->T().P());
     vcg::Point2<ScalarType> t1=(f->V1(vert)->T().P());
     vcg::Point2<ScalarType> t2=(f->V2(vert)->T().P());
	 ScalarType area2 = fabs((t1-t0) ^ (t2-t0));
	 ScalarType  a = (t1-t0).Norm(),
				 b =  ((t1-t0) * (t2-t0))/a,
			     c = area2 / a,
			    
				 m0= data[face][vert] / area2,
				 m1= data[face][(vert+1)%3] / area2,
				 m2= data[face][(vert+2)%3] / area2,
				  
				 mx= (b-a)/area2,
				 my= c/area2, // 1.0/a
				 mA= data[face][3]/area2* scale,
				 e = m0*((b-a)*(b-a)+c*c) + m1*(b*b+c*c) + m2*a*a, // as obvious
				 M1= mA + 1.0/mA,
				 M2= mA - 1.0/mA,
				 px= e*my,
				 py=-e*mx,
				 qx= m1*b+ m2*a,
				 qy= m1*c,

				 
				 dx=pow(M1,theta-1)
				  	 *(px*(M1+ theta*M2) - 2.0*qx*M1), 

				 dy=pow(M1,theta-1)
					   *(py*(M1+ theta*M2) - 2.0*qy*M1), 

				 gy= dy/c,
				 gx= (dx - gy*b) / a;

				  // 3d gradient
				Point2<ScalarType> val=( (t1-t0) * gx + (t2-t0) * gy ) * data[face][3]; 

				return val;
}

void UpdateSum(const double &scale)
{
         int n=Super::m.face.size();
	 int k;
	 FaceType *f;
	 ScalarType myscale=scale;
	 vcg::Point2<ScalarType> val0,val1,val2;
#ifdef _USE_OMP
	  #pragma omp parallel for default (none) shared(n,myscale) private(k,f,val0,val1,val2) 
#endif
	  for (k=0;k<n; k++) {
                          f=&Super::m.face[k];
			  val0=VertValue(k,0,myscale);
			  val1=VertValue(k,1,myscale);
			  val2=VertValue(k,2,myscale);
			  sumX[k].V(0)=val0.X();		     
			  sumX[k].V(1)=val1.X();
			  sumX[k].V(2)=val2.X();
			  sumY[k].V(0)=val0.Y();		     
			  sumY[k].V(1)=val1.Y();
			  sumY[k].V(2)=val2.Y();
	  }
#ifdef _USE_OMP
	  #pragma omp barrier
#endif
}


void SumVertex()
{
	for (unsigned int j=0; j<Super::m.face.size(); j++) 
	{
		for (int i=0;i<3;i++)
		{
                        VertexType *v=Super::m.face[j].V(i);
			sum[v].X()+=sumX[j].V(i);
			sum[v].Y()+=sumY[j].V(i);
		}
	}
}

 ScalarType Iterate(){

	InitSum();
		
	ScalarType tot_proj_area=getProjArea();


	double scale= tot_proj_area / totArea ;

	UpdateSum(scale);


    ScalarType max=0; // max displacement
	// #pragma omp parallel for num_threads(4)
 	 // for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) 
	/*omp_lock_t lck0;
	omp_init_lock(&lck0);*/
	
	SumVertex();

	//#pragma omp parallel for
	for (unsigned int j=0; j<Super::m.vert.size(); j++) 
	{
		VertexType *v=&Super::m.vert[j];

    if (  !Super::isFixed[v] ) //if (!v->IsB()) 
    {
		  ScalarType n=sum[v].Norm();
		  //printf("N %f \n",n);
		  if ( n > 1 ) { sum[v]/=n; n=1.0;}
		  
		  if (lastDir[v]*sum[v]<0) vSpeed[v]*=(ScalarType)0.85; else vSpeed[v]/=(ScalarType)0.92;
		  lastDir[v]= sum[v];
		  
		 /* if ( n*speed<=0.1 );
		  {*/
			
			vcg::Point2f goal=v->T().P()-(sum[v] * (speed * vSpeed[v]) );
			bool isOK=testParamCoordsPoint<ScalarType>(goal);
			if (isOK)
				v->T().P()-=(sum[v] * (speed * vSpeed[v]) );


			n=n*speed * vSpeed[v];
			//#pragma omp critical 
			max=std::max(max,n);
     /* }*/
	}
	}
	return max;
 }

  
  void TargetCurrentGeometry(){
    
   /* Super::isFixed.Start();
    data.Start();
    sum.Start();*/
      sumX.resize(Super::m.face.size());
          sumY.resize(Super::m.face.size());
	  totArea=0;
	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++) {
		  double area2 = 	((f->V(1)->P() - f->V(0)->P() )^(f->V(2)->P() - f->V(0)->P() )).Norm();
		  totArea+=area2;
		
		  //if (  Super::isFixed[f->V1(0)] )
		  for (int i=0; i<3; i++){
			  data[f][i]=(
				(f->V1(i)->P() - f->V0(i)->P() )*(f->V2(i)->P() - f->V0(i)->P() )
			  )/area2;
			  data[f][3]=area2;
		  }
	  }
  }
  
};


/*
MIPS TEXTURE OPTIMIZATION

Features:
  
:( - Targets angle distortions only (not ideal for texture mapping).
:) - Quite fast.
:) - Does not require fixed boundary (will auto-find a boundary -- up to a scale).
:) - Tends to nicely heal flipped faces 
:( - Requires a speed parameter to be set 
    (SHOULD NOT BE LIKE THIS. BETTER IMPLEMENTATION NEEDED). 
       Speed too large => when close, bounces back and forth around minimum, w/o getting any closer.
       Lower speed => longer convercence times

*/

template<class MESH_TYPE> 
class MIPSTexCoordOptimization:public TexCoordOptimization<MESH_TYPE>{
public:
  /* Types */
  typedef MESH_TYPE MeshType;
  typedef typename MESH_TYPE::VertexIterator VertexIterator;
  typedef typename MESH_TYPE::FaceIterator FaceIterator;
  typedef typename MESH_TYPE::VertexType VertexType;
  typedef typename MESH_TYPE::FaceType FaceType;
  typedef typename MESH_TYPE::ScalarType ScalarType;
  

protected:
  typedef TexCoordOptimization<MESH_TYPE> Super; // superclass (commodity)
  
  // extra data per face: [0..3] -> cotangents. 
  SimpleTempData<typename MESH_TYPE::FaceContainer, Point3<ScalarType> > data;
  SimpleTempData<typename MESH_TYPE::VertContainer, Point2<ScalarType> > sum;
  
  ScalarType totArea;
  ScalarType speed;
  
public:
  
   
  // constructor and destructor
  MIPSTexCoordOptimization(MeshType &_m):Super(_m),data(_m.face),sum(_m.vert){
    speed=(ScalarType)0.001;
  }
  
  ~MIPSTexCoordOptimization(){
   /* data.Stop();
    sum.Stop();
    Super::isFixed.Stop();*/
  }
  
  void SetSpeed(ScalarType _speed){
    speed=_speed;
  }

  ScalarType GetSpeed(){
    return speed;
  }
  
  void IterateBlind(){
    /* todo: do as iterate, but without */ 
    Iterate();
  }
  
  ScalarType Iterate(){
        
    #define v0 (f->V(0)->T().P())
    #define v1 (f->V(1)->T().P())
    #define v2 (f->V(2)->T().P())
    #define vi (f->V(i)->T().P())
    #define vj (f->V(j)->T().P())
    #define vk (f->V(k)->T().P())
	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) {
		  //sum[v].Zero();
		  sum[v]=Point2<ScalarType>(0,0);
	  }

    ScalarType totProjArea=0;
	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++) {
      ScalarType area2 = ((v1-v0) ^ (v2-v0));
      totProjArea+=area2;
      ScalarType o[3] = { // (opposite edge)^2 
        (   v1-v2).SquaredNorm(),
        (v0   -v2).SquaredNorm(),
        (v0-v1   ).SquaredNorm(),
      };
      ScalarType e =( data[f][0] * o[0] + 
                      data[f][1] * o[1] + 
                      data[f][2] * o[2] ) / (area2*area2);            

		  for (int i=0; i<3; i++){               
        int j=(i+1)%3, k=(i+2)%3;                     
			  ScalarType p=(vj-vi)*(vk-vi);							  
				ScalarType gy= e*(o[k]-p) - 2*data[f][j]; 
				ScalarType gx= e*(o[j]-p) - 2*data[f][k];
				      
				// speed free mode: (a try!)
			  //sum[f->V(i)]+= ( (vj-vi) * gx + (vk-vi) * gy );// / area2; 
			  
			  // speed mode:
        sum[f->V(i)]+= ( (vj-vi) * gx + (vk-vi) * gy ) / area2; 
		  }
	  }
 
    ScalarType max=0; // max displacement

 	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) 
    if (  !Super::isFixed[v] ) 
    {
      // speed free mode: (a try!)
      //v->T().P()-=speed * sum[v] *totProjArea/totArea;
      
      // speed mode:     
      
      ScalarType n=sum[v].Norm(); if ( n > 1 ) { sum[v]/=n; n=1.0;}
		  v->T().P()-=(sum[v] ) * speed ;
		  if (max<n) max=n;
		  
  	}
  	return max;
  	#undef v0
    #undef v1 
    #undef v2 
  	#undef vi
    #undef vj 
    #undef vk 
  	//printf("rejected %d\n",rejected);
  }
  


  
  void TargetCurrentGeometry(){
    
   /* Super::isFixed.Start();
    data.Start();
    sum.Start();*/
    
	  totArea=0;
	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++) {
		  double area2 = 	((f->V(1)->P() - f->V(0)->P() )^(f->V(2)->P() - f->V(0)->P() )).Norm();
		  totArea+=area2;
		  for (int i=0; i<3; i++){
			  data[f][i]=(
				  (f->V1(i)->P() - f->V0(i)->P() )*(f->V2(i)->P() - f->V0(i)->P() )
			  );
        // / area2;
		  }
	  }
  }
  
};


#if 0  // Temporarly commented out. It still have to be thoroughly tested...

template<class MESH_TYPE> 
class WachspressTexCoordOptimization:public TexCoordOptimization<MESH_TYPE>{
public:
  /* Types */
  typedef MESH_TYPE MeshType;
  typedef typename MESH_TYPE::VertexIterator VertexIterator;
  typedef typename MESH_TYPE::FaceIterator FaceIterator;
  typedef typename MESH_TYPE::VertexType VertexType;
  typedef typename MESH_TYPE::FaceType FaceType;
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType PointType;
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType::ScalarType ScalarType;
  
private:
  class Factors{
    public:
    ScalarType data[3][2];
  };
  
  typedef TexCoordOptimization<MESH_TYPE> Super; // superclass (commodity)
  
  // extra data per face: [0..3] -> cotangents. [4] -> area*2
  SimpleTempData<typename MESH_TYPE::FaceContainer, Factors > factors;
  
public:
  
   
  // constructor and destructor
  WachspressTexCoordOptimization(MeshType &_m):Super(_m),factors(_m.face){
  }
  
  ~WachspressTexCoordOptimization(){
   /* factors.Stop();
    Super::isFixed.Stop();*/
  }

  void IterateBlind(){
    /* todo: do as iterate, but without */ 
    Iterate();
  }
    
  ScalarType Iterate(){
    
    ScalarType max; // max displacement
  	
	  
    for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) {
		  v->div=0; v->sum.SetZero();
	  }

    #define vi0 (f->V(i0)->P())
    #define vi1 (f->V(i1)->P())
    #define vi2 (f->V(i2)->P())
    #define EPSILON 1e-4

	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
		  ScalarType A=(((f->V(0)->P()) - (f->V(1)->P()))^((f->V(0)->P()) - (f->V(2)->P()))).Norm(); 
		  if (A<EPSILON) continue;
		  for (int i0=0; i0<3; i0++) {
			  int i1=(i0+1)%3,i2=(i0+2)%3;


  		  ScalarType fact = (vi1-vi0)*(vi2-vi0)/A;
				//fact=1;
			  if ( (!f->V(i1)->IsB()) /*|| f->V(o)->IsB()*/);{
				  f->V(i1)->sum += f->V(i0)->T().P() * fact;
				  f->V(i1)->div += fact;
			  }
			  if ( (!f->V(i2)->IsB()) /*|| f->V(o)->IsB()*/);{
				  f->V(i2)->sum += f->V(i0)->T().P() * fact;
				  f->V(i2)->div += fact;
			  }
		  }
	  }

	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) {
      if (  !Super::isFixed[v] )
		  if (v->div>0.001) {
			  v->T().P() = v->sum/v->div;
		  }
	  }
    return max; 	
  }
  
  
  void TargetCurrentGeometry(){
  }
  
};

#endif

template<class MESH_TYPE> 
class MeanValueTexCoordOptimization:public TexCoordOptimization<MESH_TYPE>{
public:
  /* Types */
  typedef MESH_TYPE MeshType;
  typedef typename MESH_TYPE::VertexIterator VertexIterator;
  typedef typename MESH_TYPE::FaceIterator FaceIterator;
  typedef typename MESH_TYPE::VertexType VertexType;
  typedef typename MESH_TYPE::FaceType FaceType;
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType PointType;
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType::ScalarType ScalarType;
  
private:
  class Factors{
    public:
    ScalarType data[3][2];
  };
  
  typedef TexCoordOptimization<MESH_TYPE> Super; // superclass (commodity)
  
  // extra data per face: factors
  SimpleTempData<typename MESH_TYPE::FaceContainer, Factors > factors;
  
  // extra data per vertex: sums and div
  SimpleTempData<typename MESH_TYPE::VertContainer, PointType > sum;
  SimpleTempData<typename MESH_TYPE::VertContainer, ScalarType > div;
  
public:
  
   
  // constructor and destructor
  MeanValueTexCoordOptimization(MeshType &_m):Super(_m),factors(_m.face),sum(_m.vert),div(_m.vert){
  }
  
  ~MeanValueTexCoordOptimization(){
   /* factors.Stop();
    sum.Stop();
    div.Stop();
    Super::isFixed.Stop();*/
  }
  
  void IterateBlind(){
    /* todo: do as iterate, but without */ 
    Iterate();
  }
  
  ScalarType Iterate(){
    
    ScalarType max=0; // max displacement
  		  
	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) {
		  sum[v]=PointType(0,0);
		  div[v]=0; 
	  }

	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
		  for (int i=0; i<3; i++) 
		  for (int j=1; j<3; j++) {
			  int d=i, o=(i+3-j)%3;
			  sum[f->V(d)] += f->V(o)->T().P() * factors[f].data[i][j-1];
			  div[f->V(d)] += factors[f].data[i][j-1];
		  }
	  }

	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) 
    if (  !Super::isFixed[v] )
	  if (		div[v]>0.000001 ) {
		  PointType swap=v->T().P();
		  PointType goal=sum[v]/div[v];
		
		  v->T().P() = goal*0.1+swap*0.9;

		  //v->T().P()=v->RestUV*(1-v->Damp)+(sum[v]/div[v])*(v->Damp);
		  ScalarType temp=(swap-v->T().P()).SquaredNorm();
		  if (max<temp)
			  max=temp;
	  }
    return max; 	
  }
  
  void TargetEquilateralGeometry(){
	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) {
		  div[v]=0;
	  }
	  const ScalarType fact= 1.0 / sqrt(3.0);
	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
		  for (int i=0; i<3; i++) 
			  for (int j=0; j<2; j++) {
				  factors[f].data[i][j] = fact;
				  div[f->V(i)] += fact ;
			  }
	  }
  }

  void TargetCurrentGeometry(){
    
    /*Super::isFixed.Start();
    factors.Start();
    sum.Start();
    div.Start();*/

    for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) {
		  div[v]=0; 
	  };
	  for (FaceIterator  f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
		  for (int i=0; i<3; i++) 
		  for (int j=1; j<3; j++) factors[f].data[i][j-1]=0;
	  };

    #define vs (f->V(s)->P())
    #define vd (f->V(d)->P())
    #define vo (f->V(o)->P())
    #define EPSILON 1e-4

	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
		  int s=0,d=1,o=2;
		  ScalarType A=((vs - vd)^(vs - vo)).Norm(); 
		  if (A<EPSILON) break;
		  for (int i=0; i<3; i++) 
		  for (int j=1; j<3; j++) {
			  d=i; s=(i+j)%3; o=(i+3-j)%3;
			  {

				  ScalarType dd=((vd-vs).Norm());
				  if (dd<=0.0001) continue;
				  ScalarType fact= ( ( vd -vo ).Norm() - ((vd-vo)*(vd-vs))/dd) /A;

				  //if (fact<0) printf("AAAGH!");
				  factors[f].data[d][j-1] = fact;
				  //f->V(d)->sum += f->V(o)->projected * fact;
				  div[f->V(d)] += fact ;
			  } //else {
				  //printf(".");
			  //}
		  }
	  }
	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
      for (int i=0; i<3; i++) 
		  for (int j=1; j<3; j++)   
		  	if (div[f->V(i)]>0.0001) {
		  	  //fact[i][j-1]/=div[f->V(i)];
		  	} 
		  	//else f->fact[i][j-1]=0.0;      
    }
		
	  /*
    for (f=face.begin(); f!=face.end(); f++)  {
	  	for (int i=0; i<3; i++) 
		  for (int j=1; j<3; j++) 
		  	if (f->V(i)->div>0.01) {
		  	  f->fact[i][j-1]/=f->V(i)->div;
		  	} 
		  	else f->fact[i][j-1]=0.0;
	  } */
	  
	  #undef vs 
    #undef vd 
    #undef vo

  }
  
};


/* texture coords general utility functions */
/*++++++++++++++++++++++++++++++++++++++++++*/

// returns false if any fold is present (faster than MarkFolds)
template<class MESH_TYPE>
bool IsTexCoordFoldFree(MESH_TYPE &m){
  
  assert(m.HasPerVertexTexCoord());
  
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType PointType;
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType::ScalarType ScalarType;
  
  ScalarType lastsign=0;
  for (typename MESH_TYPE::FaceIterator f=m.face.begin(); f!=m.face.end(); f++){
    ScalarType sign=((f->V(1)->T().P()-f->V(0)->T().P()) ^ (f->V(2)->T().P()-f->V(0)->T().P()));
    if (sign!=0) {
      if (sign*lastsign<0) return false;
      lastsign=sign;
    }
  }
  return true;
}

// detects and marks folded faces, by setting their quality to 0 (or 1 otherwise)
// returns number of folded faces
template<class MESH_TYPE>
int MarkTexCoordFolds(MESH_TYPE &m){
  
  assert(m.HasPerVertexTexCoord());
  assert(m.HasPerFaceQuality());
  
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType PointType;
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType::ScalarType ScalarType;
  
  SimpleTempData<typename MESH_TYPE::FaceContainer, short> sign(m.face);
  //sign.Start(0);
  
  // first pass, determine predominant sign
  int npos=0, nneg=0;
  ScalarType lastsign=0;
  for (typename MESH_TYPE::FaceIterator f=m.face.begin(); f!=m.face.end(); f++){
    ScalarType fsign=((f->V(1)->T().P()-f->V(0)->T().P()) ^ (f->V(2)->T().P()-f->V(0)->T().P()));
    if (fsign<0) { sign[f]=-1;  nneg++; }
    if (fsign>0) { sign[f]=+1; npos++; }
  }
  
  // second pass, detect folded faces
  int res=0;
  short gsign= (nneg>npos)?-1:+1;
  for (typename MESH_TYPE::FaceIterator f=m.face.begin(); f!=m.face.end(); f++){
    if (sign[f]*gsign<0){
      res++;
      f->Q()=0;
    } else f->Q()=1;
  }
  
  //sign.Stop();
  
  return res;
}

// Smooths texture coords.
// (can be useful to remove folds, 
//  e.g. these created when obtaining tecture coordinates after projections)
template<class MESH_TYPE>
void SmoothTexCoords(MESH_TYPE &m){
  
  assert(m.HasPerVertexTexCoord());
  
  typedef typename MESH_TYPE::VertexType::TexCoordType::PointType PointType;
  
  SimpleTempData<typename MESH_TYPE::VertContainer, int> div(m.vert);
  SimpleTempData<typename MESH_TYPE::VertContainer, PointType > sum(m.vert);
  
 /* div.Start();
  sum.Start();*/
  
	for (typename MESH_TYPE::VertexIterator v=m.vert.begin(); v!=m.vert.end(); v++) {
		sum[v].SetZero();
    div[v]=0;
	}

	for (typename MESH_TYPE::FaceIterator f=m.face.begin(); f!=m.face.end(); f++){
		div[f->V(0)] +=2; sum[f->V(0)] += f->V(2)->T().P(); sum[f->V(0)] += f->V(1)->T().P();
		div[f->V(1)] +=2; sum[f->V(1)] += f->V(0)->T().P(); sum[f->V(1)] += f->V(2)->T().P();
		div[f->V(2)] +=2; sum[f->V(2)] += f->V(1)->T().P(); sum[f->V(2)] += f->V(0)->T().P();
	}

	for (typename MESH_TYPE::VertexIterator v=m.vert.begin(); v!=m.vert.end(); v++) 
	if (!v->IsB()) 
  {
		//if (v->div>0) {
	    if (div[v]>0) {
			v->T().P() = sum[v]/div[v];
		}
	}
	
	/*div.Stop();
  sum.Stop();*/

}
// MIPSTexCoordFoldHealer
// ----------------------
// Uses MIPS optimization to attempt to remove folds.
// Acts only in proximity of foleded faces!
// Use "iterateUntilConvergence" to unfold faces (returns number of performed iterations, as usual)
// Use "maxStarSize" (direct access) to determine size of affected patch around folded face

// AUTO_SPEED metaparameter:
#define AUTO_SPEED 1
// if set to one, speed is reduced/increase automatically to avoid oscillating behaviour
// (consumes memory and CPU, but increase robustness with speed parameter and sometimes converge faster)

template<class MESH_TYPE> 
class MIPSTexCoordFoldHealer:public MIPSTexCoordOptimization<MESH_TYPE>{
public:
  
  int maxStarSize; // max star size that is affected around a folded face.. Defualt: 3
  
  typedef MESH_TYPE MeshType;
  typedef typename MESH_TYPE::VertexIterator VertexIterator;
  typedef typename MESH_TYPE::FaceIterator FaceIterator;
  typedef typename MESH_TYPE::VertexType VertexType;
  typedef typename MESH_TYPE::FaceType FaceType;
  typedef typename MESH_TYPE::ScalarType ScalarType;

  typedef MIPSTexCoordOptimization<MESH_TYPE> Super; // superclass (commodity)
protected:
  
  SimpleTempData<typename MESH_TYPE::FaceContainer, bool > foldf;
  SimpleTempData<typename MESH_TYPE::VertContainer, bool > foldv;
  
#if AUTO_SPEED
  SimpleTempData<typename MESH_TYPE::VertContainer, Point2<ScalarType> > lastDir;
  SimpleTempData<typename MESH_TYPE::VertContainer, ScalarType > lastSpeed;
#endif
  
  ScalarType sign;
  int nfolds;
  FaceType* aFoldedFace;

  void PropagateFoldF(){
    for (typename MeshType::FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
      if (foldv[ f->V(0)] || foldv[ f->V(1)] || foldv[ f->V(2) ] ) {
        foldf[f] = true;
      }
    }    
  }
    
  void PropagateFoldV(){
    for (typename MeshType::FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++) {
      if (foldf[f] ) {
        foldv[ f->V(0) ] = foldv[ f->V(1) ] = foldv[ f->V(2) ] = true;
      }
    }
  }
  
  bool FindFolds(){
  
    /*ScalarType lastsign=0;*/
    int npos=0, nneg=0;
    for (typename MESH_TYPE::FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
      ScalarType sign=((f->V(1)->T().P()-f->V(0)->T().P()) ^ (f->V(2)->T().P()-f->V(0)->T().P()));
      if (sign>0) { npos++; }
      if (sign<0) { nneg++; }
    }
    if (npos*nneg==0)     {sign=0; nfolds=0;} else
    if (npos>nneg) { sign=+1; nfolds=nneg; } else
    { sign=-1; nfolds=npos; };
    
    for (typename MeshType::FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++){
      ScalarType signf=((f->V(1)->T().P()-f->V(0)->T().P()) ^ (f->V(2)->T().P()-f->V(0)->T().P()));
	 /* if ((!Super::isFixed[f->V(0)])&&(!Super::isFixed[f->V(1)])&&(!Super::isFixed[f->V(2)]))*/
		foldf[f] = (signf*sign<0);
	 /* else
		foldf[f] =  false;*/
    }
    
    return true;
  }

public:
  
 int IterateUntilConvergence(ScalarType threshold=0.0001, int maxite=50){
	(void)threshold;
	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) foldv[v]=false;
    FindFolds();
    PropagateFoldV();
    PropagateFoldF();
   /* int i=0;*/
    int nite = 0, totIte=0, pass=0;
    while (Iterate()>0) { 
      totIte++; 
	  nite++;
      if (nite>=maxite) {
        PropagateFoldV();
        PropagateFoldF();
        nite=0;
        if (pass++>=maxStarSize) break; // number of passes
      } 
    }
    return totIte;
  }

   
  // constructor and destructor
  MIPSTexCoordFoldHealer(MeshType &_m):MIPSTexCoordOptimization<MeshType>(_m),foldf(_m.face),foldv(_m.vert)
#if AUTO_SPEED
                   ,lastDir(_m.vert),lastSpeed(_m.vert,1.0)
#endif
  {sign=0; nfolds=0;  maxStarSize=3; };
  
  ~MIPSTexCoordFoldHealer(){
   /* data.Stop();
    sum.Stop();
    Super::isFixed.Stop();*/
  }
  
  ScalarType Iterate(){
        
    #define v0 (f->V(0)->T().P())
    #define v1 (f->V(1)->T().P())
    #define v2 (f->V(2)->T().P())
    #define vi (f->V(i)->T().P())
    #define vj (f->V(j)->T().P())
    #define vk (f->V(k)->T().P())
	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) {
		  //sum[v].Zero();
		  Super::sum[v]=Point2<ScalarType>(0,0);
	  }

    ScalarType totProjArea=0;
    nfolds=0;
	  for (FaceIterator f=Super::m.face.begin(); f!=Super::m.face.end(); f++) {
      if (Super::isFixed[f->V(0)] && Super::isFixed[f->V(1)] && Super::isFixed[f->V(2)]) continue;
      if (!foldf[f]) continue;
      ScalarType area2 = ((v1-v0) ^ (v2-v0));
      if (area2*sign<0) nfolds++;
      totProjArea+=area2;
      ScalarType o[3] = { // (opposite edge)^2 
        (   v1-v2).SquaredNorm(),
        (v0   -v2).SquaredNorm(),
        (v0-v1   ).SquaredNorm(),
      };
      ScalarType e =( Super::data[f][0] * o[0] + 
                      Super::data[f][1] * o[1] + 
                      Super::data[f][2] * o[2] ) / (area2*area2);            

		  for (int i=0; i<3; i++){
        int j=(i+1)%3, k=(i+2)%3;                     
			  ScalarType p=(vj-vi)*(vk-vi);							  
				ScalarType gy= e*(o[k]-p) - 2*Super::data[f][j]; 
				ScalarType gx= e*(o[j]-p) - 2*Super::data[f][k];
				      
				// speed free mode: (a try!)
			  //sum[f->V(i)]+= ( (vj-vi) * gx + (vk-vi) * gy );// / area2; 
			  
			  // speed mode:
        Super::sum[f->V(i)]+= ( (vj-vi) * gx + (vk-vi) * gy ) / area2; 
		  }
	  }
 
    if (nfolds==0) return 0;
    
 	  for (VertexIterator v=Super::m.vert.begin(); v!=Super::m.vert.end(); v++) 
    if (  !Super::isFixed[v] && foldv[v] )
    {
      ScalarType n=Super::sum[v].Norm(); if ( n > 1 ) { Super::sum[v]/=n; n=1.0;}
#if AUTO_SPEED
      if (Super::sum[v]*lastDir[v] < 0.0) lastSpeed[v]*=(ScalarType)0.8; else lastSpeed[v]*=(ScalarType)1.1;
      lastDir[v]=Super::sum[v];
		  v->T().P()-=(Super::sum[v] ) * (Super::speed * lastSpeed[v] );
#else
		  v->T().P()-=(Super::sum[v] ) * Super::speed;
#endif
  	}
  	return (ScalarType)nfolds;
  	#undef v0
    #undef v1 
    #undef v2 
  	#undef vi
    #undef vj 
    #undef vk 
  	//printf("rejected %d\n",rejected);
  }
  
  
};


}	}	// End namespace vcg::tri

#endif //  __VCGLIB__TEXTCOOORD_OPTIMIZATION
