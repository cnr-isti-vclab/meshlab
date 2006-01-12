/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
 Revision 1.24  2006/01/12 16:16:36  fmazzant
 diminished the time completely of the export_obj

 Revision 1.23  2006/01/11 15:55:14  fmazzant
 bug-fix in vertex-normal

 Revision 1.22  2006/01/10 16:52:19  fmazzant
 update ply::PlyMask -> io::Mask

 Revision 1.21  2006/01/04 16:51:44  fmazzant
 changed PM_VERTEXCOORD in PM_WEDGTEXCOORD

 Revision 1.20  2005/12/23 10:12:51  fmazzant
 deleted bug to save a face

 Revision 1.19  2005/12/16 14:58:28  fmazzant
 deleted old code

 Revision 1.18  2005/12/15 01:00:51  fmazzant
 added the +1 to the value of return from function GetIndexVertex(m, (*fi).V(k))

 Revision 1.17  2005/12/14 22:21:49  cignoni
 Changed GetIndexVertex from O(n) to O(1)

 Revision 1.16  2005/12/14 18:08:24  fmazzant
 added generic save of all type define obj, ply, off, stl

 Revision 1.15  2005/12/14 10:49:00  mariolatronico
 check on textures size [ with Federico Mazzanti supervision ]

 Revision 1.14  2005/12/14 08:38:55  fmazzant
 bugfix

 Revision 1.13  2005/12/14 01:21:15  fmazzant
 bug-fix

 Revision 1.12  2005/12/14 00:06:19  fmazzant
 deleted bug meterial index usemtl.

 Revision 1.11  2005/12/13 14:02:51  fmazzant
 added the rescue of the materials of the obj

 Revision 1.10  2005/12/11 08:33:57  fmazzant
 deleted method.

 Revision 1.9  2005/12/09 18:16:14  fmazzant
 added generic obj save with plugin arch.

 Revision 1.8  2005/12/07 08:01:09  fmazzant
 exporter obj temporany

 Revision 1.7  2005/12/03 09:45:43  fmazzant
 adding to mask how much we save a obj file format. the mask taken from the dialogue window SaveMaskDialog.

 Revision 1.6  2005/12/02 17:49:27  fmazzant
 added support for the rescue of the normal per vertex

 Revision 1.5  2005/11/30 00:44:07  fmazzant
 added:
 1. save TCoord2 with struct map
 2. callback
 3. define static member to the access class Exporter

 Revision 1.4  2005/11/10 00:06:43  fmazzant
 Added comment code

 Revision 1.3  2005/11/10 00:01:50  fmazzant
 Added comment revision

Revision 1.2  2005/11/09 23:36:10  fmazzant
Up to date method Save

Revision 1.1  2005/11/08 17:45:26  fmazzant
Added  implementation of OBJ file exporter.

****************************************************************************/

#ifndef __VCGLIB_EXPORT_OBJ
#define __VCGLIB_EXPORT_OBJ

#include <wrap/callback.h>
#include <vcg/complex/trimesh/allocate.h>
#include <wrap/io_trimesh/io_mask.h>
#include "io_obj.h"
#include <iostream>
#include <fstream>
#include <map>
#include <QMessageBox>

namespace vcg {
namespace tri {
namespace io {

	struct Material
	{
		unsigned int index;

		Point3f Ka;//ambient
		Point3f Kd;//diffuse
		Point3f Ks;//specular
		
		float d;//alpha
		float Tr;//alpha
		
		int illum;//specular illumination
		float Ns;

		std::string map_Kd; //filename texture
	};

	template <class SaveMeshType>
	class ExporterOBJ
	{
	public:	
		typedef typename SaveMeshType::FaceIterator FaceIterator;
		typedef typename SaveMeshType::VertexIterator VertexIterator;
		typedef typename SaveMeshType::VertexType VertexType;
	
		/*
			salva la Mesh in formato Obj File.
		*/
		static bool SaveASCII(SaveMeshType &m, const char * filename, ObjInfo &oi)	
		{
			CallBackPos *cb = oi.cb;
			
			int current = 0;
			int max = m.vert.size()+ m.face.size();
		
			std::vector<Material> materials;//vettore dei materiali.

			std::ofstream stream(filename);

			if (stream.fail())
				return false; 		

			stream << "####" << std::endl;
			stream << "#" << std::endl;
			stream << "# OBJ File Generated by Meshlab" << std::endl;
			stream << "#" << std::endl;	
			stream << "####" << std::endl;
			QString fileName = QString(filename);
			QStringList list = fileName.split("/");
			stream << "# Object: " <<  list.at(list.size()-1).toStdString() << std::endl;
			stream << "#" << std::endl;
			stream << "# Vertices: " << m.vert.size() << std::endl;
			stream << "# Points: " << 0 <<std::endl;
			stream << "# Lines: " << 0 <<std::endl;
			stream << "# Faces: " << m.face.size() << std::endl;
			stream << "# Materials: " << 0 <<std::endl;
			stream << "#" << std::endl;
			stream << "####" << std::endl << std::endl;
			
			//library materials
			if(oi.mask & vcg::tri::io::Mask::IOM_FACECOLOR)
			{
				QString fileName = QString(filename);
				QStringList list = fileName.split("/");
				stream << std::endl << "mtllib ./" << list.at(list.size()-1).toStdString() << ".mtl" << std::endl << std::endl;
			}

			
			//vertexs + normal
			VertexIterator vi;
			std::map<Point3f,int> NormalVertex;
			if(oi.mask & vcg::tri::io::Mask::IOM_VERTQUALITY)
			{
				int numvert = 0;
				int value = 1;
				for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if( !(*vi).IsD() )
				{
					stream << "v " << (*vi).P()[0] << " " << (*vi).P()[1] << " " << (*vi).P()[2] << std::endl;

					//salva le normali per vertice
					if(oi.mask & vcg::tri::io::Mask::IOM_VERTNORMAL) 
					{
						if(AddNewNormalVertex(NormalVertex,(*vi).N(),value))
						{
							stream << "vn " << (*vi).N()[0] << " " << (*vi).N()[1] << " " << (*vi).N()[2] << std::endl;
							value++;
						}
					}

					if (cb !=NULL)
						(*cb)(100.0 * (float)++current/(float)max, "writing vertices ");
					else
						CancellCallBack(stream);
				}
				stream << "# " << m.vert.size() << " vertices" << std::endl;//stampa numero di vertici come commento
				stream << std::endl;
			}
			
			//faces + texture coords
			FaceIterator fi;
			std::map<vcg::TCoord2<float>,int> CoordIndexTexture;
			if(oi.mask & vcg::tri::io::Mask::IOM_FACEQUALITY)
			{
				unsigned int material_num = 0;
				int mem_index = 0; //var temporany
				int value = 1;//tmp
				for(fi=m.face.begin(); fi!=m.face.end(); ++fi) if( !(*fi).IsD() )
				{
					if(oi.mask & vcg::tri::io::Mask::IOM_FACECOLOR)
					{
						int index = CreateNewMaterial(m,materials,material_num,fi);
						
						if(index == materials.size())//inserted a new element material
						{
							material_num++;
							stream << "usemtl material_" << materials[index-1].index << std::endl;
							mem_index = index-1;
						}
						else
						{
							if(index != mem_index)
							{
								stream << "usemtl material_" << materials[index].index << std::endl;
								mem_index=index;
							}
						}
					}

					//salva le coordinate di texture
					unsigned int MAX = 3;
					for(unsigned int k=0;k<MAX;k++)
					{
						if(m.HasPerWedgeTexture() && oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD)
						{
							if(AddNewTextureCoord(CoordIndexTexture,(*fi).WT(k),value))
							{
								stream << "vt " << (*fi).WT(k).u() << " " << (*fi).WT(k).v() << " " << std::endl;
								value++;//incrementa il numero di valore da associare alle texture
							}
						}
					}

					stream << "f "; 					
					for(unsigned int k=0;k<MAX;k++)
					{
						int v = -1; 
						//il +1 perche' gli obj considerano i vertici a partire da 1 e non da 0.
						v = GetIndexVertex(m, (*fi).V(k)) + 1;//considera i vertici per faccia
						
						int vt = -1;
						if(oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD)
							vt = GetIndexVertexTexture(CoordIndexTexture,(*fi).WT(k));//considera le texture per faccia

						int vn = -1;
						if(oi.mask & vcg::tri::io::Mask::IOM_VERTNORMAL) 
							vn = GetIndexVertexNormal(m, NormalVertex, v);//considera le normali per faccia per ora non va considerato.

						//scrive elementi sul file obj
						WriteFacesElement(stream,v,vt,vn);

						if(k!=MAX-1)
							stream << " ";
						else
							stream << std::endl;		
					}	
					if (cb !=NULL)
						(*cb)(100.0 * (float)++current/(float)max, "writing faces ");
					else
						CancellCallBack(stream);
				}//for
				stream << "# " << m.face.size() << " faces" << std::endl;//stampa numero di facce come commento
				stream << std::endl;
			}
			
			stream << "# End of File" << std::endl;
			stream.close();
			WriteMaterials(materials, filename,cb);//scrive il file dei materiali
			return true;
		}

		static bool SaveBinary(SaveMeshType &m, const char * filename, ObjInfo &oi)
		{
			//TODO: implementare un eventuale salvataggio in binario!
			QMessageBox::warning(new QWidget(),"Warning","not define!");
			return false;
		}

		static bool Save(SaveMeshType &m, const char * filename, bool binary, int &mask, CallBackPos *cb=0)
		{
			ObjInfo oi;
			oi.cb=cb;
			oi.mask=mask;
			if(binary)
				return SaveBinary(m,filename,oi);
			else 
				return SaveASCII(m,filename,oi);
		}

		
		/*
			restituisce l'indice del vertice, aggiunto di una unita'.
		*/
		inline static int GetIndexVertex(SaveMeshType &m, VertexType *p)
		{
			return p-&*(m.vert.begin());
		}
		
		/*
			restituisce l'indice della coordinata di texture.
		*/
		inline static int GetIndexVertexTexture(std::map<vcg::TCoord2<float>,int> &m, const vcg::TCoord2<float> &wt)
		{
			int index = m[wt];
			if(index!=0){return index;}
			return -1;
		}

		/*
			restituisce l'indice della normale.
		*/
		inline static int GetIndexVertexNormal(SaveMeshType &m, std::map<Point3f,int> &ma, unsigned int iv )
		{
			int index = ma[m.vert[iv].N()];
			if(index!=0){return index;}
			return -1;	
		}

		
		/*
			scrive gli elementi su file.
		*/
		inline static void WriteFacesElement(std::ofstream &stream,int v,int vt, int vn)
		{
			stream << v;
			if(vt!=-1)
			{
				stream << "/" << vt;
				if(vn!=-1) stream << "/" << vn;
			}
			else
			{
				if(vn!=-1) stream << "//" << vn;
			}
		}
		
		/*
			aggiunge un nuovo indice alla coordinata di texture se essa e' la prima
			volta che viene incontrata altrimenti non esegue niente.
		*/
		inline static bool AddNewTextureCoord(std::map<vcg::TCoord2<float>,int> &m, const vcg::TCoord2<float> &wt,int value)
		{
			int index = m[wt];
			if(index==0){m[wt]=value;return true;}
			return false;
		}

		/*
			aggiunge una normal restituendo true nel caso dell'insirimento e false in caso contrario
		*/
		inline static bool AddNewNormalVertex(std::map<Point3f,int> &m, Point3f &n ,int value)
		{
			int index = m[n];
			if(index==0){m[n]=value;return true;}
			return false;
		}
		
		/*
			gestione del file material.
		*/
		inline static int CreateNewMaterial(SaveMeshType &m, std::vector<Material> &materials, unsigned int index, FaceIterator &fi)
		{			
			unsigned char r = (*fi).C()[0];
			unsigned char g = (*fi).C()[1];
			unsigned char b = (*fi).C()[2];
			unsigned char alpha = (*fi).C()[3];
			
			Point3f diffuse = Point3f((float)r/255.0,(float)g/255.0,(float)b/255.0);
			float Tr = (float)alpha/255.0;
			
			int illum = 2; //default not use Ks!
			float ns = 0.0; //default

			Material mtl;

			mtl.index = index;//index of materials
			mtl.Ka = Point3f(0.2,0.2,0.2);//ambient
			mtl.Kd = diffuse;//diffuse
			mtl.Ks = Point3f(1.0,1.0,1.0);//specular
			mtl.Tr = Tr;//alpha
			mtl.Ns = ns;
			mtl.illum = illum;//illumination
			
			if(m.textures.size() && (*fi).WT(0).n() >=0 ) {
				
				mtl.map_Kd = m.textures[(*fi).WT(0).n()];
			}

			else
				mtl.map_Kd = "";
			
			int i = -1;
			if((i = MaterialsCompare(materials,mtl)) == -1)
			{
				materials.push_back(mtl);
				return materials.size();
			}
			return i;
		}

		inline static bool WriteMaterials(std::vector<Material> &materials, const char * filename, CallBackPos *cb=0)
		{			
			std::string fileName = std::string(filename);
			fileName+=".mtl";
			
			if(materials.size() > 0)
			{
				std::ofstream stream(fileName.c_str());
				if (stream.fail())
					return false;
				
				stream << "#" << std::endl;
				stream << "# Wavefront material file" << std::endl;
				stream << "# Converted by Meshlab Group" << std::endl;
				stream << "#" << std::endl << std::endl;
				
				int current = 0;

				for(unsigned int i=0;i<materials.size();i++)
				{
					if (cb !=NULL)
						(*cb)(100.0 * (float)++current/(float)materials.size(), "saving material file ");
					else
						CancellCallBack(stream);

					stream << "newmtl " << "material_" << materials[i].index << std::endl;
					stream << "Ka "		<< materials[i].Ka[0] << " " << materials[i].Ka[1] << " " << materials[i].Ka[2] << std::endl;
					stream << "Kd "		<< materials[i].Kd[0] << " " << materials[i].Kd[1] << " " << materials[i].Kd[2] << std::endl;
					stream << "Ks "		<< materials[i].Ks[0] << " " << materials[i].Ks[1] << " " << materials[i].Ks[2] << std::endl;
					stream << "Tr "		<< materials[i].Tr << std::endl;
					stream << "illum "	<< materials[i].illum << std::endl;
					stream << "Ns "		<< materials[i].Ns << std::endl;
					if(materials[i].map_Kd.size()>0)
						stream << "map_Kd "	<< materials[i].map_Kd << std::endl;
					
					stream << std::endl;
				}
				stream.close();
			}
			return true;
		}

		inline static int MaterialsCompare(std::vector<Material> &materials, Material mtl)
		{
			for(int i=0;i<materials.size();i++)
			{
				bool ka = materials[i].Ka == mtl.Ka;
				bool kd = materials[i].Kd == mtl.Kd;
				bool ks = materials[i].Ks == mtl.Ks;
				bool tr = materials[i].Tr == mtl.Tr;
				bool illum = materials[i].illum == mtl.illum;
				bool ns = materials[i].Ns == mtl.Ns;
				bool map = materials[i].map_Kd == mtl.map_Kd;
				if(ka & kd & ks & tr & illum & ns & map){return i;}
			}
			return -1;
		}

		/*
			nel caso in cui la callbackpas sia uguale a NULL viene cancellato qualsiasi file generato dall'esecuzione di save.
		*/
		inline static void CancellCallBack(std::ofstream &out)
		{
			out.close();
		}

	}; // end class

} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif
