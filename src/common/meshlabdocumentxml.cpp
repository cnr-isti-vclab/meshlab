#include <QString>
#include <QtGlobal>
#include <QFileInfo>
#include <QtXml>

#include "meshmodel.h"
#include "meshlabdocumentxml.h"
#include <wrap/qt/shot_qt.h>

bool MeshDocumentToXMLFile(MeshDocument &md, QString filename, bool onlyVisibleLayers)
{
  md.setFileName(filename);
  QFileInfo fi(filename);
	QDir tmpDir = QDir::current();
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	QDomDocument doc = MeshDocumentToXML(md, onlyVisibleLayers);
  QFile file(filename);
  file.open(QIODevice::WriteOnly);
  QTextStream qstream(&file);
  doc.save(qstream,1);
	file.close();
  QDir::setCurrent(tmpDir.absolutePath());
  return true;
}

QDomElement Matrix44fToXML(vcg::Matrix44f &m, QDomDocument &doc)
{
  QDomElement matrixElem = doc.createElement("MLMatrix44");
  QString Row[4];
  for(int i=0;i<4;++i)
    Row[i] =QString("%1 %2 %3 %4 \n").arg(m[i][0]).arg(m[i][1]).arg(m[i][2]).arg(m[i][3]);

  QDomText nd = doc.createTextNode("\n"+Row[0]+Row[1]+Row[2]+Row[3]);
  matrixElem.appendChild(nd);

  return matrixElem;
}

bool MeshDocumentFromXML(MeshDocument &md, QString filename)
{
	QFile qf(filename);
		QFileInfo qfInfo(filename);
		QDir tmpDir = QDir::current();		
		QDir::setCurrent(qfInfo.absoluteDir().absolutePath());
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
        label=mesh.attributes().namedItem("label").nodeValue();
        /*MeshModel* mm = */md.addNewMesh(filen,label);
				QDomNode tr=mesh.firstChild();
		
				if(!tr.isNull() && QString::compare(tr.nodeName(),"MLMatrix44")==0)
				{
				    vcg::Matrix44f trm;
					if (tr.childNodes().size() == 1)
					{
						QStringList values =  tr.firstChild().nodeValue().split(" ", QString::SkipEmptyParts);
						for(int y = 0; y < 4; y++)
							for(int x = 0; x < 4; x++)
								md.mm()->cm.Tr[y][x] = values[x + 4*y].toFloat();
					}

				}
				mesh=mesh.nextSibling();
				}
			}
			// READ IN POINT CORRESPONDECES INCOMPLETO!!
			else if(QString::compare(node.nodeName(),"RasterGroup")==0)
			{
				QDomNode raster; QString filen, label;
				raster = node.firstChild();
				while(!raster.isNull())
				{
				//return true;
					md.addNewRaster();
					QString labelRaster=raster.attributes().namedItem("label").nodeValue();
					md.rm()->setLabel(labelRaster);
					QDomNode sh=raster.firstChild();
					ReadShotFromQDomNode(md.rm()->shot,sh);

					QDomElement el = raster.firstChildElement("Plane");
					while(!el.isNull())
					{
						QString filen = el.attribute("fileName");
						QFileInfo fi(filen);
						QString sem = el.attribute("semantic");
						QString nm = fi.absoluteFilePath();
						md.rm()->addPlane(new Plane(fi.absoluteFilePath(),sem));
						el = node.nextSiblingElement("Plane");
					}	
					raster=raster.nextSibling();
				}
			}
			node = node.nextSibling();
		}

		QDir::setCurrent(tmpDir.absolutePath());
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

QDomElement RasterModelToXML(RasterModel *mp,QDomDocument &doc)
{
  QDomElement rasterElem = doc.createElement("MLRaster");
  rasterElem.setAttribute("label",mp->label());
  rasterElem.appendChild(WriteShotToQDomNode(mp->shot,doc));
  for(int ii = 0;ii < mp->planeList.size();++ii)
	rasterElem.appendChild(PlaneToXML(mp->planeList[ii],mp->par->pathName(),doc));
  return rasterElem;
}

QDomElement PlaneToXML(Plane* pl,const QString& basePath,QDomDocument& doc)
{
	QDomElement planeElem = doc.createElement("Plane");
	QDir dir(basePath);
	planeElem.setAttribute("fileName",dir.relativeFilePath(pl->fullPathFileName));
	planeElem.setAttribute("semantic",pl->semantic);
	return planeElem;
}

QDomDocument MeshDocumentToXML(MeshDocument &md, bool onlyVisibleLayers)
{
  QDomDocument ddoc("MeshLabDocument");

  QDomElement root = ddoc.createElement("MeshLabProject");
  ddoc.appendChild( root );
  QDomElement mgroot = ddoc.createElement("MeshGroup");

  foreach(MeshModel *mmp, md.meshList)
  {
    if((!onlyVisibleLayers) || (mmp->visible))
    {
      QDomElement meshElem = MeshModelToXML(mmp, ddoc);
      mgroot.appendChild(meshElem);
    }
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

