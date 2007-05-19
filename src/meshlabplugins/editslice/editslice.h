
#ifndef SLIDEPLUGIN_H
#define SLIDEPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include "slicedialog.h"
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <wrap/gui/trackball.h>

#include <vcg/space/index/grid_static_ptr.h>
// VCG Vertex
#include <vcg/simplex/vertex/vertex.h>
#include <vcg/simplex/vertex/with/afvn.h>
#include <vcg/simplex/face/base.h>
//#include <vcg/simplex/face/with/afav.h>
#include <vcg/complex/edgemesh/base.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/simplex/edge/with/ae.h>



class n_Face;
class n_Edge;
class n_Vertex  : public VertexAFVN<float, n_Edge, n_Face> {};

class n_Edge    : public vcg::Edge<n_Edge, n_Vertex> {};
class n_Mesh    : public vcg::tri::TriMesh< vector<n_Vertex>, vector<CFaceO> > {};
class n_EdgeMesh: public vcg::edge::EdgeMesh< vector<n_Vertex>, vector<n_Edge> > {};

typedef vcg::GridStaticPtr<n_Mesh::FaceType, n_Mesh::ScalarType> TriMeshGrid;
typedef vcg::edge::EdgeMesh<vector<n_Vertex>,vector<n_Edge>> Edge_Mesh;

//class MyFace    : public FaceAFAV< CVertexO, CEdge, CFaceO > {};


class ExtraMeshSlidePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	QList <QAction *> actionList;
	
public:
	
	ExtraMeshSlidePlugin();
    vcg::Trackball trackball_slice;
	virtual ~ExtraMeshSlidePlugin();
	virtual const QString Info(QAction *);
    virtual const PluginInfo &Info();
	virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent    (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseMoveEvent     (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseReleaseEvent  (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual QList<QAction *> actions() const ;
	void restoreDefault();
	
private:
	
	std::vector<Plane3f> plains;
	QString fileName;
	bool isDragging;
	GLArea * gla;
	MeshModel m;
    bool first;
	Box3f b;
	dialogslice *dialogsliceobj;
    void DrawPlane(GLArea * gla,MeshModel &m);
	bool  activeDefaultTrackball;
    bool disableTransision;
public Q_SLOTS:
	void RestoreDefault();
	void SlotExportButton();
	void upGlA();
};
#endif
