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
 Revision 1.5  2006/02/03 10:04:41  fmazzant
 no significant updated

 Revision 1.4  2006/01/30 14:02:05  fmazzant
 bug-fix

 Revision 1.3  2006/01/29 23:52:43  fmazzant
 correct a small bug

 Revision 1.2  2006/01/29 18:33:42  fmazzant
 added some comment to the code

 Revision 1.1  2006/01/29 16:33:03  fmazzant
 moved export_obj and export_3ds from test/io into meshio/

 Revision 1.34  2006/01/22 23:59:01  fmazzant
 changed default value of diffuse. 1.0 -> 0.8

 Revision 1.33  2006/01/19 09:36:29  fmazzant
 cleaned up history log

 Revision 1.32  2006/01/18 00:45:56  fmazzant
 added control on face's diffuse

 Revision 1.31  2006/01/17 13:48:54  fmazzant
 added capability mask on export file format

 Revision 1.30  2006/01/15 00:45:40  fmazzant
 extend mask exporter for all type file format +

 Revision 1.29  2006/01/14 00:03:26  fmazzant
 added more controls

****************************************************************************/

#ifndef __VCGLIB_EXPORT_OBJ
#define __VCGLIB_EXPORT_OBJ

#include <wrap/callback.h>
#include <vcg/complex/trimesh/allocate.h>
#include <wrap/io_trimesh/io_mask.h>
#include <iostream>
#include <fstream>
#include <map>
#include <QMessageBox>

namespace vcg {
namespace tri {
namespace io {

	/*
		structures material
	*/
	struct Material
	{
		unsigned int index;//index of material

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
			enum of all the types of error
		*/
		enum SaveError
		{
			E_NOERROR,					// 0
			E_CANTOPENFILE,				// 1
			E_CANTCLOSEFILE,			// 2
			E_UNESPECTEDEOF,			// 3
			E_ABORTED,					// 4
			E_NOTDEFINITION,			// 5
			E_NOTVEXTEXVALID,			// 6
			E_NOTFACESVALID				// 7
		};

		/*
			this function takes an index and the relative error message gets back
		*/
		static const char* ErrorMsg(int error)
		{
			static const char* obj_error_msg[] =
			{
					"No errors",							// 0
					"Can't open file",						// 1
					"can't close file",						// 2
					"Premature End of file",				// 3
					"File saving aborted",					// 4
					"Function not defined",					// 5
					"Vertices not valid",					// 6
					"Faces not valid"						// 7
				};

			if(error>7 || error<0) return "Unknown error";
			else return obj_error_msg[error];
		};

		/*
			returns mask of capability one define with what are the saveable information of the format.
		*/
		static int GetExportMaskCapability()
		{
			int capability = 0;
			
			//vert
			capability |= vcg::tri::io::Mask::IOM_VERTNORMAL;

			//face
			capability |= vcg::tri::io::Mask::IOM_FACECOLOR;

			//wedg
			capability |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
			capability |= vcg::tri::io::Mask::IOM_WEDGNORMAL;

			return capability;
		}

		/*
			function which saves in OBJ file format
		*/
		static int SaveASCII(SaveMeshType &m, const char * filename, ObjInfo &oi)	
		{
			CallBackPos *cb = oi.cb;
			
			if(m.vert.size() == 0)
				return E_NOTVEXTEXVALID;
			if(m.face.size() == 0)
				return E_NOTFACESVALID;

			int current = 0;
			int max = m.vert.size()+ m.face.size();
		
			std::vector<Material> materials;
			
			std::string fn(filename);
			int i=fn.size()-1;
			while(fn[--i]!='/');

			FILE *fp;
			fp = fopen(filename,"w");
			if(fp == NULL)return E_CANTOPENFILE;

			fprintf(fp,"####\n#\n# OBJ File Generated by Meshlab\n#\n####\n");
			fprintf(fp,"# Object %s\n#\n# Vertices: %d\n# Faces: %d\n#\n####\n",fn.substr(i+1).c_str(),m.vert.size(),m.face.size());
			
			//library materials
			if(oi.mask & vcg::tri::io::Mask::IOM_FACECOLOR)
				fprintf(fp,"mtllib ./%s.mtl\n\n",fn.substr(i+1).c_str());
			
			//vertexs + normal
			VertexIterator vi;
			std::map<Point3f,int> NormalVertex;
			int numvert = 0;
			int value = 1;
			for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if( !(*vi).IsD() )
			{
				//saves normal per vertex
				if(oi.mask & vcg::tri::io::Mask::IOM_VERTNORMAL | oi.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL) 
				{
					if(AddNewNormalVertex(NormalVertex,(*vi).N(),value))
					{
						fprintf(fp,"vn %f %f %f\n",(*vi).N()[0],(*vi).N()[1],(*vi).N()[2]);
						value++;
					}
				}
				
				//saves vertex
				fprintf(fp,"v %f %f %f\n",(*vi).P()[0],(*vi).P()[1],(*vi).P()[2]);

				if (cb !=NULL)
					(*cb)(100.0 * (float)++current/(float)max, "writing vertices ");
				else
				{ fclose(fp); return E_ABORTED;}
			}
			fprintf(fp,"# %d vertices, %d vertices normals\n\n",m.vert.size(),NormalVertex.size());
			
			//faces + texture coords
			FaceIterator fi;
			std::map<vcg::TCoord2<float>,int> CoordIndexTexture;
			unsigned int material_num = 0;
			int mem_index = 0; //var temporany
			/*int*/ value = 1;//tmp
			for(fi=m.face.begin(); fi!=m.face.end(); ++fi) if( !(*fi).IsD() )
			{
				if(oi.mask & vcg::tri::io::Mask::IOM_FACECOLOR)
				{
					int index = CreateNewMaterial(m,materials,material_num,fi);
					
					if(index == materials.size())//inserts a new element material
					{
						material_num++;
						fprintf(fp,"\nusemtl material_%d\n",materials[index-1].index);
						mem_index = index-1;
					}
					else
					{
						if(index != mem_index)//inserts old name elemente material
						{
							fprintf(fp,"\nusemtl material_%d\n",materials[index].index);
							mem_index=index;
						}
					}
				}

				//saves texture coord
				unsigned int MAX = 3;
				for(unsigned int k=0;k<MAX;k++)
				{
					if(m.HasPerWedgeTexture() && oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD)
					{
						if(AddNewTextureCoord(CoordIndexTexture,(*fi).WT(k),value))
						{
							fprintf(fp,"vt %f %f\n",(*fi).WT(k).u(),(*fi).WT(k).v());
							value++;//ncreases the value number to be associated to the Texture
						}
					}
				}

				fprintf(fp,"f ");
				for(unsigned int k=0;k<MAX;k++)
				{
					int v = -1; 
					// +1 because Obj file format begins from index = 1 but not from index = 0.
					v = GetIndexVertex(m, (*fi).V(k)) + 1;//index of vertex per face
					
					int vt = -1;
					if(oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD)
						vt = GetIndexVertexTexture(CoordIndexTexture,(*fi).WT(k));//index of vertex texture per face

					int vn = -1;
					if(oi.mask & vcg::tri::io::Mask::IOM_VERTNORMAL | oi.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL) 
						vn = GetIndexVertexNormal(m, NormalVertex, v);//index of vertex normal per face.

					//writes elements on file obj
					WriteFacesElement(fp,v,vt,vn);

					if(k!=MAX-1)
						fprintf(fp," ");
					else
						fprintf(fp,"\n");	
				}	
				if (cb !=NULL)
					(*cb)(100.0 * (float)++current/(float)max, "writing faces ");
				else
				{ fclose(fp); return E_ABORTED;}
			}//for
			fprintf(fp,"# %d faces, %d coords texture\n\n",m.face.size(),CoordIndexTexture.size());
			
			fprintf(fp,"# End of File");
			fclose(fp);

			int r = 0;
			if(oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD | oi.mask & vcg::tri::io::Mask::IOM_FACECOLOR)
				r = WriteMaterials(materials, filename,cb);//write material 
			
			if(r!= E_NOERROR)
				return r;
			return E_NOERROR;
		}

		/*
			function which saves in OBJ file format
		*/
		static int SaveBinary(SaveMeshType &m, const char * filename, ObjInfo &oi)
		{
			return E_NOTDEFINITION;
		}

		/*
			function which saves in OBJ file format
		*/
		static int Save(SaveMeshType &m, const char * filename, bool binary, int &mask, CallBackPos *cb=0)
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
			returns index of the vertex
		*/
		inline static int GetIndexVertex(SaveMeshType &m, VertexType *p)
		{
			return p-&*(m.vert.begin());
		}
		
		/*
			returns index of the texture coord
		*/
		inline static int GetIndexVertexTexture(std::map<vcg::TCoord2<float>,int> &m, const vcg::TCoord2<float> &wt)
		{
			int index = m[wt];
			if(index!=0){return index;}
			return -1;
		}

		/*
			returns index of the vertex normal
		*/
		inline static int GetIndexVertexNormal(SaveMeshType &m, std::map<Point3f,int> &ma, unsigned int iv )
		{
			int index = ma[m.vert[iv].N()];
			if(index!=0){return index;}
			return -1;	
		}
		
		/*
			write elements on file
		*/
		inline static void WriteFacesElement(FILE *fp,int v,int vt, int vn)
		{
			fprintf(fp,"%d",v);
			if(vt!=-1)
			{
				fprintf(fp,"/%d",vt);
				if(vn!=-1) 
					fprintf(fp,"/%d",vn);
			}
			else if(vn!=-1)
				fprintf(fp,"//%d",vn);
		}
		
		/*
			adds a new index to the coordinate of Texture if it is the first time 
			which is otherwise met not execute anything
		*/
		inline static bool AddNewTextureCoord(std::map<vcg::TCoord2<float>,int> &m, const vcg::TCoord2<float> &wt,int value)
		{
			int index = m[wt];
			if(index==0){m[wt]=value;return true;}
			return false;
		}

		/*
			adds a new index to the normal per vertex if it is the first time 
			which is otherwise met does not execute anything
		*/
		inline static bool AddNewNormalVertex(std::map<Point3f,int> &m, Point3f &n ,int value)
		{
			int index = m[n];
			if(index==0){m[n]=value;return true;}
			return false;
		}
		
		/*
			creates a new meterial
		*/
		inline static int CreateNewMaterial(SaveMeshType &m, std::vector<Material> &materials, unsigned int index, FaceIterator &fi)
		{			
			unsigned char r = (*fi).C()[0];
			unsigned char g = (*fi).C()[1];
			unsigned char b = (*fi).C()[2];
			unsigned char alpha = (*fi).C()[3];
			
			Point3f diffuse;
			if(r > 1 || g > 1 || b > 1)
				diffuse = Point3f((float)r/255.0,(float)g/255.0,(float)b/255.0);
			else 
				diffuse = Point3f((float)r,(float)g,(float)b);
			
			float Tr;
			if(alpha > 1)
				Tr = (float)alpha/255.0;
			else
				Tr = (float)alpha;
			
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
				return materials.size();
			}
			return i;
		}

		/*
			writes material into file
		*/
		inline static int WriteMaterials(std::vector<Material> &materials, const char * filename, CallBackPos *cb=0)
		{			
			std::string fileName = std::string(filename);
			fileName+=".mtl";
			
			if(materials.size() > 0)
			{
				FILE *fp;
				fp = fopen(fileName.c_str(),"w");
				if(fp==NULL)return E_ABORTED;
				
				fprintf(fp,"#\n# Wavefront material file\n# Converted by Meshlab Group\n#\n\n");
				
				int current = 0;

				for(unsigned int i=0;i<materials.size();i++)
				{
					if (cb !=NULL)
						(*cb)(100.0 * (float)++current/(float)materials.size(), "saving material file ");
					else
					{ fclose(fp); return E_ABORTED;}

					fprintf(fp,"newmtl material_%d\n",materials[i].index);
					fprintf(fp,"Ka %f %f %f\n",materials[i].Ka[0],materials[i].Ka[1],materials[i].Ka[2]);
					fprintf(fp,"Kd %f %f %f\n",materials[i].Kd[0],materials[i].Kd[1],materials[i].Kd[2]);
					fprintf(fp,"Ks %f %f %f\n",materials[i].Ks[0],materials[i].Ks[1],materials[i].Ks[2]);
					fprintf(fp,"Tr %f\n",materials[i].Tr);
					fprintf(fp,"illum %d\n",materials[i].illum);
					fprintf(fp,"Ns %f\n",materials[i].Ns);

					if(materials[i].map_Kd.size()>0)
						fprintf(fp,"map_Kd %s\n",materials[i].map_Kd.c_str());
					fprintf(fp,"\n");
				}
				fclose(fp);
			}
			return E_NOERROR;
		}

		/*
			returns the index of the material if it exists inside the list of the materials, 
			otherwise it returns -1.
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
