#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <QThread>
#include <vcg/math/matrix44.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/point2.h>
#include <vcg/space/point3.h>
//#include <../code/lib/non_linear_minimization/non_linear_minimization.h>
//#include <wrap/minpack/minpack.h>
#include <vcg/math/point_matching.h>
#include <vcg/math/histogram.h>
#include "../../meshlab/meshmodel.h"




typedef vcg::GridStaticPtr<CMeshO::VertexType> GridType;

int callEvaluateError(void *data,int m_dat,int n,const double* par,double* fvec,int iflag);

class Primitive : public QThread{

	Q_OBJECT

protected:
	double												*parameters;					//Lista parametri delle primitiva.
	int														numParameters;				//Numero parametri primitiva.
	vcg::Matrix44f								orientation;					//Posizione ed orientamento della primitiva, la ricava il costruttore dal gesto2d e gesto3d e viene modificata dalla RigidTranformation.
	CMeshO												*TotMesh;							//La Mesh intera serve per la GetClosestVertex.
	CMeshO												*selectionMesh;				//La sotto Mesh
	std::vector<vcg::Point2i>			*gesture2D;						//Il gesto2D fatto dall'utente sulla primiva.
	std::vector<vcg::Point3f>			*gesture3D;						//La corrispondente proezione del gesto2d sulla mesh.
	std::vector<vcg::Point3f>			cloudNearPoints;			//Sono tutti i punti della Mesh più vicini a quelli del Sampling.
	std::vector<vcg::Point3f>			cloudSampling;				//Il risultato del Sampling.
	GridType											*grid;							  //La sotto mesh nella struttura di ricerca griglia statica.
	double												totDistance;
	double												lastTotDistance;
	std::vector<double>						weight;
	std::vector<double>						myfvec;
	
	//Begin_Debug:
		std::vector<std::vector<vcg::Point3f> >			DebugCloudMesh;							
		std::vector<std::vector<vcg::Point3f> >			DebugCloudNearPoints;
		std::vector<std::vector<vcg::Point3f> >			DebugCloudSampling;
		std::vector<vcg::Matrix44f>									DebugOrientationMatrix;
	//End_Debug:

	bool showCloudNearPoints;
	bool showLines;
	bool showSelectionMesh;
	bool showCloudSampling;
	bool showInObjectSpace;
	bool showStepMode;
	int  indexOfstep;
	


public:
	Primitive(std::vector<vcg::Point2i> *_gesture2D,std::vector<vcg::Point3f>	*_gesture3D,CMeshO *_TotMesh);
	virtual void Sampling(const double *par)=0;
	virtual void InitSampling(double *par)=0;
	virtual void Draw()=0;
	//virtual void EvaluateError(double* par,int m_dat,double* fvec,void *data,NLMin<Primitive>::lm_status_type *status);
	virtual int EvaluateError(void *data,int m_dat,int n,const double* par,double* fvec,int iflag);
	virtual void run();
	void DrawDebug();
	void DrawDebugInWorldSpace();
	void DrawDebugInObjectSpace();
	void DrawDebugInStapMode();
	virtual ~Primitive();

public slots:
	void change_checkBoxCloudNearPoints(bool);
	void change_checkBoxLines(bool);
	void change_checkBoxSelectionMesh(bool);
	void change_checkBoxCloudSampling(bool);
	void change_checkBoxObjectSpace(bool);
	void change_checkBoxStepMode(bool);
	//spinBox
	void change_spinBoxStepMode(int);

signals:
	void updateGlArea();
	void updateDebugStapModeSize(int);

private:
	void RigidTranformation();
	void NonLinearMinimization();
	void UpdateMesh_and_Grid_and_Near(const vcg::Matrix44f &m);
	void UpdateWeight();
	void InitDebug();
};
#endif