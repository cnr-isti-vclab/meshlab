#ifndef FILTER_HQRENDER_H
#define FILTER_HQRENDER_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QStringList>
#include <RibFileStack.h>
#include <export_rib.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>


class FilterHighQualityRender : public QObject, public MeshFilterInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshFilterInterface)

public:

  enum { FP_HIGHQUALITY_RENDER } ;

  FilterHighQualityRender();

  virtual const QString filterName(FilterIDType filter) const;
  virtual const QString filterInfo(FilterIDType filter) const;
  virtual bool autoDialog(QAction *) {return true;}
  virtual void initParameterSet(QAction *,MeshModel &m, RichParameterSet & param);
  virtual bool applyFilter(QAction *filter, MeshModel &m, RichParameterSet & param, vcg::CallBackPos * cb) ;
  virtual const FilterClass getClass(QAction *a);

private:
  QDir templateDir;
  QStringList templates;
  bool delRibFiles;
  QStringList alignValue;

  int convertGeometry(RibFileStack* files, FILE* fout, QString destDir, MeshModel &m, RichParameterSet &, QStringList* textureList);
  QString getDirFromPath(QString* path);
  QString getFileNameFromPath(QString* path, bool type = true);
};

#endif