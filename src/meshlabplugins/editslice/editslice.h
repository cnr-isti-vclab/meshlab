
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

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component.h>
#include <vcg/complex/edgemesh/base.h>
#include <vcg/simplex/edge/base.h>
#include <vcg/simplex/edge/component.h>
#include <wrap/io_edgemesh/export_svg.h>

typedef CMeshO n_Mesh;

class n_Face;
class n_Edge;
class n_Vertex  : public vcg::VertexSimp2<n_Vertex, n_Edge, n_Face, vcg::vertex::Coord3f, vcg::vertex::BitFlags> {};
class n_Edge    : public vcg::EdgeSimp2<n_Vertex,n_Edge, n_Vertex, vcg::edge::VertexRef> {};


class n_EdgeMesh: public vcg::edg::EdgeMesh< std::vector<n_Vertex>, std::vector<n_Edge> > {};

typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType> TriMeshGrid;
typedef vcg::edg::EdgeMesh<std::vector<n_Vertex>,std::vector<n_Edge> > Edge_Mesh;
typedef vcg::edg::io::SVGProperties SVGProperties;



class ExtraMeshSlidePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
public:
	
	ExtraMeshSlidePlugin();
    vcg::Trackball trackball_slice;
	virtual ~ExtraMeshSlidePlugin();
	static const QString Info();
	virtual void StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	
private:
	TriMeshGrid* mesh_grid;
	n_EdgeMesh* edge_mesh;
	n_Mesh trimesh;
	std::vector< TriMeshGrid::Cell *> intersected_cells;
	std::vector<vcg::Point3f> point_Vector;
	QString fileName, dirName, fileN;
	bool isDragging;
	GLArea * gla;
	MeshModel *m;
  vcg::Box3f b;
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
