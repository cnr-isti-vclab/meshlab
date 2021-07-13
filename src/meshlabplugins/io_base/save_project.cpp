#include "save_project.h"

#include <QDir>
#include <QTextStream>

#include <wrap/qt/shot_qt.h>
#include <wrap/io_trimesh/alnParser.h>
#include <common/mlexception.h>

namespace mlp {

QDomElement matrix44mToXML(const Matrix44m &m, bool binary, QDomDocument &doc)
{
	QDomElement matrixElem = doc.createElement("MLMatrix44");
	QDomText nd;
	if (binary) {
		QByteArray value = QByteArray::fromRawData((char *)m.V(), sizeof(Matrix44m::ScalarType) * 16).toBase64();
		QDomText nd = doc.createTextNode(QString(value));
	}
	else {
		QString Row[4];
		for (int i = 0; i < 4; ++i)
			Row[i] = QString("%1 %2 %3 %4 \n").arg(m[i][0]).arg(m[i][1]).arg(m[i][2]).arg(m[i][3]);

		nd = doc.createTextNode("\n" + Row[0] + Row[1] + Row[2] + Row[3]);
	}
	matrixElem.appendChild(nd);

	return matrixElem;
}

QDomElement meshModelToXML(const MeshModel& mp, QDomDocument &doc, const QString& path, bool binary, bool saveViewState, const MLRenderingData& rendOpt = MLRenderingData())
{
	QDomElement meshElem = doc.createElement("MLMesh");
	meshElem.setAttribute("label", mp.label());
	meshElem.setAttribute("filename", mp.relativePathName(path));
	meshElem.setAttribute("visible", saveViewState? mp.isVisible() : true);
	meshElem.setAttribute("idInFile", mp.idInFile());

	meshElem.appendChild(matrix44mToXML(mp.cm.Tr, binary, doc));

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

QDomElement planeToXML(const RasterPlane* pl, const QString& basePath, QDomDocument& doc)
{
	QDomElement planeElem = doc.createElement("Plane");
	QDir dir(basePath);
	planeElem.setAttribute("fileName", dir.relativeFilePath(pl->fullPathFileName));
	planeElem.setAttribute("semantic", pl->semantic);
	return planeElem;
}

QDomElement rasterModelToXML(const RasterModel *mp, QDomDocument &doc, const QString& pathName, bool binary)
{
	QDomElement rasterElem = doc.createElement("MLRaster");
	rasterElem.setAttribute("label", mp->label());
	if (binary)
		rasterElem.appendChild(WriteShotToQDomNodeBinary(mp->shot, doc));
	else
		rasterElem.appendChild(WriteShotToQDomNode(mp->shot, doc));
	for (int ii = 0; ii < mp->planeList.size(); ++ii)
		rasterElem.appendChild(planeToXML(mp->planeList[ii], pathName, doc));
	return rasterElem;
}

QDomDocument meshDocumentToXML(
		const MeshDocument &md,
		bool onlyVisibleLayers,
		bool binary,
		const std::vector<MLRenderingData>& rendOpt)
{
	QDomDocument ddoc("MeshLabDocument");

	QDomElement root = ddoc.createElement("MeshLabProject");
	ddoc.appendChild(root);
	QDomElement mgroot = ddoc.createElement("MeshGroup");

	unsigned int i = 0;
	for(const MeshModel& mmp : md.meshIterator()) {
		if ((!onlyVisibleLayers) || (mmp.isVisible())) {
			QDomElement meshElem;
			if (rendOpt.size() == md.meshNumber())
				meshElem = meshModelToXML(mmp, ddoc, md.pathName(), binary, true, rendOpt[i]);
			else
				meshElem = meshModelToXML(mmp, ddoc, md.pathName(), binary, false);
			mgroot.appendChild(meshElem);
		}
		++i;
	}
	root.appendChild(mgroot);

	QDomElement rgroot = ddoc.createElement("RasterGroup");

	for(const RasterModel *rmp: md.rasterIterator()) {
		QDomElement rasterElem = mlp::rasterModelToXML(rmp, ddoc, md.pathName(), binary);
		rgroot.appendChild(rasterElem);
	}

	root.appendChild(rgroot);

	return ddoc;
}
} // namespace mlp

void saveMLP(
		const QString& filename,
		const MeshDocument& md,
		bool onlyVisibleLayers,
		const std::vector<MLRenderingData>& rendOpt,
		vcg::CallBackPos* cb)
{
	QFileInfo fi(filename);
	bool binary = fi.suffix().toUpper() == "MLB";
	QDir tmpDir = QDir::current();
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	QDomDocument doc = mlp::meshDocumentToXML(md, onlyVisibleLayers, binary, rendOpt);
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream qstream(&file);
	doc.save(qstream, 1);
	file.close();
	QDir::setCurrent(tmpDir.absolutePath());
}

void saveALN(
		const QString& filename,
		const MeshDocument& md,
		bool onlyVisibleLayers,
		vcg::CallBackPos* cb)
{
	std::vector<std::string> meshNameVector;
	std::vector<Matrix44m> transfVector;

	for(const MeshModel& mp : md.meshIterator())
	{
		if((!onlyVisibleLayers) || (mp.isVisible()))
		{
			meshNameVector.push_back(qUtf8Printable(mp.relativePathName(md.pathName())));
			transfVector.push_back(mp.cm.Tr);
		}
	}
	bool ret = ALNParser::SaveALN(qUtf8Printable(filename), meshNameVector, transfVector);
	if (!ret)
		throw MLException("Impossible to save " + filename);
}
