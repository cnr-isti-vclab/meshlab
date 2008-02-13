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
 Revision 1.2  2008/02/13 15:25:08  gianpaolopalma
 First working version

 Revision 1.1  2008/02/02 13:52:54  gianpaolopalma
 Defined X3D exporter interface

 
 *****************************************************************************/
#ifndef EXPORTERX3D
#define EXPORTERX3D

#include<util_x3d.h>

namespace vcg {
namespace tri {
namespace io {

	template<typename SaveMeshType>
	class ExporterX3D : public UtilX3D
	{
	private:

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

	template<class T>
	static int findInsert(std::vector<T>& list, const T& val, bool multi = false)
	{
		int index;
		if (multi)
		{
			index = list.size();
			list.push_back(val);
		}
		else
		{
			std::vector<T>::const_iterator iter = find(list.begin(), list.end(), val);
			if (iter == list.end())
			{
				index = list.size();
				list.push_back(val);
			}
			else
				index = iter - list.begin();
		}
		return index;
	}
	
	
	inline static void getString(const std::vector<QString>& list, QString& ret)
	{
		ret.reserve(list.size() * 3);
		for (int i = 0; i < list.size(); i++)
		{
			ret.append(list[i] + " ");
			if (((i + 1) % 3) == 0)
				ret.append(QString::number(-1) + " ");
		}
		ret.squeeze();
	}

	
	inline static void getPointString(const std::vector<vcg::Point3f>& list, QString& ret)
	{
		ret.reserve(list.size()*30);
		for (int i = 0; i < list.size(); i++)
			for (int j = 0; j < 3; j++)
				ret.append(QString::number(list[i][j]) + " ");
		ret.squeeze();
	}



	inline static void getColorString(const std::vector<vcg::Color4b>& list, QString& ret)
	{
		ret.reserve(list.size()*16);
		for (int i = 0; i < list.size(); i++)
			for (int j = 0; j < 4; j++)
				ret.append(QString::number(list[i][j]/255.0f) + " ");
		ret.squeeze();
	}




	inline static void getTextCoordString(const std::vector<vcg::TexCoord2<>>& list, QString& ret)
	{
		ret.reserve(list.size()*20);
		for (int i = 0; i < list.size(); i++)
			ret.append(QString::number(list[i].U()) + " " + QString::number(list[i].V()) + " ");
		ret.squeeze();
	}
	
	public:

		static int Save(SaveMeshType& m, const char * filename, const int mask, CallBackPos *cb=0)
		{
			bool bHasPerWedgeTexCoord = (mask & MeshModel::IOM_WEDGTEXCOORD) && m.HasPerWedgeTexCoord();
			bool bHasPerWedgeNormal = (mask & MeshModel::IOM_WEDGNORMAL) && m.HasPerWedgeNormal();
			bool bHasPerWedgeColor = (mask & MeshModel::IOM_WEDGCOLOR) && m.HasPerWedgeColor();
			bool bHasPerVertexColor	= (mask & MeshModel::IOM_VERTCOLOR) && m.HasPerVertexColor();
			bool bHasPerVertexNormal = (mask & MeshModel::IOM_VERTNORMAL) && m.HasPerVertexNormal();
			bool bHasPerVertexTexCoord = (mask & MeshModel::IOM_VERTTEXCOORD) && m.HasPerVertexTexCoord();
			bool bHasPerFaceColor = (mask & MeshModel::IOM_FACECOLOR) && m.HasPerFaceColor();
			bool bHasPerFaceNormal = (mask & MeshModel::IOM_FACENORMAL) && m.HasPerFaceNormal();
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
			for (int i = 0; i < object.size() - 1; i++)
			{
				std::vector<QString> vertexIndex, colorIndex, normalIndex, textureCoordIndex;
				std::vector<vcg::Point3f> vertexList, normalList;
				std::vector<vcg::Color4b> colorList;
				std::vector<vcg::TexCoord2<>> texCoordList;
				QDomElement shape = doc.createElement("Shape");
				int indexTexture = -1;
				if (bHasPerVertexTexCoord)
					indexTexture = object[i]->cV(0)->T().N();
				else if (bHasPerWedgeTexCoord)
					indexTexture = object[i]->cWT(0).N();
				if (indexTexture > -1 && indexTexture < m.textures.size())
				{
					QDomElement appearance = doc.createElement("Appearance");
					QDomElement imageTexture = doc.createElement("ImageTexture");
					imageTexture.setAttribute("url", m.textures[indexTexture].c_str());
					appearance.appendChild(imageTexture);
					shape.appendChild(appearance);
				}
				QDomElement geometry = doc.createElement("IndexedFaceSet");
				geometry.setAttribute("solid", "false");
				for (fi = object[i]; fi != object[i+1]; fi++)
				{
					//typename SaveMeshType::FaceType face = (*fi);
					for (int tt = 0; tt < 3; tt++)
					{
						int tmpSize = vertexList.size();
						int indexVert = findInsert<vcg::Point3f>(vertexList, fi->cP(tt));
						vertexIndex.push_back(QString::number(indexVert));
						vertexSet.insert(fi->P(tt));
						int index;
						if (bHasPerWedgeColor)
						{
							index = findInsert<vcg::Color4b>(colorList, fi->WC(tt));
							colorIndex.push_back(QString::number(index));					
						}
						if (bHasPerWedgeNormal)
						{
							index = findInsert<vcg::Point3f>(normalList, fi->cWN(tt));
							normalIndex.push_back(QString::number(index));			
						}
						if (bHasPerWedgeTexCoord)
						{
							index = findInsert<vcg::TexCoord2<>>(texCoordList, fi->cWT(tt));
							textureCoordIndex.push_back(QString::number(index));			
						}
						if (indexVert >= tmpSize)
						{
							if (bHasPerVertexColor)
								findInsert<vcg::Color4b>(colorList, fi->cV(tt)->C(), true);
							if (bHasPerVertexNormal)
								findInsert<vcg::Point3f>(normalList, fi->cV(tt)->cN(), true);
							if (bHasPerVertexTexCoord)
								findInsert<vcg::TexCoord2<>>(texCoordList, fi->cV(tt)->T(), true);
						}
					}
					if (bHasPerFaceColor)
						findInsert<vcg::Color4b>(colorList, fi->C());
					if (bHasPerFaceNormal)
						findInsert<vcg::Point3f>(normalList, fi->cN());
				}
				QString vertIndexValue, vertCoordValue;
				getString(vertexIndex, vertIndexValue);
				getPointString(vertexList, vertCoordValue);
				QDomElement coordinate = doc.createElement("Coordinate");
				coordinate.setAttribute("point", vertCoordValue);
				geometry.appendChild(coordinate);
				geometry.setAttribute("coordIndex", vertIndexValue);
				if (bHasPerWedgeColor || bHasPerVertexColor || bHasPerFaceColor)
				{
					QString colorValue;
					getColorString(colorList, colorValue);
					QDomElement color = doc.createElement("ColorRGBA");
					color.setAttribute("color", colorValue);
					if (bHasPerFaceColor)
						geometry.setAttribute("colorPerVertex", "false");
					if (bHasPerWedgeColor)
					{
						QString colorIndexString;
						getString(colorIndex, colorIndexString);
						geometry.setAttribute("colorIndex", colorIndexString);
					}
					geometry.appendChild(color);
				}
				if (bHasPerWedgeNormal || bHasPerVertexNormal || bHasPerFaceNormal)
				{
					QString normalValue;
					getPointString(normalList, normalValue);
					QDomElement normal = doc.createElement("Normal");
					normal.setAttribute("vector", normalValue);
					if (bHasPerFaceNormal)
						geometry.setAttribute("normalPerVertex", "false");
					if (bHasPerWedgeNormal)
					{
						QString normalIndexString;
						getString(normalIndex, normalIndexString);
						geometry.setAttribute("normalIndex", normalIndexString);
					}
					geometry.appendChild(normal);
				}
				if (indexTexture != -1 && (bHasPerWedgeTexCoord || bHasPerVertexTexCoord))
				{
					QString texCoordValue;
					getTextCoordString(texCoordList, texCoordValue);
					QDomElement textureCoord = doc.createElement("TextureCoordinate");
					textureCoord.setAttribute("point", texCoordValue);
					if (bHasPerWedgeTexCoord)
					{
						QString texCoordIndexString;
						getString(textureCoordIndex, texCoordIndexString);
						geometry.setAttribute("texCoordIndex", texCoordIndexString);
					}
					geometry.appendChild(textureCoord);
				}
				shape.appendChild(geometry);
				scene.appendChild(shape);
			}
			typename SaveMeshType::VertexIterator vi;
			std::vector<vcg::Point3f> pointVect;
			std::vector<vcg::Color4b> colorVect;
			for (vi = m.vert.begin(); vi != m.vert.end(); vi++)
			{
				if (!vi->IsD() && vertexSet.find(vi->P()) == vertexSet.end())
				{
					pointVect.push_back(vi->P());
					if (bHasPerVertexColor)
						findInsert<vcg::Color4b>(colorVect, vi->C());
				}
			}
			if (pointVect.size() != 0)
			{
				QDomElement shape = doc.createElement("Shape");
				QDomElement pointSet = doc.createElement("PointSet");
				QDomElement coord = doc.createElement("Coordinate");
				QString coordValue;
				getPointString(pointVect, coordValue);
				coord.setAttribute("point", coordValue);
				pointSet.appendChild(coord);
				if (colorVect.size() != 0)
				{
					QDomElement color = doc.createElement("ColorRGBA");
					QString colorValue;
					getColorString(colorVect, colorValue);
					color.setAttribute("color", colorValue);
					pointSet.appendChild(color);
				}
				shape.appendChild(pointSet);
				scene.appendChild(shape);
			}
			QFile file(filename);
			if (!file.open(QIODevice::WriteOnly))
				return E_CANTOPEN;
			QTextStream out(&file);
			out << doc.toString();
			file.close();

			return E_NOERROR;
		}


		static int GetExportMaskCapability()
		{
			int capability = 0;
			//vert
			capability |= MeshModel::IOM_VERTNORMAL;
			capability |= MeshModel::IOM_VERTTEXCOORD;
			capability |= MeshModel::IOM_VERTCOLOR;
			//face
			capability |= MeshModel::IOM_FACECOLOR;
			capability |= MeshModel::IOM_FACENORMAL;
			//wedg
			capability |= MeshModel::IOM_WEDGTEXCOORD;
			capability |= MeshModel::IOM_WEDGNORMAL;
			capability |= MeshModel::IOM_WEDGCOLOR;
			//capability |= MeshModel::IOM_WEDGTEXMULTI;

			return capability;
		}
	};
}
}
}

#endif
