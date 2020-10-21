#ifndef POLYGONALPOLYCHORDCOLLAPSEFILTER_H
#define POLYGONALPOLYCHORDCOLLAPSEFILTER_H

#include <QObject>
#include <common/interfaces.h>

class PolygonalPolychordCollapseFilter : public QObject, public MeshFilterInterface
{
  Q_OBJECT
  MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
  Q_INTERFACES(MeshFilterInterface)

public:
  enum { FP_POLYCHORD_COLLAPSE };

  PolygonalPolychordCollapseFilter();

  QString pluginName() const;
  QString filterName(FilterIDType filterID) const;
  QString filterInfo(FilterIDType filterID) const;
  FilterClass getClass(QAction *a);
  int getRequirements(QAction *a);
  int postCondition(QAction *a) const;
  bool applyFilter(QAction *a, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos *cb);
  void initParameterSet(QAction *a, MeshModel &, RichParameterSet &par);
};

#endif // POLYGONALPOLYCHORDCOLLAPSEFILTER_H
