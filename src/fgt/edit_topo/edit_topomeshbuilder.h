
#ifndef edit_retoptoolmeshbuilder_H
#define edit_retoptoolmeshbuilder_H

#include <QList>

#include <vcg/complex/trimesh/clean.h>
//#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/space/index/grid_static_ptr.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/intersection.h>

#include "edit_retoptool.h"


using namespace std;
using namespace vcg;

class RetopMeshBuilder
{
public:


	QList<Point3f> Lin;
	QList<Point3f> Lout;

	RetopMeshBuilder(MeshModel *originalMeshModel);

	Point3f getClosestPoint(vcg::Point3f toCheck, float dist1, float dist2);//(MeshModel &m2, vcg::Point3f toCheck, QList<Fce> Flist);

	void createBasicMesh(MeshModel &out, QList<Fce> Fstack, QList<Vtx> Vstack);
	void createRefinedMesh(MeshModel &out, int iterations, QList<Fce> Fstack, edit_retoptooldialog *dialog, int d1, int d2);

	void draww(QList<Vtx> Vstack);

private:
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
	MetroMeshGrid   unifGrid;
	typedef trimesh::FaceTmark<CMeshO> MarkerFace;
	MarkerFace markerFunctor;



MeshModel *m2;
CMeshO *m;
 };

#endif


