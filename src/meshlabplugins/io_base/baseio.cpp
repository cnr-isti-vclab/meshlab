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


// initialize importing parameters
void BaseMeshIOPlugin::initPreOpenParameter(const QString &formatName, RichParameterList &parlst)
{
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
		parlst.addParam(RichBool(stlUnifyParName(), true, "Unify Duplicated Vertices in STL files", "The STL format is not an vertex-indexed format. Each triangle is composed by independent vertices, so, usually, duplicated vertices should be unified"));
	}
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
		m.Enable(mask);


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
		m.Enable(mask);
		int result = tri::io::ImporterSTL<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result != 0) // all the importers return 0 on success
		{
			throw MLException(errorMsgFormat.arg(fileName, tri::io::ImporterSTL<CMeshO>::ErrorMsg(result)));
		}

		bool stluinf = parlst.getBool(stlUnifyParName());
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
		m.Enable(oi.mask);

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
		m.Enable(oi.mask);
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

		m.Enable(importparams.mask);

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
		m.Enable(loadMask);

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
		m.Enable(loadMask);

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
		m.Enable(loadMask);

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
		m.Enable(tri::io::Mask::IOM_WEDGTEXCOORD);

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
	// verify if texture files are present
	QString missingTextureFilesMsg = "The following texture files were not found:\n";
	bool someTextureNotFound = false;
	for (unsigned textureIdx = 0; textureIdx < m.cm.textures.size(); ++textureIdx)
	{
		if (!QFile::exists(m.cm.textures[textureIdx].c_str()))
		{
			missingTextureFilesMsg.append("\n");
			missingTextureFilesMsg.append(m.cm.textures[textureIdx].c_str());
			someTextureNotFound = true;
		}
	}
	if (someTextureNotFound)
		log("Missing texture files: %s", qUtf8Printable(missingTextureFilesMsg));

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
			if (pname.startsWith("PVAF")) { // if pname starts with PVAF, it is a PLY per-vertex float custom attribute
				if (par.getBool(pname)) { // if it is true, add to save list
					pi.addPerVertexScalarAttribute(qUtf8Printable(pname.mid(4)), scalarPlyType);
				}
			}
			else if (pname.startsWith("PVA3F")) { // if pname starts with PVA3F, it is a PLY per-vertex point3f custom attribute
				if (par.getBool(pname)) { // if it is true, add to save list
					pi.addPerVertexPoint3mAttribute(qUtf8Printable(pname.mid(5)), scalarPlyType);
				}
			}
			else if (pname.startsWith("PFAF")) { // if pname starts with PFAF, it is a PLY per-face float custom attribute
				if (par.getBool(pname)) { // if it is true, add to save list
					pi.addPerFaceScalarAttribute(qUtf8Printable(pname.mid(4)), scalarPlyType);
				}
			}
			else if (pname.startsWith("PFA3F")){ // if pname starts with PFA3F, it is a PLY per-face point3f custom attribute
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
		if (mask & tri::io::Mask::IOM_BITPOLYGONAL)
			m.updateDataMask(MeshModel::MM_FACEFACETOPO);
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
			m.updateDataMask(MeshModel::MM_FACEFACETOPO);
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

//void BaseMeshIOPlugin::initOpenParameter(const QString &format, MeshModel &/*m*/, RichParameterSet &par)
//{
//    if(format.toUpper() == tr("STL"))
//        par.addParam(new RichBool("Unify",true, "Unify Duplicated Vertices",
//                                "The STL format is not an vertex-indexed format. Each triangle is composed by independent vertices, so, usually, duplicated vertices should be unified"));
//}

void BaseMeshIOPlugin::initSaveParameter(const QString &format, const MeshModel &m, RichParameterList &par)
{
	if (format.toUpper() == tr("STL") || format.toUpper() == tr("PLY"))
		par.addParam(RichBool("Binary", true, "Binary encoding",
		"Save the mesh using a binary encoding. If false the mesh is saved in a plain, readable ascii format."));

	if (format.toUpper() == tr("STL"))
		par.addParam(RichBool("ColorMode", true, "Materialise Color Encoding",
		"Save the color using a binary encoding according to the Materialise's Magic style (e.g. RGB coding instead of BGR coding)."));

	if (format.toUpper() == tr("PLY")){
		std::vector<std::string> AttribNameVector;
		vcg::tri::Allocator<CMeshO>::GetAllPerVertexAttribute< Scalarm >(m.cm, AttribNameVector);
		for (int i = 0; i < (int)AttribNameVector.size(); i++)
		{
			QString va_name = AttribNameVector[i].c_str();
			par.addParam(RichBool("PVAF" + va_name, false, "V(f): " + va_name, "Save this custom scalar (f) per-vertex attribute."));
		}
		vcg::tri::Allocator<CMeshO>::GetAllPerVertexAttribute< Point3m >(m.cm, AttribNameVector);
		for (int i = 0; i < (int)AttribNameVector.size(); i++)
		{
			QString va_name = AttribNameVector[i].c_str();
			par.addParam(RichBool("PVA3F" + va_name, false, "V(3f): " + va_name, "Save this custom vector (3f) per-vertex attribute."));
		}
		vcg::tri::Allocator<CMeshO>::GetAllPerFaceAttribute< Scalarm >(m.cm, AttribNameVector);
		for (int i = 0; i < (int)AttribNameVector.size(); i++)
		{
			QString va_name = AttribNameVector[i].c_str();
			par.addParam(RichBool("PFAF" + va_name, false, "F(f): " + va_name, "Save this custom scalar (f) per-face attribute."));
		}
		vcg::tri::Allocator<CMeshO>::GetAllPerFaceAttribute< Point3m >(m.cm, AttribNameVector);
		for (int i = 0; i < (int)AttribNameVector.size(); i++)
		{
			QString va_name = AttribNameVector[i].c_str();
			par.addParam(RichBool("PFA3F" + va_name, false, "F(3f): " + va_name, "Save this custom vector (3f) per-face attribute."));
		}
	}
}

//void BaseMeshIOPlugin::applyOpenParameter(const QString &format, MeshModel &m, const RichParameterSet &par)
//{
//    if(format.toUpper() == tr("STL"))
//		if (par.findParameter(stlUnifyParName())->value().getBool())
//		{
//			tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
//			tri::Allocator<CMeshO>::CompactEveryVector(m.cm);
//		}
//}

MESHLAB_PLUGIN_NAME_EXPORTER(BaseMeshIOPlugin)
