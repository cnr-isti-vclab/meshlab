#include <QString>
#include <QtGlobal>
#include <QFileInfo>
#include <QtXml>

#include "meshlabdocumentxml.h"
#include <wrap/qt/shot_qt.h>

bool MeshDocumentToXMLFile(MeshDocument &md, QString filename, bool onlyVisibleLayers, bool saveViewState, bool binary, const std::map<int, MLRenderingData>& rendOpt)
{
	md.setFileName(filename);
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

QDomElement Matrix44mToXML(Matrix44m &m, QDomDocument &doc)
{
	QDomElement matrixElem = doc.createElement("MLMatrix44");
	QString Row[4];
	for (int i = 0; i < 4; ++i)
		Row[i] = QString("%1 %2 %3 %4 \n").arg(m[i][0]).arg(m[i][1]).arg(m[i][2]).arg(m[i][3]);

	QDomText nd = doc.createTextNode("\n" + Row[0] + Row[1] + Row[2] + Row[3]);
	matrixElem.appendChild(nd);

	return matrixElem;
}

QDomElement Matrix44mToBinaryXML(Matrix44m &m, QDomDocument &doc)
{
	QDomElement matrixElem = doc.createElement("MLMatrix44");
	QByteArray value = QByteArray::fromRawData((char *)m.V(), sizeof(Matrix44m::ScalarType) * 16).toBase64();
	QDomText nd = doc.createTextNode(QString(value));
	matrixElem.appendChild(nd);
	return matrixElem;
}

bool MeshDocumentFromXML(MeshDocument &md, QString filename, bool binary, std::map<int, MLRenderingData>& rendOpt)
{
	QFile qf(filename);
	QFileInfo qfInfo(filename);
	QDir tmpDir = QDir::current();
	QDir::setCurrent(qfInfo.absoluteDir().absolutePath());
	if (!qf.open(QIODevice::ReadOnly))
		return false;

	QString project_path = qfInfo.absoluteFilePath();

	QDomDocument doc("MeshLabDocument");    //It represents the XML document
	if (!doc.setContent(&qf))
		return false;

	QDomElement root = doc.documentElement();

	QDomNode node;

	node = root.firstChild();

	//Devices
	while (!node.isNull()) {
		if (QString::compare(node.nodeName(), "MeshGroup") == 0)
		{
			QDomNode mesh; QString filen, label;
			mesh = node.firstChild();
			while (!mesh.isNull()) {
				//return true;
				filen = mesh.attributes().namedItem("filename").nodeValue();
				label = mesh.attributes().namedItem("label").nodeValue();
				bool visible = true;
				if (mesh.attributes().contains("visible"))
					visible = (mesh.attributes().namedItem("visible").nodeValue().toInt() == 1);
				MeshModel* mm = md.addNewMesh(filen, label);
				mm->visible = visible;

				if (mesh.attributes().contains("idInFile"))
					mm->setIdInFile(mesh.attributes().namedItem("idInFile").nodeValue().toInt());

				QDomNode tr = mesh.firstChildElement("MLMatrix44");

				if (!tr.isNull())
				{
					vcg::Matrix44f trm;
					if (tr.childNodes().size() == 1)
					{
						if (!binary)
						{
							QStringList rows = tr.firstChild().nodeValue().split("\n", QString::SkipEmptyParts);
							int i = 0;
							for (const QString& row : qAsConst(rows)){
								if (rows.size() > 0) {
									QStringList values = row.split(" ", QString::SkipEmptyParts);
									int j = 0;
									for (const QString& value : qAsConst(values)) {
										if (i < 4 && j < 4) {
											md.mm()->cm.Tr[i][j] = value.toFloat();
											j++;
										}
									}
									i++;
								}
							}
						}
						else
						{
							QString str = tr.firstChild().nodeValue();
							QByteArray value = QByteArray::fromBase64(str.toLocal8Bit());
							memcpy(md.mm()->cm.Tr.V(), value.data(), sizeof(Matrix44m::ScalarType) * 16);
						}
					}
				}

				QDomNode renderingOpt = mesh.firstChildElement("RenderingOption");
				if (!renderingOpt.isNull())
				{
					QString value = renderingOpt.firstChild().nodeValue();
					MLRenderingData::GLOptionsType opt;
					if (renderingOpt.attributes().contains("pointSize"))
						opt._perpoint_pointsize = renderingOpt.attributes().namedItem("pointSize").nodeValue().toFloat();
					if (renderingOpt.attributes().contains("wireWidth"))
						opt._perwire_wirewidth = renderingOpt.attributes().namedItem("wireWidth").nodeValue().toFloat();
					if (renderingOpt.attributes().contains("boxColor"))
					{
						QStringList values = renderingOpt.attributes().namedItem("boxColor").nodeValue().split(" ", QString::SkipEmptyParts);
						opt._perbbox_fixed_color = vcg::Color4b(values[0].toInt(), values[1].toInt(), values[2].toInt(), values[3].toInt());
					}
					if (renderingOpt.attributes().contains("pointColor"))
					{
						QStringList values = renderingOpt.attributes().namedItem("pointColor").nodeValue().split(" ", QString::SkipEmptyParts);
						opt._perpoint_fixed_color = vcg::Color4b(values[0].toInt(), values[1].toInt(), values[2].toInt(), values[3].toInt());
					}
					if (renderingOpt.attributes().contains("wireColor"))
					{
						QStringList values = renderingOpt.attributes().namedItem("wireColor").nodeValue().split(" ", QString::SkipEmptyParts);
						opt._perwire_fixed_color = vcg::Color4b(values[0].toInt(), values[1].toInt(), values[2].toInt(), values[3].toInt());
					}
					if (renderingOpt.attributes().contains("solidColor"))
					{
						QStringList values = renderingOpt.attributes().namedItem("solidColor").nodeValue().split(" ", QString::SkipEmptyParts);
						opt._persolid_fixed_color = vcg::Color4b(values[0].toInt(), values[1].toInt(), values[2].toInt(), values[3].toInt());
					}
					MLRenderingData data;
					data.set(opt);
					if (data.deserialize(value.toStdString()))
						rendOpt.insert(std::pair<int, MLRenderingData>(mm->id(), data));
				}

				mesh = mesh.nextSibling();
			}
		}
		// READ IN POINT CORRESPONDECES INCOMPLETO!!
		else if (QString::compare(node.nodeName(), "RasterGroup") == 0)
		{
			QDomNode raster; QString filen, label;
			raster = node.firstChild();
			while (!raster.isNull())
			{
				//return true;
				md.addNewRaster();
				QString labelRaster = raster.attributes().namedItem("label").nodeValue();
				md.rm()->setLabel(labelRaster);
				QDomNode sh = raster.firstChild();
				ReadShotFromQDomNode(md.rm()->shot, sh);

				QDomElement el = raster.firstChildElement("Plane");
				while (!el.isNull())
				{
					QString filen = el.attribute("fileName");
					QFileInfo fi(filen);
					QString sem = el.attribute("semantic");
					QString nm = fi.absoluteFilePath();
					md.rm()->addPlane(new RasterPlane(fi.absoluteFilePath(), RasterPlane::RGBA));
					el = node.nextSiblingElement("Plane");
				}
				raster = raster.nextSibling();
			}
		}
		node = node.nextSibling();
	}

	QDir::setCurrent(tmpDir.absolutePath());
	qf.close();
	return true;
}

QDomElement MeshModelToXML(MeshModel *mp, QDomDocument &doc, const QString& path, bool binary, bool saveViewState, const MLRenderingData& rendOpt = MLRenderingData())
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

QDomElement RasterModelToXML(RasterModel *mp, QDomDocument &doc, bool binary)
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

QDomElement PlaneToXML(RasterPlane* pl, const QString& basePath, QDomDocument& doc)
{
	QDomElement planeElem = doc.createElement("Plane");
	QDir dir(basePath);
	planeElem.setAttribute("fileName", dir.relativeFilePath(pl->fullPathFileName));
	planeElem.setAttribute("semantic", pl->semantic);
	return planeElem;
}

QDomDocument MeshDocumentToXML(MeshDocument &md, bool onlyVisibleLayers, bool saveViewState, bool binary, const std::map<int, MLRenderingData>& rendOpt)
{
	QDomDocument ddoc("MeshLabDocument");

	QDomElement root = ddoc.createElement("MeshLabProject");
	ddoc.appendChild(root);
	QDomElement mgroot = ddoc.createElement("MeshGroup");

	for(MeshModel *mmp : md.meshIterator())
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

	for(RasterModel *rmp: md.rasterIterator())
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

