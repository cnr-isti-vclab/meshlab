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
    Row[i] =QString("%1 %2 %3 %4 ").arg(m[i][0]).arg(m[i][1]).arg(m[i][2]).arg(m[i][3]);

  matrixElem.setAttribute("Tr",Row[0]+Row[1]+Row[2]+Row[3]);

  return matrixElem;
}

bool MeshDocumentFromXML(MeshDocument &md, QString filename)
{
	QFile qf(filename);
		QFileInfo qfInfo(filename);

		if( !qf.open(QIODevice::ReadOnly ) )
			return false;

		QString project_path = qfInfo.absoluteFilePath();

		QDomDocument doc("MeshLabDocument");    //It represents the XML document
		if(!doc.setContent( &qf ))	
			return false;

		QDomElement root = doc.documentElement();

		QDomNode node;

		node = root.firstChild();

		//Devices
		while(!node.isNull()){
			if(QString::compare(node.nodeName(),"MeshGroup")==0)
			{
				QDomNode mesh; QString filen, label;
				mesh = node.firstChild();
				while(!mesh.isNull()){
				//return true;
				filen=mesh.attributes().namedItem("filename").nodeValue();
				md.addNewMesh(filen);
				label=mesh.attributes().namedItem("label").nodeValue();
				md.mm()->setLabel(label);
				QDomNode tr;
				tr=mesh.firstChild();
				if(!tr.isNull() && QString::compare(tr.nodeName(),"MLMatrix44")==0)
				{
				    vcg::Matrix44f trm;
					QStringList values =  tr.attributes().namedItem("Tr").nodeValue().split(" ", QString::SkipEmptyParts);
					for(int y = 0; y < 4; y++)
					  for(int x = 0; x < 4; x++)
						  md.mm()->cm.Tr[y][x] = values[x + 4*y].toFloat();

				}
				mesh=mesh.nextSibling();
				}
			}
			// READ IN POINT CORRESPONDECES INCOMPLETO!!
			else if(QString::compare(node.nodeName(),"RasterGroup")==0)
			{
				QDomNode raster; QString filen, label;
				raster = node.firstChild();
				while(!raster.isNull()){
				//return true;
				filen=raster.attributes().namedItem("label").nodeValue();
				md.addNewRaster(filen);
				label=raster.attributes().namedItem("label").nodeValue();
				md.rm()->setLabel(label);
				QDomNode sh=raster.firstChild();
				ReadShotFromQDomNode(md.rm()->shot,sh);
				
				raster=raster.nextSibling();
				}
			}
			node = node.nextSibling();
		}



		qf.close();
		return true;
}

QDomElement MeshModelToXML(MeshModel *mp, QDomDocument &doc)
{
  QDomElement meshElem = doc.createElement("MLMesh");
  meshElem.setAttribute("label",mp->label());
  meshElem.setAttribute("filename",mp->relativePathName());
  meshElem.appendChild(Matrix44fToXML(mp->cm.Tr,doc));
  return meshElem;
}

QDomElement RasterModelToXML(RasterModel *mp, QDomDocument &doc)
{
  QDomElement rasterElem = doc.createElement("MLRaster");
  rasterElem.setAttribute("label",mp->label());
  rasterElem.appendChild(WriteShotToQDomNode(mp->shot,doc));
  return rasterElem;
}

QDomDocument MeshDocumentToXML(MeshDocument &md)
{
  QDomDocument ddoc("MeshLabDocument");

  QDomElement root = ddoc.createElement("MeshLabProject");
  ddoc.appendChild( root );
  QDomElement mgroot = ddoc.createElement("MeshGroup");

  foreach(MeshModel *mmp, md.meshList)
  {
    QDomElement meshElem = MeshModelToXML(mmp, ddoc);
    mgroot.appendChild(meshElem);
  }
  root.appendChild(mgroot);

  QDomElement rgroot = ddoc.createElement("RasterGroup");

  foreach(RasterModel *rmp, md.rasterList)
  {
    QDomElement rasterElem = RasterModelToXML(rmp, ddoc);
    rgroot.appendChild(rasterElem);
  }

  root.appendChild(rgroot);

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

