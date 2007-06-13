
#ifndef SLIDEPLUGIN_H
#define SLIDEPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>
#include "svgpro.h"
#include "slicedialog.h"
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <wrap/gui/trackball.h>

#include <vcg/space/index/grid_static_ptr.h>

#include <vcg/simplex/vertex/vertex.h>

#include <vcg/complex/edgemesh/base.h>

#include <vcg/simplex/edge/edge.h>

#include <wrap/io_edgemesh/export_svg.h>

typedef CMeshO n_Mesh;

class n_Face;
class n_Edge;
class n_Vertex  : public Vertex<float, n_Edge, n_Face> {};
class n_Edge    : public vcg::Edge<n_Edge, n_Vertex> {};


class n_EdgeMesh: public vcg::edge::EdgeMesh< vector<n_Vertex>, vector<n_Edge> > {};

typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType> TriMeshGrid;
typedef vcg::edge::EdgeMesh<vector<n_Vertex>,vector<n_Edge> > Edge_Mesh;
typedef vcg::edge::io::SVGProperties SVGProperties;



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
	
	
private:
	TriMeshGrid* mesh_grid;
	n_EdgeMesh* edge_mesh;
	n_Mesh trimesh;
	std::vector<typename TriMeshGrid::Cell *> intersected_cells;
	vector<Point3f> point_Vector;
	QString fileName, dirName, fileN;
	bool isDragging;
	GLArea * gla;
	MeshModel m;
    bool first;
	Box3f b;
	SVGPro *svgpro;
	float edgeMax;
SVGProperties pr;
	dialogslice *dialogsliceobj;
    void DrawPlane(GLArea * gla,MeshModel &m);
	void UpdateVal(SVGPro * sv, SVGProperties * pr);
	bool  activeDefaultTrackball;
    bool disableTransision;

public Q_SLOTS:
	void RestoreDefault();
	void SlotExportButton();
	void upGlA();
};
#endif
