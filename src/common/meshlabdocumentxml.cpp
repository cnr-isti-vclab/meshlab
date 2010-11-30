#include <QString>
#include <QtGlobal>
#include <QFileInfo>
#include <QtXml>

#include "meshmodel.h"
#include "meshlabdocumentxml.h"
#include <wrap/qt/shot_qt.h>

bool MeshDocumentToXMLFile(MeshDocument &md, QString filename)
{
  QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QTextStream qstream(&file);
    QDomDocument doc = MeshDocumentToXML(md);
    doc.save(qstream,1);
    file.close();
    return true;
}

QDomElement Matrix44fToXML(vcg::Matrix44f &m, QDomDocument &doc)
{
  QDomElement matrixElem = doc.createElement("MLMatrix44");
  QString Row[4];
  for(int i=0;i<4;++i)
    Row[i] =QString("%1 %2 %3 %4\n").arg(m[i][0]).arg(m[i][1]).arg(m[i][2]).arg(m[i][3]);

  matrixElem.setNodeValue(Row[0]+Row[1]+Row[2]+Row[3]);

  return matrixElem;
}

bool MeshDocumentFromXML(MeshDocument &, QDomDocument &doc)
{

 return true;
}

QDomElement MeshModelToXML(MeshModel *mp, QDomDocument &doc)
{
  QDomElement meshElem = doc.createElement("MLMesh");
  meshElem.setAttribute("name",mp->label());
  meshElem.setAttribute("filename",mp->relativePathName());
  meshElem.appendChild(Matrix44fToXML(mp->cm.Tr,doc));
  return meshElem;
}

QDomElement RasterModelToXML(RasterModel *mp, QDomDocument &doc)
{
  QDomElement rasterElem = doc.createElement("MLRaster");
  rasterElem.setAttribute("name",mp->label());
  rasterElem.appendChild(WriteShotToQDomNode(mp->shot,doc));
  return rasterElem;
}

QDomDocument MeshDocumentToXML(MeshDocument &md)
{
  QDomDocument ddoc("MeshLabDocument");
  QDomElement mgroot = ddoc.createElement("MeshGroup");

  foreach(MeshModel *mmp, md.meshList)
  {
    QDomElement meshElem = MeshModelToXML(mmp, ddoc);
    mgroot.appendChild(meshElem);
  }
  ddoc.appendChild(mgroot);

  QDomElement rgroot = ddoc.createElement("RasterGroup");

  foreach(RasterModel *rmp, md.rasterList)
  {
    QDomElement rasterElem = RasterModelToXML(rmp, ddoc);
    rgroot.appendChild(rasterElem);
  }

  ddoc.appendChild(rgroot);

//    tag.setAttribute(QString("name"),(*ii).first);
//    RichParameterSet &par=(*ii).second;
//    QList<RichParameter*>::iterator jj;
//    RichParameterXMLVisitor v(doc);
//    for(jj=par.paramList.begin();jj!=par.paramList.end();++jj)
//    {
//      (*jj)->accept(v);
//      tag.appendChild(v.parElem);
//    }
//    root.appendChild(tag);
//  }
//
    return ddoc;
}
