#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <vcg/math/matrix44.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/point2.h>
#include <vcg/space/point3.h>
#include <../code/lib/non_linear_minimization/non_linear_minimization.h>
#include <vcg/math/point_matching.h>


typedef vcg::GridStaticPtr<CMeshO::VertexType> GridType;

class Primitive{

protected:
	double												*parameters;					//Lista parametri delle primitiva.
	int														numParameters;				//Numero parametri primitiva.
	vcg::Matrix44f								orientation;					//Posizione ed orientamento della primitiva, la ricava il costruttore dal gesto2d e gesto3d.
	CMeshO												*TotMesh;							//La Mesh intera serve per la GetClosestVertex.
	CMeshO												*selectionMesh;				//La sotto Mesh
	std::vector<vcg::Point2i>			*gesture2D;						//Il gesto2D fatto dall'utente sulla primiva.
	std::vector<vcg::Point3f>			*gesture3D;						//La corrispondente proezione del gesto2d sulla mesh.
	std::vector<vcg::Point3f>			cloudNearPoints;			//Sono tutti i punti della Mesh più vicini a quelli del Sampling.
	std::vector<vcg::Point3f>			cloudSampling;				//Il risultato del Sampling.
	GridType											*grid;							  //La sotto mesh nella struttura di ricerca griglia statica.
	std::vector<std::vector<vcg::Point3f> >			DebugSampling;
	std::vector<std::vector<vcg::Point3f> >			DebugNear;
	std::vector<std::vector<vcg::Point3f> >			DebugMesh;
	


public:
	Primitive(std::vector<vcg::Point2i> *_gesture2D,std::vector<vcg::Point3f>	*_gesture3D,CMeshO *_TotMesh);
	virtual void Sampling(double *par)=0;
	virtual void InitSampling(double *par)=0;
	virtual void Draw()=0;
	virtual void EvaluateError(double* par,int m_dat,double* fvec,void *data,NLMin<Primitive>::lm_status_type *status);
	virtual void Minimize();
	virtual ~Primitive();

private:
	void RigidTranformation();
	void NonLinearMinimization();
	void UpdateMesh_and_Grid(const vcg::Matrix44f &m);
};


inline Primitive::Primitive(std::vector<vcg::Point2i> *_gesture2D,std::vector<vcg::Point3f>	*_gesture3D,CMeshO *_TotMesh):gesture2D(_gesture2D),gesture3D(_gesture3D),TotMesh(_TotMesh){
	parameters=NULL;
	grid=NULL;
	numParameters=0;
	orientation.SetIdentity();
}

inline void Primitive::EvaluateError(double* par,int m_dat,double* fvec,void *data,NLMin<Primitive>::lm_status_type *status){
	Sampling(par);
	//DebugSampling.push_back(cloudSampling);			//<---per debug
	for(int i=0;i<m_dat;i++){
		fvec[i]=(double)vcg::Distance(cloudSampling[i],cloudNearPoints[i]);
	}
}

inline void Primitive::Minimize(){
	InitSampling(parameters);					//Inizializza il vettore cloudSampling e cloudNearPoints
	RigidTranformation();							//
	RigidTranformation();							//
	RigidTranformation();							//
	RigidTranformation();							//
	//NonLinearMinimization();					//
}
inline void Primitive::RigidTranformation(){
	//Inizializzo il vettore di punti vicini al sampling sulla mesh.
	for(int i=0;i<(int)cloudSampling.size();i++){
		float error=0;
		CVertexO *p=NULL;
		p=vcg::trimesh::GetClosestVertex<CMeshO,GridType>(*selectionMesh,*grid,cloudSampling[i],10000/*da modificare*/,error);
		if(p!=NULL){cloudNearPoints[i]=p->P();}
		else{cloudNearPoints[i]=vcg::Point3f(0,0,0);}
	}
	vcg::Matrix44f tmp;
	vcg::PointMatching<float>::ComputeRigidMatchMatrix(tmp,cloudNearPoints/*pfix*/,cloudSampling/*pmov*/);
	UpdateMesh_and_Grid(tmp);
}
inline void Primitive::UpdateMesh_and_Grid(const vcg::Matrix44f &m){
	std::vector<vcg::Point3f>			TempMesh;										//<------Debug
	for(int i=0;i<(int)selectionMesh->vert.size();i++){
		selectionMesh->vert[i].P()=m*selectionMesh->vert[i].P();
		TempMesh.push_back(selectionMesh->vert[i].P());					//<------Debug
	}
	DebugMesh.push_back(TempMesh);														//<------Debug
	delete grid;
	grid= new GridType;
	grid->Set<CMeshO::VertexIterator>(selectionMesh->vert.begin(),selectionMesh->vert.end());
	/*Il sampling rimane fisso si muove solo la sottoMesh*/
}
inline void Primitive::NonLinearMinimization(){
	NLMin<Primitive> nlm;
	nlm.InitControl();
	nlm.control.maxcall  = 5;
	nlm.control.gtol = 0.0001;
	nlm.Fit(cloudSampling.size()/*codominio*/,numParameters/*dominio*/,parameters,this,&Primitive::EvaluateError,0);
}
inline Primitive::~Primitive(){
	delete [] parameters;
}
#endif