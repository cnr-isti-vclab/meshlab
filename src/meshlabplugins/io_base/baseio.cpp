/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "baseio.h"
#include "load_project.h"
#include "save_project.h"

#include <QTextStream>

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/import_off.h>
#include <wrap/io_trimesh/import_ptx.h>
#include <wrap/io_trimesh/import_fbx.h>
#include <wrap/io_trimesh/import_vmi.h>
#include <wrap/io_trimesh/import_gts.h>

#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/export_stl.h>
#include <wrap/io_trimesh/export_obj.h>
#include <wrap/io_trimesh/export_vrml.h>
#include <wrap/io_trimesh/export_dxf.h>
#include <wrap/io_trimesh/export_vmi.h>
#include <wrap/io_trimesh/export_gts.h>
#include <wrap/io_trimesh/export.h>

using namespace std;
using namespace vcg;

class PEdge;
class PFace;
class PVertex;
struct PUsedTypes : public UsedTypes<Use<PVertex>   ::AsVertexType,
	Use<PEdge>     ::AsEdgeType,
	Use<PFace>     ::AsFaceType> {};

class PVertex : public Vertex<PUsedTypes, vertex::Coord3f, vertex::Normal3f, vertex::Qualityf, vertex::Color4b, vertex::BitFlags  > {};
class PEdge : public Edge< PUsedTypes, edge::VertexRef, edge::BitFlags> {};
class PFace :public vcg::Face<
	PUsedTypes,
	face::PolyInfo, // this is necessary  if you use component in vcg/simplex/face/component_polygon.h
	face::PFVAdj,   // Pointer to the vertices (just like FVAdj )
	face::Color4b,
	face::BitFlags, // bit flags
	face::Normal3f, // normal
	face::WedgeTexCoord2f
> {};

class PMesh : public tri::TriMesh< vector<PVertex>, vector<PEdge>, vector<PFace>   > {};

const static std::list<FileFormat> importImageFormatList = {
	FileFormat("Windows Bitmap", "BMP"),
	FileFormat("Joint Photographic Experts Group", {"JPG", "JPEG"}),
	FileFormat("Portable Network Graphics", "PNG"),
	FileFormat("Truevision Graphics Adapter", "TGA"),
	FileFormat("Tagged Image File Format", {"TIF", "TIFF"}),
	FileFormat("X11 Bitmap", "XBM"),
	FileFormat("X11 Bitmap", "XPM")
};

const static std::list<FileFormat> exportImageFormatList = {
	FileFormat("Windows Bitmap", "BMP"),
	FileFormat("Joint Photographic Experts Group", {"JPG", "JPEG"}),
	FileFormat("Portable Network Graphics", "PNG"),
	FileFormat("Tagged Image File Format", {"TIF", "TIFF"}),
	FileFormat("X11 Bitmap", "XBM"),
	FileFormat("X11 Bitmap", "XPM")
};

BaseMeshIOPlugin::BaseMeshIOPlugin() : IOPlugin()
{
}

QString BaseMeshIOPlugin::pluginName() const
{
	return "IOBase";
}

/*
	returns the list of the file's type which can be imported
*/
std::list<FileFormat> BaseMeshIOPlugin::importFormats() const
{
	std::list<FileFormat> formatList = {
		FileFormat("Stanford Polygon File Format", tr("PLY")),
		FileFormat("STL File Format", tr("STL")),
		FileFormat("Alias Wavefront Object", tr("OBJ")),
		FileFormat("Quad Object", tr("QOBJ")),
		FileFormat("Object File Format", tr("OFF")),
		FileFormat("PTX File Format", tr("PTX")),
		FileFormat("VCG Dump File Format", tr("VMI")),
		FileFormat("FBX Autodesk Interchange Format", tr("FBX"))
	};
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
std::list<FileFormat> BaseMeshIOPlugin::exportFormats() const
{
	std::list<FileFormat> formatList = {
		FileFormat("Stanford Polygon File Format", tr("PLY")),
		FileFormat("STL File Format", tr("STL")),
		FileFormat("Alias Wavefront Object", tr("OBJ")),
		FileFormat("Object File Format", tr("OFF")),
		FileFormat("VRML File Format", tr("WRL")),
		FileFormat("DXF File Format", tr("DXF"))
	};
	return formatList;
}

std::list<FileFormat> BaseMeshIOPlugin::importImageFormats() const
{
	return importImageFormatList;
}

std::list<FileFormat> BaseMeshIOPlugin::exportImageFormats() const
{
	return exportImageFormatList;
}

std::list<FileFormat> BaseMeshIOPlugin::importProjectFormats() const
{
	std::list<FileFormat> formatList = {
		FileFormat("MeshLab Project", tr("MLP")),
		FileFormat("MeshLab Binary Project", tr("MLB")),
		FileFormat("Align Project", tr("ALN")),
		FileFormat("Bundler Output", tr("OUT")),
		FileFormat("VisualSFM Output", tr("NVM"))
	};
	return formatList;
}

std::list<FileFormat> BaseMeshIOPlugin::exportProjectFormats() const
{
	std::list<FileFormat> formatList = {
		FileFormat("MeshLab Project", tr("MLP")),
		FileFormat("MeshLab Binary Project", tr("MLB")),
		FileFormat("Align Project", tr("ALN"))
	};
	return formatList;
}

// initialize importing parameters
RichParameterList BaseMeshIOPlugin::initPreOpenParameter(const QString &formatName) const
{
	RichParameterList parlst;
	if (formatName.toUpper() == tr("PTX")) {
		parlst.addParam(RichInt("meshindex", 0, "Index of Range Map to be Imported",
			"PTX files may contain more than one range map. 0 is the first range map. If the number if higher than the actual mesh number, the import will fail"));
		parlst.addParam(RichBool("pointsonly", true, "Keep only points", "Import points a point cloud only, with radius and normals, no triangulation involved, isolated points and points with normals with steep angles are removed."));
		parlst.addParam(RichBool("usecolor", true, "import color", "Read color from PTX, if color is not present, uses reflectance instead"));
		parlst.addParam(RichBool("flipfaces", false, "LEICA: flip normal direction", "LEICA PTX exporter goes counterclockwise, FARO PTX exporter goes clockwise"));
		parlst.addParam(RichBool("pointcull", true, "delete unsampled points", "Deletes unsampled points in the grid that are normally located in [0,0,0]"));
		parlst.addParam(RichBool("anglecull", true, "Cull faces by angle", "short"));
		parlst.addParam(RichFloat("angle", 85.0, "Angle limit for face culling", "short"));
	}
	if (formatName.toUpper() == tr("STL")) {
		parlst.addParam(RichBool(
			"unify_vertices", true, "Unify Duplicated Vertices in STL files",
			"The STL format is not an vertex-indexed format. Each triangle is "
			"composed by independent vertices, so, usually, duplicated vertices "
			"should be unified"));
	}
	return parlst;
}

void BaseMeshIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList &parlst, CallBackPos *cb)
{
	//bool normalsUpdated = false;
	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

	if(!QFile::exists(fileName)) {
		throw MLException(errorMsgFormat.arg(fileName, "File does not exist"));
	}
	// initializing mask
	mask = 0;

	// initializing progress bar status
	if (cb != NULL)
		(*cb)(0, "Loading...");

	
	//string filename = fileName.toUtf8().data();
	string filename = QFile::encodeName(fileName).constData();

	if (formatName.toUpper() == tr("PLY"))
	{
		tri::io::ImporterPLY<CMeshO>::LoadMask(filename.c_str(), mask);
		// small patch to allow the loading of per wedge color into faces.
		if (mask & tri::io::Mask::IOM_WEDGCOLOR) mask |= tri::io::Mask::IOM_FACECOLOR;
		m.enable(mask);


		int result = tri::io::ImporterPLY<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result != 0) // all the importers return 0 on success
		{
			if (tri::io::ImporterPLY<CMeshO>::ErrorCritical(result))
			{
				throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterPLY<CMeshO>::ErrorMsg(result)));
			}
		}
	}
	else if (formatName.toUpper() == tr("STL"))
	{
		if (!tri::io::ImporterSTL<CMeshO>::LoadMask(filename.c_str(), mask))
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterSTL<CMeshO>::ErrorMsg(tri::io::ImporterSTL<CMeshO>::E_MALFORMED)));
		}
		m.enable(mask);
		int result = tri::io::ImporterSTL<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result != 0) // all the importers return 0 on success
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterSTL<CMeshO>::ErrorMsg(result)));
		}

		bool stluinf = parlst.getBool("unify_vertices");
		if (stluinf)
		{
			tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
			tri::Allocator<CMeshO>::CompactEveryVector(m.cm);
		}

	}
	else if ((formatName.toUpper() == tr("OBJ")) || (formatName.toUpper() == tr("QOBJ")))
	{
		tri::io::ImporterOBJ<CMeshO>::Info oi;
		oi.cb = cb;
		if (!tri::io::ImporterOBJ<CMeshO>::LoadMask(filename.c_str(), oi)){
			throw MLException("Error while loading OBJ mask.");
		}
		m.enable(oi.mask);

		int result = tri::io::ImporterOBJ<CMeshO>::Open(m.cm, filename.c_str(), oi);
		if (result != tri::io::ImporterOBJ<CMeshO>::E_NOERROR)
		{
			if (result & tri::io::ImporterOBJ<CMeshO>::E_NON_CRITICAL_ERROR) {
				reportWarning(errorMsgFormat.arg(fileName, tri::io::ImporterOBJ<CMeshO>::ErrorMsg(result)));
			}
			else {
				throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterOBJ<CMeshO>::ErrorMsg(result)));
			}
		}

//		if (oi.mask & tri::io::Mask::IOM_WEDGNORMAL)
//			normalsUpdated = true;
		m.enable(oi.mask);
		if (m.hasDataMask(MeshModel::MM_POLYGONAL)) qDebug("Mesh is Polygonal!");
		mask = oi.mask;
	}
	else if (formatName.toUpper() == tr("PTX"))
	{
		tri::io::ImporterPTX<CMeshO>::Info importparams;

		importparams.meshnum = parlst.getInt("meshindex");
		importparams.anglecull = parlst.getBool("anglecull");
		importparams.angle = parlst.getFloat("angle");
		importparams.savecolor = parlst.getBool("usecolor");
		importparams.pointcull = parlst.getBool("pointcull");
		importparams.pointsonly = parlst.getBool("pointsonly");
		importparams.flipfaces = parlst.getBool("flipfaces");

		// if color, add to mesh
		if (importparams.savecolor)
			importparams.mask |= tri::io::Mask::IOM_VERTCOLOR;
		if (importparams.pointsonly)
			importparams.mask |= tri::io::Mask::IOM_VERTRADIUS;

		// reflectance is stored in quality
		importparams.mask |= tri::io::Mask::IOM_VERTQUALITY;

		m.enable(importparams.mask);

		int result = tri::io::ImporterPTX<CMeshO>::Open(m.cm, filename.c_str(), importparams, cb);
		if (result == 1)
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterPTX<CMeshO>::ErrorMsg(result)));
		}

		// update mask
		mask = importparams.mask;
	}
	else if (formatName.toUpper() == tr("OFF"))
	{
		int loadMask;
		if (!tri::io::ImporterOFF<CMeshO>::LoadMask(filename.c_str(), loadMask))
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterOFF<CMeshO>::ErrorMsg(tri::io::ImporterOFF<CMeshO>::InvalidFile)));
		}
		m.enable(loadMask);

		int result = tri::io::ImporterOFF<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result != 0)  // OFFCodes enum is protected
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterOFF<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("VMI"))
	{
		int loadMask;
		if (!tri::io::ImporterVMI<CMeshO>::LoadMask(filename.c_str(), loadMask)) {
			throw MLException("Error while loading VMI mask.");
		}
		m.enable(loadMask);

		int result = tri::io::ImporterVMI<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterOFF<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("GTS"))
	{
		int loadMask;
		if (!tri::io::ImporterGTS<CMeshO>::LoadMask(filename.c_str(), loadMask)){
			throw MLException("Error while loading GTS mask.");
		}
		m.enable(loadMask);

		tri::io::ImporterGTS<CMeshO>::Options opt;
		opt.flipFaces = true;

		int result = tri::io::ImporterGTS<CMeshO>::Open(m.cm, filename.c_str(), mask, opt, cb);
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, vcg::tri::io::ImporterGTS<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("FBX"))
	{
		m.enable(tri::io::Mask::IOM_WEDGTEXCOORD);

		int result = tri::io::ImporterFBX<CMeshO>::Open(m.cm, filename.c_str(),cb);
		if(m.cm.textures.empty())
			m.clearDataMask(tri::io::Mask::IOM_WEDGTEXCOORD);

		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, vcg::tri::io::ImporterFBX<CMeshO>::ErrorMsg(result)));
		}
	}
	else {
		wrongOpenFormat(formatName);
	}

	// Add a small pass to convert backslash into forward slash
	for(auto i = m.cm.textures.begin();i!=m.cm.textures.end();++i)
	{
		std::replace(i->begin(), i->end(), '\\', '/');
	}
//	// verify if texture files are present
//	QString missingTextureFilesMsg = "The following texture files were not found:\n";
//	bool someTextureNotFound = false;
//	for (unsigned textureIdx = 0; textureIdx < m.cm.textures.size(); ++textureIdx)
//	{
//		if (!QFile::exists(m.cm.textures[textureIdx].c_str()))
//		{
//			missingTextureFilesMsg.append("\n");
//			missingTextureFilesMsg.append(m.cm.textures[textureIdx].c_str());
//			someTextureNotFound = true;
//		}
//	}
//	if (someTextureNotFound)
//		log("Missing texture files: %s", qUtf8Printable(missingTextureFilesMsg));

	if (cb != NULL)	(*cb)(99, "Done");
}

void BaseMeshIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterList & par, CallBackPos *cb)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData();
	//string filename = fileName.toUtf8().data();
	string ex = formatName.toUtf8().data();
	bool binaryFlag = false;
	if (formatName.toUpper() == tr("STL") || formatName.toUpper() == tr("PLY"))
		binaryFlag = par.getBool("Binary");

	if (formatName.toUpper() == tr("PLY"))
	{
		tri::io::PlyInfo pi;
		pi.mask = mask;

		vcg::ply::PlyTypes scalarPlyType =
				sizeof(Scalarm) == sizeof(float) ?
					vcg::ply::T_FLOAT :
					vcg::ply::T_DOUBLE;

		// custom attributes
		for (const RichParameter& pr : par) {
			QString pname = pr.name();
			// if pname starts with __CA_VS__, it is a PLY per-vertex scalar custom attribute
			if (pname.startsWith("__CA_VS__")) {
				if (par.getBool(pname)) {        // if it is true, add to save list
					pi.addPerVertexScalarAttribute(qUtf8Printable(pname.mid(4)), scalarPlyType);
				}
			}
			// if pname starts with __CA_VP__, it is a PLY per-vertex point3m custom attribute
			else if (pname.startsWith("__CA_VP__")) {
				if (par.getBool(pname)) {             // if it is true, add to save list
					pi.addPerVertexPoint3mAttribute(qUtf8Printable(pname.mid(5)), scalarPlyType);
				}
			}
			// if pname starts with __CA_FS__, it is a PLY per-face scalar custom attribute
			else if (pname.startsWith("__CA_FS__")) {
				if (par.getBool(pname)) {             // if it is true, add to save list
					pi.addPerFaceScalarAttribute(qUtf8Printable(pname.mid(4)), scalarPlyType);
				}
			}
			// if pname starts with __CA_FP__, it is a PLY per-face point3m custom attribute
			else if (pname.startsWith("__CA_FP__")) {
				if (par.getBool(pname)) {
					pi.addPerFacePoint3mAttribute(qUtf8Printable(pname.mid(5)), scalarPlyType);
				}
			}
		}

		int result = tri::io::ExporterPLY<CMeshO>::Save(m.cm, filename.c_str(), binaryFlag, pi, cb);
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ExporterPLY<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("STL"))
	{
		bool magicsFlag = par.getBool("ColorMode");

		int result = tri::io::ExporterSTL<CMeshO>::Save(m.cm, filename.c_str(), binaryFlag, mask, "STL generated by MeshLab", magicsFlag);
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ExporterSTL<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("WRL"))
	{
		int result = tri::io::ExporterWRL<CMeshO>::Save(m.cm, filename.c_str(), mask, cb);
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ExporterWRL<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("OFF"))
	{
		int result = tri::io::ExporterOFF<CMeshO>::Save(m.cm, filename.c_str(), mask);
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ExporterOFF<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("OBJ"))
	{
		tri::Allocator<CMeshO>::CompactEveryVector(m.cm);
		int result;

		if (mask & tri::io::Mask::IOM_BITPOLYGONAL)
		{
			PMesh pm;
			tri::PolygonSupport<CMeshO, PMesh>::ImportFromTriMesh(pm, m.cm);
			result = tri::io::ExporterOBJ<PMesh>::Save(pm, filename.c_str(), mask, cb);
		}
		else
		{
			result = tri::io::ExporterOBJ<CMeshO>::Save(m.cm, filename.c_str(), mask, cb);
		}
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ExporterOBJ<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("DXF"))
	{
		int result = tri::io::ExporterDXF<CMeshO>::Save(m.cm, filename.c_str());
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ExporterDXF<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("GTS"))
	{
		int result = vcg::tri::io::ExporterGTS<CMeshO>::Save(m.cm, filename.c_str(), mask);
		if (result != 0)
		{
			throw MLException(errorMsgFormat.arg(fileName, vcg::tri::io::ExporterGTS<CMeshO>::ErrorMsg(result)));
		}
	}
	else {
		wrongSaveFormat(formatName);
	}
}

QImage BaseMeshIOPlugin::openImage(
		const QString& format,
		const QString& fileName,
		vcg::CallBackPos*)
{
	QImage loadedImage;
	//format not natively supported by QImage
	if (format.toUpper() == "TGA") {
		loadedImage = loadTga(fileName.toStdString().c_str());
	}
	else { //check if it is a format supported natively by QImage
		bool supportedFormat = false;
		for (const FileFormat& f : importImageFormatList){
			for (const QString& ext : f.extensions){
				if (ext.toUpper() == format.toUpper())
					supportedFormat = true;
			}
		}

		if (supportedFormat) {
			QFileInfo fi(fileName);

			if(!fi.exists()) {
				QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 does not exist.";
				throw MLException(errorMsgFormat.arg(fileName));
			}
			if(!fi.isReadable()) {
				QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
				throw MLException(errorMsgFormat.arg(fileName));
			}

			loadedImage.load(fileName);
			if (loadedImage.isNull()){
				throw MLException("Failed to load the image " + fileName);
			}
		}
		else {
			wrongOpenFormat(format);
		}
	}
	return loadedImage;
}

void BaseMeshIOPlugin::saveImage(
		const QString& format,
		const QString& fileName,
		const QImage& image,
		int quality,
		CallBackPos*)
{
	bool supportedFormat = false;
	for (const FileFormat& f : exportImageFormatList){
		for (const QString& ext : f.extensions){
			if (ext.toUpper() == format.toUpper())
				supportedFormat = true;
		}
	}
	if (supportedFormat){
		bool ok = image.save(fileName, nullptr, quality);
		if (!ok){
			throw MLException("Failed to save the image " + fileName);
		}
	}
	else{
		wrongSaveFormat(format);
	}
}

std::list<FileFormat> BaseMeshIOPlugin::projectFileRequiresAdditionalFiles(
		const QString& format,
		const QString&)
{
	if (format.toUpper() == "OUT"){
		return {FileFormat("Image List File", "TXT")};
	}
	else
		return {};
}

std::vector<MeshModel*> BaseMeshIOPlugin::openProject(
		const QString& format,
		const QStringList& filenames,
		MeshDocument& md,
		std::vector<MLRenderingData>& rendOpt,
		CallBackPos* cb)
{
	std::vector<MeshModel*> meshList;
	rendOpt.clear();
	if (format.toUpper() == "ALN") {
		meshList = loadALN(filenames.first(), md, cb);
	}
	else if (format.toUpper() == "OUT" || format.toUpper() == "NVM" ||
			 format.toUpper() =="MLP" || format.toUpper() == "MLB") {
		std::vector<std::string> unloadedImgs;
		if (format.toUpper() == "OUT") {
			meshList = loadOUT(filenames.first(), filenames[1], md, unloadedImgs, cb);
		}
		else if (format.toUpper() == "NVM"){
			meshList = loadNVM(filenames.first(), md, unloadedImgs, cb);
		}
		else if (format.toUpper() =="MLP" || format.toUpper() == "MLB") {
			meshList = loadMLP(filenames.first(), md, rendOpt, unloadedImgs, cb);
		}
		if (unloadedImgs.size() > 0){
			QString msg = "Unable to load the following " +
				QString::number(unloadedImgs.size()) + " images (using dummy images): \n";

			unsigned int size = unloadedImgs.size();
			if (size > 5)
				size = 5;

			for (unsigned int i = 0; i < size; ++i)
				msg += QString::fromStdString(unloadedImgs[i]) + "\n";

			if (unloadedImgs.size() > 5){
				msg += "\n...\n\n";
				msg += QString::fromStdString(unloadedImgs[unloadedImgs.size()-1]) + "\n";
			}
			reportWarning(msg);
		}
	}

	else {
		wrongOpenFormat(format);
	}
	return meshList;
}

void BaseMeshIOPlugin::saveProject(
		const QString& format,
		const QString& fileName,
		const MeshDocument& md,
		bool onlyVisibleMeshes,
		const std::vector<MLRenderingData>& rendOpt,
		CallBackPos* cb)
{
	if (format.toUpper() == "MLP" || format.toUpper() == "MLB") {
		saveMLP(fileName, md, onlyVisibleMeshes, rendOpt, cb);
	}
	else if (format.toUpper() == "ALN") {
		saveALN(fileName, md, onlyVisibleMeshes, cb);
	}
	else {
		wrongSaveFormat(format);
	}
}

/*
	returns the mask on the basis of the file's type.
	otherwise it returns 0 if the file format is unknown
*/
void BaseMeshIOPlugin::exportMaskCapability(const QString &format, int &capability, int &defaultBits) const
{
	if (format.toUpper() == tr("PLY")) {
		capability = tri::io::ExporterPLY<CMeshO>::GetExportMaskCapability();
		// For the default bits of the ply format disable flags and normals that usually are not useful.
		defaultBits = capability;
		defaultBits &= (~tri::io::Mask::IOM_FLAGS);
		defaultBits &= (~tri::io::Mask::IOM_VERTNORMAL);
		defaultBits &= (~tri::io::Mask::IOM_FACENORMAL);
	}
	if (format.toUpper() == tr("STL")) {
		capability = tri::io::ExporterSTL<CMeshO>::GetExportMaskCapability();
		defaultBits = capability;
	}
	if (format.toUpper() == tr("OBJ")) {
		capability = defaultBits = tri::io::ExporterOBJ<CMeshO>::GetExportMaskCapability() | tri::io::Mask::IOM_BITPOLYGONAL;
	}
	if (format.toUpper() == tr("OFF")) { capability = defaultBits = tri::io::ExporterOFF<CMeshO>::GetExportMaskCapability(); }
	if (format.toUpper() == tr("WRL")) { capability = defaultBits = tri::io::ExporterWRL<CMeshO>::GetExportMaskCapability(); }
	if (format.toUpper() == tr("DXF")) { capability = defaultBits = tri::io::Mask::IOM_VERTCOORD | tri::io::Mask::IOM_FACEINDEX;}

}

RichParameterList BaseMeshIOPlugin::initSaveParameter(const QString &format, const MeshModel &m) const
{
	RichParameterList par;
	if (format.toUpper() == tr("STL") || format.toUpper() == tr("PLY"))
		par.addParam(RichBool(
			"Binary",
			true,
			"Binary encoding",
			"Save the mesh using a binary encoding. If false the mesh is saved in a plain, "
			"readable ascii format."));

	if (format.toUpper() == tr("STL"))
		par.addParam(RichBool(
			"ColorMode",
			true,
			"Materialise Color Encoding",
			"Save the color using a binary encoding according to the Materialise's Magic style "
			"(e.g. RGB coding instead of BGR coding)."));

	if (format.toUpper() == tr("PLY")) {
		std::vector<std::string> attribNameVector;
		vcg::tri::Allocator<CMeshO>::GetAllPerVertexAttribute<Scalarm>(m.cm, attribNameVector);
		for (int i = 0; i < (int) attribNameVector.size(); i++) {
			QString ca_name = attribNameVector[i].c_str();
			par.addParam(RichBool(
				"__CA_VS__" + ca_name,
				false,
				"V(f): " + ca_name,
				"Save this custom scalar (f) per-vertex attribute.",
				false,
				"Custom attributes"));
		}
		vcg::tri::Allocator<CMeshO>::GetAllPerVertexAttribute<Point3m>(m.cm, attribNameVector);
		for (int i = 0; i < (int) attribNameVector.size(); i++) {
			QString ca_name = attribNameVector[i].c_str();
			par.addParam(RichBool(
				"__CA_VP__" + ca_name,
				false,
				"V(3f): " + ca_name,
				"Save this custom vector (3f) per-vertex attribute.",
				false,
				"Custom attributes"));
		}
		vcg::tri::Allocator<CMeshO>::GetAllPerFaceAttribute<Scalarm>(m.cm, attribNameVector);
		for (int i = 0; i < (int) attribNameVector.size(); i++) {
			QString ca_name = attribNameVector[i].c_str();
			par.addParam(RichBool(
				"__CA_FS__" + ca_name,
				false,
				"F(f): " + ca_name,
				"Save this custom scalar (f) per-face attribute.",
				false,
				"Custom attributes"));
		}
		vcg::tri::Allocator<CMeshO>::GetAllPerFaceAttribute<Point3m>(m.cm, attribNameVector);
		for (int i = 0; i < (int) attribNameVector.size(); i++) {
			QString ca_name = attribNameVector[i].c_str();
			par.addParam(RichBool(
				"__CA_FP__" + ca_name,
				false,
				"F(3f): " + ca_name,
				"Save this custom vector (3f) per-face attribute.",
				false,
				"Custom attributes"));
		}
	}
	return par;
}

/**
 * @brief BaseMeshIOPlugin::loadTga
 * source: https://forum.qt.io/topic/74712/qimage-from-tga-with-alpha/11
 */
QImage BaseMeshIOPlugin::loadTga(const char* filePath)
{
	QImage img;
	if (!img.load(filePath)) {

		// open the file
		std::fstream fsPicture(filePath, std::ios::in | std::ios::binary);

		if (!fsPicture.is_open()) {
			throw MLException("Impossible to open the file");
		}

		// some variables
		std::vector<std::uint8_t>* vui8Pixels;
		std::uint32_t ui32BpP;
		std::uint32_t ui32Width;
		std::uint32_t ui32Height;

		// read in the header
		std::uint8_t ui8x18Header[19] = { 0 };
		fsPicture.read(reinterpret_cast<char*>(&ui8x18Header), sizeof(ui8x18Header) - 1);

		//get variables
		vui8Pixels = new std::vector<std::uint8_t>;
		bool bCompressed;
		std::uint32_t ui32IDLength;
		std::uint32_t ui32PicType;
		std::uint32_t ui32PaletteLength;
		std::uint32_t ui32Size;

		// extract all information from header
		ui32IDLength = ui8x18Header[0];
		ui32PicType = ui8x18Header[2];
		ui32PaletteLength = ui8x18Header[6] * 0x100 + ui8x18Header[5];
		ui32Width = ui8x18Header[13] * 0x100 + ui8x18Header[12];
		ui32Height = ui8x18Header[15] * 0x100 + ui8x18Header[14];
		ui32BpP = ui8x18Header[16];

		// calculate some more information
		ui32Size = ui32Width * ui32Height * ui32BpP / 8;
		bCompressed = ui32PicType == 9 || ui32PicType == 10;
		vui8Pixels->resize(ui32Size);

		// jump to the data block
		fsPicture.seekg(ui32IDLength + ui32PaletteLength, std::ios_base::cur);

		if (ui32PicType == 2 && (ui32BpP == 24 || ui32BpP == 32)) {
			fsPicture.read(reinterpret_cast<char*>(vui8Pixels->data()), ui32Size);
		}
		// else if compressed 24 or 32 bit
		else if (ui32PicType == 10 && (ui32BpP == 24 || ui32BpP == 32)) { // compressed
			std::uint8_t tempChunkHeader;
			std::uint8_t tempData[5];
			unsigned int tempByteIndex = 0;

			do {
				fsPicture.read(reinterpret_cast<char*>(&tempChunkHeader), sizeof(tempChunkHeader));

				if (tempChunkHeader >> 7) { // repeat count
					// just use the first 7 bits
					tempChunkHeader = (uint8_t(tempChunkHeader << 1) >> 1);

					fsPicture.read(reinterpret_cast<char*>(&tempData), ui32BpP / 8);

					for (int i = 0; i <= tempChunkHeader; i++) {
						vui8Pixels->at(tempByteIndex++) = tempData[0];
						vui8Pixels->at(tempByteIndex++) = tempData[1];
						vui8Pixels->at(tempByteIndex++) = tempData[2];
						if (ui32BpP == 32) vui8Pixels->at(tempByteIndex++) = tempData[3];
					}
				}
				else { // data count
					// just use the first 7 bits
					tempChunkHeader = (uint8_t(tempChunkHeader << 1) >> 1);

					for (int i = 0; i <= tempChunkHeader; i++) {
						fsPicture.read(reinterpret_cast<char*>(&tempData), ui32BpP / 8);

						vui8Pixels->at(tempByteIndex++) = tempData[0];
						vui8Pixels->at(tempByteIndex++) = tempData[1];
						vui8Pixels->at(tempByteIndex++) = tempData[2];
						if (ui32BpP == 32) vui8Pixels->at(tempByteIndex++) = tempData[3];
					}
				}
			} while (tempByteIndex < ui32Size);
		}
		// not useable format
		else {
			fsPicture.close();
			throw MLException("Not useable TGA format");
		}

		fsPicture.close();

		img = QImage(ui32Width, ui32Height, QImage::Format_RGB888);

		int pixelSize = ui32BpP == 32 ? 4 : 3;
		//TODO: write direct into img
		for (unsigned int x = 0; x < ui32Width; x++) {
			for (unsigned int y = 0; y < ui32Height; y++) {
				int valr = vui8Pixels->at(y * ui32Width * pixelSize + x * pixelSize + 2);
				int valg = vui8Pixels->at(y * ui32Width * pixelSize + x * pixelSize + 1);
				int valb = vui8Pixels->at(y * ui32Width * pixelSize + x * pixelSize);

				QColor value(valr, valg, valb);
				img.setPixelColor(x, y, value);
			}
		}

		img = img.mirrored();

	}
	return img;
}

MESHLAB_PLUGIN_NAME_EXPORTER(BaseMeshIOPlugin)
