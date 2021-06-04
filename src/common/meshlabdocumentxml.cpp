#include <QString>
#include <QtGlobal>
#include <QFileInfo>
#include <QtXml>

#include "meshlabdocumentxml.h"
#include <wrap/qt/shot_qt.h>

bool MeshDocumentToXMLFile(const MeshDocument &md, QString filename, bool onlyVisibleLayers, bool saveViewState, bool binary, const std::map<int, MLRenderingData>& rendOpt)
{
	//md.setFileName(filename);
	QFileInfo fi(filename);
	QDir tmpDir = QDir::current();
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	QDomDocument doc = MeshDocumentToXML(md, onlyVisibleLayers, saveViewState, binary, rendOpt);
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream qstream(&file);
	doc.save(qstream, 1);
	file.close();
	QDir::setCurrent(tmpDir.absolutePath());
	return true;
}

QDomElement Matrix44mToXML(const Matrix44m &m, QDomDocument &doc)
{
	QDomElement matrixElem = doc.createElement("MLMatrix44");
	QString Row[4];
	for (int i = 0; i < 4; ++i)
		Row[i] = QString("%1 %2 %3 %4 \n").arg(m[i][0]).arg(m[i][1]).arg(m[i][2]).arg(m[i][3]);

	QDomText nd = doc.createTextNode("\n" + Row[0] + Row[1] + Row[2] + Row[3]);
	matrixElem.appendChild(nd);

	return matrixElem;
}

QDomElement Matrix44mToBinaryXML(const Matrix44m &m, QDomDocument &doc)
{
	QDomElement matrixElem = doc.createElement("MLMatrix44");
	QByteArray value = QByteArray::fromRawData((char *)m.V(), sizeof(Matrix44m::ScalarType) * 16).toBase64();
	QDomText nd = doc.createTextNode(QString(value));
	matrixElem.appendChild(nd);
	return matrixElem;
}

QDomElement MeshModelToXML(const MeshModel *mp, QDomDocument &doc, const QString& path, bool binary, bool saveViewState, const MLRenderingData& rendOpt = MLRenderingData())
{
	QDomElement meshElem = doc.createElement("MLMesh");
	meshElem.setAttribute("label", mp->label());
	meshElem.setAttribute("filename", mp->relativePathName(path));
	meshElem.setAttribute("visible", saveViewState?mp->isVisible():true);
	meshElem.setAttribute("idInFile", mp->idInFile());
	if (binary)
		meshElem.appendChild(Matrix44mToBinaryXML(mp->cm.Tr, doc));
	else
		meshElem.appendChild(Matrix44mToXML(mp->cm.Tr, doc));

	if (saveViewState)
	{
		QDomElement renderingElem = doc.createElement("RenderingOption");
		std::string text;
		rendOpt.serialize(text);
		QDomText nd = doc.createTextNode(QString(text.c_str()));
		renderingElem.appendChild(nd);
		MLRenderingData::GLOptionsType opt;
		if (rendOpt.get(opt))
		{
			renderingElem.setAttribute("boxColor", QString("%1 %2 %3 %4").arg(opt._perbbox_fixed_color[0]).arg(opt._perbbox_fixed_color[1]).arg(opt._perbbox_fixed_color[2]).arg(opt._perbbox_fixed_color[3]));
			renderingElem.setAttribute("pointColor", QString("%1 %2 %3 %4").arg(opt._perpoint_fixed_color[0]).arg(opt._perpoint_fixed_color[1]).arg(opt._perpoint_fixed_color[2]).arg(opt._perpoint_fixed_color[3]));
			renderingElem.setAttribute("wireColor", QString("%1 %2 %3 %4").arg(opt._perwire_fixed_color[0]).arg(opt._perwire_fixed_color[1]).arg(opt._perwire_fixed_color[2]).arg(opt._perwire_fixed_color[3]));
			renderingElem.setAttribute("solidColor", QString("%1 %2 %3 %4").arg(opt._persolid_fixed_color[0]).arg(opt._persolid_fixed_color[1]).arg(opt._persolid_fixed_color[2]).arg(opt._persolid_fixed_color[3]));
			renderingElem.setAttribute("pointSize", opt._perpoint_pointsize);
			renderingElem.setAttribute("wireWidth", opt._perwire_wirewidth);
		}
		meshElem.appendChild(renderingElem);
	}

	return meshElem;
}

QDomElement RasterModelToXML(const RasterModel *mp, QDomDocument &doc, bool binary)
{
	QDomElement rasterElem = doc.createElement("MLRaster");
	rasterElem.setAttribute("label", mp->label());
	if (binary)
		rasterElem.appendChild(WriteShotToQDomNodeBinary(mp->shot, doc));
	else
		rasterElem.appendChild(WriteShotToQDomNode(mp->shot, doc));
	for (int ii = 0; ii < mp->planeList.size(); ++ii)
		rasterElem.appendChild(PlaneToXML(mp->planeList[ii], mp->par->pathName(), doc));
	return rasterElem;
}

QDomElement PlaneToXML(const RasterPlane* pl, const QString& basePath, QDomDocument& doc)
{
	QDomElement planeElem = doc.createElement("Plane");
	QDir dir(basePath);
	planeElem.setAttribute("fileName", dir.relativeFilePath(pl->fullPathFileName));
	planeElem.setAttribute("semantic", pl->semantic);
	return planeElem;
}

QDomDocument MeshDocumentToXML(const MeshDocument &md, bool onlyVisibleLayers, bool saveViewState, bool binary, const std::map<int, MLRenderingData>& rendOpt)
{
	QDomDocument ddoc("MeshLabDocument");

	QDomElement root = ddoc.createElement("MeshLabProject");
	ddoc.appendChild(root);
	QDomElement mgroot = ddoc.createElement("MeshGroup");

	for(const MeshModel *mmp : md.meshIterator())
	{
		if ((!onlyVisibleLayers) || (mmp->visible))
		{
			QDomElement meshElem;
			if (rendOpt.find(mmp->id()) != rendOpt.end())
				meshElem = MeshModelToXML(mmp, ddoc, md.pathName(), binary, saveViewState, rendOpt.at(mmp->id()));
			else
				meshElem = MeshModelToXML(mmp, ddoc, md.pathName(), binary, saveViewState);
			mgroot.appendChild(meshElem);
		}
	}
	root.appendChild(mgroot);

	QDomElement rgroot = ddoc.createElement("RasterGroup");

	for(const RasterModel *rmp: md.rasterIterator())
	{
		QDomElement rasterElem = RasterModelToXML(rmp, ddoc, binary);
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

