#include "polygonalpolychordcollapsefilter.h"
#include "mesh.h"
#include <vcg/complex/algorithms/polygon_polychord_collapse.h>

PolygonalPolychordCollapseFilter::PolygonalPolychordCollapseFilter() {
  typeList << FP_POLYCHORD_COLLAPSE;
  foreach(FilterIDType tt , types())
    actionList << new QAction(filterName(tt), this);
}

QString PolygonalPolychordCollapseFilter::pluginName() const {
  return "PolygonalPolychordCollapse";
}

QString PolygonalPolychordCollapseFilter::filterName(FilterIDType filterID) const {
  switch(filterID) {
    case FP_POLYCHORD_COLLAPSE :  return QString("Polychord Collapse");
    default : assert(0);
  }
  return QString();
}

QString PolygonalPolychordCollapseFilter::filterInfo(FilterIDType filterID) const {
  switch(filterID) {
    case FP_POLYCHORD_COLLAPSE :  return QString("");
    default : assert(0);
  }
  return QString("Unknown filter");
}

MeshFilterInterface::FilterClass PolygonalPolychordCollapseFilter::getClass(QAction *a) {
  switch(ID(a))
  {
    case FP_POLYCHORD_COLLAPSE :  return MeshFilterInterface::FilterClass(MeshFilterInterface::Remeshing | MeshFilterInterface::Polygonal);
    default : assert(0);
  }
  return MeshFilterInterface::Generic;
}

int PolygonalPolychordCollapseFilter::getRequirements(QAction *a) {
  switch(ID(a))
  {
    case FP_POLYCHORD_COLLAPSE :  return MeshModel::MM_POLYGONAL;
    default : assert(0);
  }
  return MeshModel::MM_NONE;
}

int PolygonalPolychordCollapseFilter::postCondition(QAction *a) const {
  switch(ID(a))
  {
    case FP_POLYCHORD_COLLAPSE :  return MeshModel::MM_VERTNUMBER | MeshModel::MM_VERTNORMAL | MeshModel::MM_FACENUMBER |
                                         MeshModel::MM_FACENORMAL;
    default : assert(0);
  }
  return MeshModel::MM_NONE;
}

bool PolygonalPolychordCollapseFilter::applyFilter(QAction *a, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos *cb) {
  PolyMesh tmpPolyMesh;
  int nFBefore = 0;
  MeshModel *mm = 0;
  switch (ID(a)) {
  case FP_POLYCHORD_COLLAPSE:
    cb(0, "Collapsing....");
    vcg::tri::PolygonSupport<CMeshO,PolyMesh>::ImportFromTriMesh(tmpPolyMesh, md.mm()->cm);
    vcg::tri::UpdateTopology<PolyMesh>::FaceFace(tmpPolyMesh);
    nFBefore = tmpPolyMesh.FN();
    vcg::tri::PolychordCollapse<PolyMesh>::CollapseAllPolychords(tmpPolyMesh, par.getBool("keepSings"));
    vcg::tri::Allocator<PolyMesh>::CompactEveryVector(tmpPolyMesh);
    mm = md.mm();
    if (par.getBool("newMesh"))
      mm = md.addNewMesh("", "collapsedMesh");
    mm->cm.Clear();
    vcg::tri::UpdateNormal<PolyMesh>::PerPolygonalFaceNormalized(tmpPolyMesh);
    vcg::tri::PolygonSupport<CMeshO,PolyMesh>::ImportFromPolyMesh(mm->cm, tmpPolyMesh);
    mm->UpdateBoxAndNormals();
    Log("Mesh successfully collapsed. %i faces deleted.", nFBefore-tmpPolyMesh.FN());
    cb(100, "Mesh successfully collapsed.");
    return true;
  default: assert(0);
  }
  return false;
}

void PolygonalPolychordCollapseFilter::initParameterSet(QAction *a, MeshModel &/*mm*/, RichParameterSet &par) {
  switch (ID(a)) {
  case FP_POLYCHORD_COLLAPSE:
    par.addParam(new RichBool("keepSings", true, "Keep singularities' structure",
                              "Keep the number of singularities and their valence.\n\n"
                              "If disabled, singularities may change."));
    par.addParam(new RichBool("newMesh", true, "Collapse a new mesh copy",
                              "Collapse a copy of the mesh, keeping the original one intact.\n\n"
                              "If disabled, the orginal mesh is modified by the collapse process."));
    break;
  default: assert(0);
  }
}

MESHLAB_PLUGIN_NAME_EXPORTER(PolygonalPolychordCollapseFilter)
