/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
  History

 $Log$
 Revision 1.5  2008/02/15 08:27:44  cignoni
 - '>> 'changed into '> >'
 - Used HasPerFaceSomething(M) instead of M.HasPerFaceSomething() that is deprecated.
 - many unsigned warning removed
 - added const to some functions parameters (FindDEF, FindAndReplaceUSE ...)

 Revision 1.4  2008/02/15 01:09:06  gianpaolopalma
 Added control to check if face is deleted

 Revision 1.3  2008/02/14 13:09:53  gianpaolopalma
 Code refactoring to reduce time to save the mesh in X3D files

 Revision 1.2  2008/02/13 15:25:08  gianpaolopalma
 First working version

 Revision 1.1  2008/02/02 13:52:54  gianpaolopalma
 Defined X3D exporter interface

 
 *****************************************************************************/
#ifndef EXPORTERX3D
#define EXPORTERX3D

#include "util_x3d.h"

namespace vcg {
namespace tri {
namespace io {

	template<typename SaveMeshType>
	class ExporterX3D : public UtilX3D
	{
	private:

		typedef vcg::Point3f x3dPointType;
		typedef vcg::Color4b x3dColorType;
		typedef vcg::TexCoord2<> x3dTexCoordType;

		/*Generate the following xml code:
		<X3D profile='Immersive' version='3.1' xmlns:xsd='http://www.w3.org/2001/XMLSchema-instance' xsd:noNamespaceSchemaLocation='http://www.web3d.org/specifications/x3d-3.1.xsd'>
			<head>
				<meta content='*enter filename' name='title'/>
				<meta content='Generated from Meshlab X3D Exported' name='description'/>
				<meta content='*enter date' name='created'/>
				<meta content='Meshlab X3D Exported, http://meshlab.sourceforge.net' name='generator'/>
			</head>
			<Scene>
				...
			</Scene>
		</X3D>
		*/
		inline static QDomElement createHeader(QDomDocument& doc, const char * filename)
		{
			QDomProcessingInstruction proc = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
			doc.appendChild(proc);
			QString metaName[] = {"title", "description", "created", "generator"};
			QString metaContent[] = {QFileInfo(filename).fileName(), "Generated from Meshlab X3D Exported", QDate::currentDate().toString("d MMMM yyyy"), "Meshlab X3D Exported, http://meshlab.sourceforge.net"};
			QDomElement root = doc.createElement("X3D");
			root.setAttribute("profile", "Immersive");
			root.setAttribute("version", "3.1");
			root.setAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema-instance");
			root.setAttribute("xsd:noNamespaceSchemaLocation", "http://www.web3d.org/specifications/x3d-3.1.xsd");
			doc.appendChild(root);
			QDomElement head = doc.createElement("head");
			for (int i = 0; i < 4; i++)
			{
				QDomElement meta = doc.createElement("meta");
				meta.setAttribute("name", metaName[i]);
				meta.setAttribute("content", metaContent[i]);
				head.appendChild(meta);
			}
			root.appendChild(head);
			QDomElement scene = doc.createElement("Scene");
			root.appendChild(scene);
			return scene;
		}

	
	/*If separator is true create the following string:
			list[0] + " " + list[1] + " " + list[2] + " -1 " + ...+ " -1 " +list[i] + " " + list[i+1] + " " + list[i+2] + " -1"
		othewise:
			list[0] + " " + list[1] + " " + ... + list[i]
	*/
	inline static void getString(const std::vector<QString>& list, QString& ret, bool separator = true)
	{
		if (list.size() > 0)
		{
			ret.reserve(list.size() * (list[0].length() + 2));
			ret.append(list[0]);
			for (size_t i = 1; i < list.size(); i++)
			{
				ret.append(" " + list[i]);
				if (separator && ((i + 1) % 3) == 0)
					ret.append(" " + QString::number(-1));
			}
			ret.squeeze();
		}
	}

	

	//Create the following string from a point:		p[0] + " " + p[1] + " " + p[2]
	inline static QString pointToString(const x3dPointType& p)
	{
		QString str;
		for (int j = 0; j < 3; j++)
			str.append(QString::number(p[j]) + " ");
		str.remove(str.size()-1, 1);
		return str;
	}

	

	//Create the following string from a color:		c[0] + " " + c[1] + " " + c[2] + " " + c[3]
	inline static QString colorToString(const x3dColorType& c)
	{
		QString str;
		vcg::Color4f color;
		color.Import(c);
		for (int j = 0; j < 4; j++)
			str.append(QString::number(color[j]) + " ");
		str.remove(str.size()-1, 1);
		return str;
	}

	

	//Create the following string:		t.U()+ " " + t.V()
	inline static QString texCoordToString(const x3dTexCoordType& t)
	{
		QString str;
		str.append(QString::number(t.U()) + " ");
		str.append(QString::number(t.V()));
		return str;
	}

	
	public:


		static int Save(SaveMeshType& m, const char * filename, const int mask, CallBackPos *cb=0)
		{
			QFile file(filename);
			if (!file.open(QIODevice::WriteOnly))
				return E_CANTOPEN;
			int nFace = 0;
			bool bHasPerWedgeTexCoord = (mask & Mask::IOM_WEDGTEXCOORD) && HasPerWedgeTexCoord(m);
			bool bHasPerWedgeNormal = (mask & Mask::IOM_WEDGNORMAL) && HasPerWedgeNormal(m);
			bool bHasPerWedgeColor = (mask & Mask::IOM_WEDGCOLOR) && HasPerWedgeColor(m);
			bool bHasPerVertexColor	= (mask & Mask::IOM_VERTCOLOR) && m.HasPerVertexColor();
			bool bHasPerVertexNormal = (mask & Mask::IOM_VERTNORMAL) && m.HasPerVertexNormal();
			bool bHasPerVertexTexCoord = (mask & Mask::IOM_VERTTEXCOORD) && m.HasPerVertexTexCoord();
			bool bHasPerFaceColor = (mask & Mask::IOM_FACECOLOR) && HasPerFaceColor(m);
			bool bHasPerFaceNormal = (mask & Mask::IOM_FACENORMAL) && HasPerFaceNormal(m);
			if (bHasPerWedgeTexCoord && bHasPerVertexTexCoord)
				return E_INVALIDSAVETEXCOORD;
			if ((bHasPerVertexColor && (bHasPerWedgeColor || bHasPerFaceColor)) || (bHasPerWedgeColor && bHasPerFaceColor))
				return E_INVALIDSAVECOLOR;
			if ((bHasPerVertexNormal && (bHasPerWedgeNormal || bHasPerFaceNormal)) || (bHasPerWedgeNormal && bHasPerFaceNormal))
				return E_INVALIDSAVENORMAL;
			if (m.vert.size() == 0)
				return E_NOGEOMETRY;
			QDomDocument doc("X3D PUBLIC \"ISO//Web3D//DTD X3D 3.1//EN\" \"http://www.web3d.org/specifications/x3d-3.1.dtd\"");
			QDomElement scene = createHeader(doc, filename);
			int indexTexture = -2;
			std::vector<typename SaveMeshType::FaceIterator> object;
			typename SaveMeshType::FaceIterator fi;
			if (bHasPerWedgeTexCoord || bHasPerVertexTexCoord)
			{
				//Search objects in the mesh(an object is a portion of mesh with the same texture index) 
				for (fi = m.face.begin(); fi != m.face.end(); fi++)
				{
					if (!fi->IsD())
					{
						int n;
						if (bHasPerVertexTexCoord)
							n = fi->cV(0)->T().N();
						else if (bHasPerWedgeTexCoord)
							n = fi->cWT(0).N();
						if (n != indexTexture)
						{
							indexTexture = n;
							object.push_back(fi);
						}
					}
				}
			}
			else
				object.push_back(m.face.begin());
			object.push_back(m.face.end());
			std::set<typename SaveMeshType::VertexType::CoordType> vertexSet;
			if (cb !=NULL) (*cb)(10, "Saving X3D File...");
			//Create a Shape element for each object
			for (size_t i = 0; i < object.size() - 1; i++)
			{
				std::vector<QString> vertexIndex, colorIndex, normalIndex, textureCoordIndex;
				std::vector<QString> vertexValue, colorValue, normalValue, textureCoordValue;
				std::map<x3dPointType, int> vertexMap, normalMap;
				std::map<x3dColorType, int> colorMap;
				std::map<x3dTexCoordType, int> texCoordMap;
				QDomElement shape = doc.createElement("Shape");
				int indexTexture = -1;
				if (bHasPerVertexTexCoord)
					indexTexture = object[i]->cV(0)->T().N();
				else if (bHasPerWedgeTexCoord)
					indexTexture = object[i]->cWT(0).N();
				if (indexTexture > -1 && indexTexture < int( m.textures.size()))
				{
					QDomElement appearance = doc.createElement("Appearance");
					QDomElement imageTexture = doc.createElement("ImageTexture");
					imageTexture.setAttribute("url", m.textures[indexTexture].c_str());
					appearance.appendChild(imageTexture);
					shape.appendChild(appearance);
				}
				QDomElement geometry = doc.createElement("IndexedFaceSet");
				geometry.setAttribute("solid", "false");
				int indexVertex = 0;
				int indexColor = 0;
				int indexNormal = 0;
				int indexTexCoord = 0;
				for (fi = object[i]; fi != object[i+1]; fi++)
				{
					nFace++;
					if(!fi->IsD())
					{
						for (int tt = 0; tt < 3; tt++)
						{
							bool newVertex = false;
							std::map<x3dPointType, int>::iterator pointIter = vertexMap.find(fi->P(tt));
							if (pointIter == vertexMap.end())
							{
								vertexIndex.push_back(QString::number(indexVertex));
								vertexValue.push_back(pointToString(fi->P(tt)));
								vertexMap[fi->P(tt)] = indexVertex++;
								vertexSet.insert(fi->P(tt));
								newVertex = true;
							}
							else
								vertexIndex.push_back(QString::number(pointIter->second));
							if (bHasPerWedgeColor)
							{
								std::map<x3dColorType, int>::iterator colorIter = colorMap.find(fi->WC(tt));
								if (colorIter == colorMap.end())
								{
									colorIndex.push_back(QString::number(indexColor));
									colorValue.push_back(colorToString(fi->WC(tt)));
									colorMap[fi->WC(tt)] = indexColor++;
								}
								else
									colorIndex.push_back(QString::number(colorIter->second));
							}
							if (bHasPerWedgeNormal)
							{
								std::map<x3dPointType, int>::iterator normalIter = normalMap.find(fi->WN(tt));
								if (normalIter == normalMap.end())
								{
									normalIndex.push_back(QString::number(indexNormal));
									normalValue.push_back(pointToString(fi->WN(tt)));
									normalMap[fi->WN(tt)] = indexNormal++;
								}	
								else
									normalIndex.push_back(QString::number(normalIter->second));
							}
							if (bHasPerWedgeTexCoord)
							{
								std::map<x3dTexCoordType, int>::iterator texIter = texCoordMap.find(fi->WT(tt));
								if (texIter == texCoordMap.end())
								{
									textureCoordIndex.push_back(QString::number(indexTexCoord));
									textureCoordValue.push_back(texCoordToString(fi->WT(tt)));
									texCoordMap[fi->WT(tt)] = indexTexCoord++;
								}
								else
									textureCoordIndex.push_back(QString::number(texIter->second));
							}
							if (newVertex)
							{
								if (bHasPerVertexColor)
									colorValue.push_back(colorToString(fi->cV(tt)->C()));
								if (bHasPerVertexNormal)
									normalValue.push_back(pointToString(fi->cV(tt)->cN()));
								if (bHasPerVertexTexCoord)
									textureCoordValue.push_back(texCoordToString(fi->cV(tt)->T()));
							}
						}
						if (bHasPerFaceColor)
							colorValue.push_back(colorToString(fi->C()));
						if (bHasPerFaceNormal)
							normalValue.push_back(pointToString(fi->N()));
						if (cb !=NULL && nFace%1000 == 0) (*cb)(10 + 60*nFace/m.face.size(), "Saving X3D File..."); 
					}
				}
				QString vertIndexStr, vertCoordStr;
				getString(vertexIndex, vertIndexStr);
				getString(vertexValue, vertCoordStr, false);
				QDomElement coordinate = doc.createElement("Coordinate");
				coordinate.setAttribute("point", vertCoordStr);
				geometry.appendChild(coordinate);
				geometry.setAttribute("coordIndex", vertIndexStr);
				if (bHasPerWedgeColor || bHasPerVertexColor || bHasPerFaceColor)
				{
					QString colorValueStr;
					getString(colorValue, colorValueStr, false);
					QDomElement color = doc.createElement("ColorRGBA");
					color.setAttribute("color", colorValueStr);
					if (bHasPerFaceColor)
						geometry.setAttribute("colorPerVertex", "false");
					if (bHasPerWedgeColor)
					{
						QString colorIndexStr;
						getString(colorIndex, colorIndexStr);
						geometry.setAttribute("colorIndex", colorIndexStr);
					}
					geometry.appendChild(color);
				}
				if (bHasPerWedgeNormal || bHasPerVertexNormal || bHasPerFaceNormal)
				{
					QString normalValueStr;
					getString(normalValue, normalValueStr, false);
					QDomElement normal = doc.createElement("Normal");
					normal.setAttribute("vector", normalValueStr);
					if (bHasPerFaceNormal)
						geometry.setAttribute("normalPerVertex", "false");
					if (bHasPerWedgeNormal)
					{
						QString normalIndexStr;
						getString(normalIndex, normalIndexStr);
						geometry.setAttribute("normalIndex", normalIndexStr);
					}
					geometry.appendChild(normal);
				}
				if (indexTexture != -1 && (bHasPerWedgeTexCoord || bHasPerVertexTexCoord))
				{
					QString texCoordValueStr;
					getString(textureCoordValue, texCoordValueStr, false);
					QDomElement textureCoord = doc.createElement("TextureCoordinate");
					textureCoord.setAttribute("point", texCoordValueStr);
					if (bHasPerWedgeTexCoord)
					{
						QString texCoordIndexStr;
						getString(textureCoordIndex, texCoordIndexStr);
						geometry.setAttribute("texCoordIndex", texCoordIndexStr);
					}
					geometry.appendChild(textureCoord);
				}
				shape.appendChild(geometry);
				scene.appendChild(shape);
			}
			typename SaveMeshType::VertexIterator vi;
			std::vector<QString> pointVect;
			std::vector<QString> colorVect;
			//Create a PoinrSet element for all isolated vertex
			for (vi = m.vert.begin(); vi != m.vert.end(); vi++)
			{
				if (!vi->IsD() && vertexSet.find(vi->P()) == vertexSet.end())
				{
					pointVect.push_back(pointToString(vi->P()));
					if (bHasPerVertexColor)
						colorVect.push_back(colorToString(vi->C()));
				}
				if (cb !=NULL && ((vi - m.vert.begin())%1000 == 0)) (*cb)(70 + 25*(vi - m.vert.begin())/m.vert.size(), "Saving X3D File...");
			}
			if (pointVect.size() != 0)
			{
				QDomElement shape = doc.createElement("Shape");
				QDomElement pointSet = doc.createElement("PointSet");
				QDomElement coord = doc.createElement("Coordinate");
				QString coordValueStr;
				getString(pointVect, coordValueStr, false);
				coord.setAttribute("point", coordValueStr);
				pointSet.appendChild(coord);
				if (colorVect.size() != 0)
				{
					QDomElement color = doc.createElement("ColorRGBA");
					QString colorValueStr;
					getString(colorVect, colorValueStr, false); 
					color.setAttribute("color", colorValueStr);
					pointSet.appendChild(color);
				}
				shape.appendChild(pointSet);
				scene.appendChild(shape);
			}
			//Print the file
			QTextStream out(&file);
			out << doc.toString();
			file.close();
			return E_NOERROR;
		}
		
		
		
		static int GetExportMaskCapability()
		{
			int capability = 0;
			//vert
			capability |= Mask::IOM_VERTNORMAL;
			capability |= Mask::IOM_VERTTEXCOORD;
			capability |= Mask::IOM_VERTCOLOR;
			//face
			capability |= Mask::IOM_FACECOLOR;
			capability |= Mask::IOM_FACENORMAL;
			//wedg
			capability |= Mask::IOM_WEDGTEXCOORD;
			capability |= Mask::IOM_WEDGNORMAL;
			capability |= Mask::IOM_WEDGCOLOR;
			return capability;
		}
	};
}
}
}

#endif
