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
 Revision 1.11  2006/01/12 23:53:17  fmazzant
 deleted part of texture base

 Revision 1.10  2006/01/12 16:38:45  fmazzant
 update code & clean code

 Revision 1.9  2006/01/11 16:32:43  fmazzant
 added comment-code for coord text

 Revision 1.8  2006/01/10 16:52:19  fmazzant
 update ply::PlyMask -> io::Mask

 Revision 1.7  2005/12/23 10:24:37  fmazzant
 added base save 3ds materials

 Revision 1.6  2005/12/16 17:26:05  fmazzant
 cleaned up code

 Revision 1.5  2005/12/16 15:30:17  fmazzant
 added in Save 3ds vertexs & faces

 Revision 1.4  2005/12/16 13:04:04  fmazzant
 update method SaveBinary. generete empty file 3ds.

 Revision 1.3  2005/12/16 00:37:31  fmazzant
 update base export_3ds.h + callback

 Revision 1.2  2005/12/15 15:26:33  fmazzant
 update lib3ds

 Revision 1.1  2005/12/15 12:27:58  fmazzant
 first commit 3ds

****************************************************************************/

#ifndef __VCGLIB_EXPORT_3DS
#define __VCGLIB_EXPORT_3DS

#include <wrap/callback.h>
#include <vcg/complex/trimesh/allocate.h>
#include <wrap/io_trimesh/io_mask.h>

#include <lib3ds/file.h>
#include <lib3ds/io.h>
#include <lib3ds/mesh.h>
#include <lib3ds/types.h>
#include <lib3ds/material.h>

#include <iostream>
#include <fstream>

#include <QMessageBox>

namespace vcg {
namespace tri {
namespace io {


	template <class SaveMeshType>
	class Exporter3DS
	{
	public:	
		typedef typename SaveMeshType::FaceIterator FaceIterator;
		typedef typename SaveMeshType::VertexIterator VertexIterator;
		typedef typename SaveMeshType::VertexType VertexType;
	
		static bool SaveASCII(SaveMeshType &m, const char * filename)	
		{
			QMessageBox::warning(new QWidget(),"Warning","Save not implemented!");
			return false;
		}

		static bool SaveBinary(SaveMeshType &m, const char * filename, CallBackPos *cb=0)
		{
			Lib3dsFile *file = lib3ds_file_new();//crea un nuovo file
			Lib3dsMesh *mesh = lib3ds_mesh_new("mesh");//crea una nuova mesh con nome mesh

			std::vector<Material> materials;

			int current = 0;
			int max = m.vert.size()+m.face.size();
			
			lib3ds_mesh_new_point_list(mesh, m.vert.size());//definisce il numero di vertici
			int v_index = 0;
			VertexIterator vi;
			for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if( !(*vi).IsD() )
			{
				Lib3dsPoint point;
				point.pos[0] = (*vi).P()[0];
				point.pos[1] = (*vi).P()[1];
				point.pos[2] = (*vi).P()[2];

				mesh->pointL[v_index] = point;

				if (cb !=NULL)
					(*cb)(100.0 * (float)++current/(float)max, "writing vertices ");
				else
					return false;
				v_index++;
			}
			
			lib3ds_mesh_new_face_list (mesh, m.face.size());
			int f_index = 0;
			FaceIterator fi;
			for(fi=m.face.begin(); fi!=m.face.end(); ++fi) if( !(*fi).IsD() )
			{
				Lib3dsFace face;
				face.points[0] = GetIndexVertex(m, (*fi).V(0));
				face.points[1] = GetIndexVertex(m, (*fi).V(1));
				face.points[2] = GetIndexVertex(m, (*fi).V(2));
				face.flags = 0;
				face.smoothing = 10;//da modificare.
				face.normal[0] = (*fi).N()[0];
				face.normal[1] = (*fi).N()[1];
				face.normal[2] = (*fi).N()[2];
				
				int material_index = 0;
				if((material_index = CreateNewMaterial(m, materials, 0, fi)) == materials.size())
				{
					Lib3dsMaterial *material = lib3ds_material_new();//cre un nuovo materiale
					material->name[0] = 'm';
					material->name[1] = 'A' + material_index;
					
					//ambient
					material->ambient[0] = materials[materials.size()-1].Ka[0];
					material->ambient[1] = materials[materials.size()-1].Ka[1];
					material->ambient[2] = materials[materials.size()-1].Ka[2];
					material->ambient[3] = materials[materials.size()-1].Tr;

					//diffuse
					material->diffuse[0] = materials[materials.size()-1].Kd[0];
					material->diffuse[1] = materials[materials.size()-1].Kd[1];
					material->diffuse[2] = materials[materials.size()-1].Kd[2];
					material->diffuse[3] = materials[materials.size()-1].Tr;

					//specular
					material->specular[0] = materials[materials.size()-1].Ks[0];
					material->specular[1] = materials[materials.size()-1].Ks[1];
					material->specular[2] = materials[materials.size()-1].Ks[2];
					material->specular[3] = materials[materials.size()-1].Tr;

					//shininess
					material->shininess = materials[materials.size()-1].Ns;
					
					unsigned int MAX = 3;
					for(unsigned int k=0;k<MAX;k++)
					{
						if(m.HasPerWedgeTexture() /*&& oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD*/)
						{
							//(*fi).WT(k).u();
							//(*fi).WT(k).v();
						}
					}					

					lib3ds_file_insert_material(file,material);//inserisce il materiale nella mesh
					face.material[0] = 'm';//associa alla faccia il materiale.
					face.material[1] = 'A' + material_index;//l'idice del materiale...
				}
				else
				{	
					face.material[0] = 'm';//associa alla faccia il materiale.
					face.material[1] = 'A' + material_index;//l'idice del materiale...
				}

				mesh->faceL[f_index]=face;

				if (cb !=NULL)
					(*cb)(100.0 * (float)++current/(float)max, "writing faces ");
				else 
					return false;
				f_index++;
			}
			
			//salva la mesh in 3ds
			lib3ds_file_insert_mesh(file, mesh);//inserisce la mesh al file
			bool result = lib3ds_file_save(file, filename); //salva il file
			
			return result;
		}
		
		static bool Save(SaveMeshType &m, const char * filename, bool binary,CallBackPos *cb=0)
		{
			return SaveBinary(m,filename,cb);	
		}

		/*
			restituisce l'indice del vertice.
		*/
		inline static int GetIndexVertex(SaveMeshType &m, VertexType *p)
		{
			return p-&*(m.vert.begin());
		}

		/*
			crea un nuovo materiale
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
			
			if(m.textures.size() && (*fi).WT(0).n() >=0 ) 		
				mtl.map_Kd = m.textures[(*fi).WT(0).n()];
			else
				mtl.map_Kd = "";
			
			int i = -1;
			if((i = MaterialsCompare(materials,mtl)) == -1)
			{
				materials.push_back(mtl);
				index++;
				return materials.size();
			}
			return i;
		}
		/*
			compara il materiale.
		*/
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
	}; // end class

} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif
