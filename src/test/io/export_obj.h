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
#include <wrap/ply/io_mask.h>
#include "io_obj.h"
#include <iostream>
#include <fstream>
#include <map>

namespace vcg {
namespace tri {
namespace io {

	template <class SaveMeshType>
	class ExporterOBJ
	{
	public:	
		typedef typename SaveMeshType::FaceIterator FaceIterator;
		typedef typename SaveMeshType::VertexIterator VertexIterator;

		/*
			salva la Mesh in formato Obj File.
		*/
		static bool SaveASCII(SaveMeshType &m, const char * filename, ObjInfo &oi)	
		{
			CallBackPos *cb = oi.cb;
			
			//TODO: avere la possibilita' di salvare il colore per vertice
			//TODO: salvare il materiale.

			std::ofstream stream(filename);

			if (stream.fail())
				return false; 
			
			stream << "#" << std::endl;
			stream << "#FILE  : " << filename << std::endl;
			stream << "#TYPE  : Obj File Format" << std::endl;
			stream << "#AUTHOR: Meshlab" << std::endl;
			stream << "#" << std::endl;
			stream << "#Computer Science from the University of Pisa" << std::endl;
			stream << "#" << std::endl;

			//vertexs
			VertexIterator vi;
			if(oi.mask & vcg::ply::PLYMask::PM_VERTQUALITY)
			{	
				int numvert = 0;
				for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
				{
					stream << "v " << (*vi).P()[0] << " " << (*vi).P()[1] << " " << (*vi).P()[2] << std::endl;

					if (cb !=NULL)
						(*cb)(100.0 * (float)++numvert/(float)m.vert.size(), "writing vertices ");
				}
				stream << "# " << m.vert.size() << " vertices" << std::endl;//stampa numero di vertici come commento
				stream << std::endl;
			}
			
			//texture coords
			FaceIterator fi;
			std::map<vcg::TCoord2<float>,int> CoordIndexTexture;
			if(oi.mask & vcg::ply::PLYMask::PM_WEDGTEXCOORD)//controllare maschera(mask)
			{
				int numface = 0;
				int value = 1;
				for(fi=m.face.begin(); fi!=m.face.end(); ++fi) if( !(*fi).IsD() )
				{
					for(unsigned int k=0;k<3;k++)
					{
						if(AddNewTextureCoord(CoordIndexTexture,(*fi).WT(k),value))
						{
							stream << "vt " << (*fi).WT(k).u() << " " << (*fi).WT(k).v() << " " << (*fi).WT(k).n() << std::endl;
							value++;//incrementa il numero di valore da associare alle texture
						}
					}
					if (cb !=NULL)
						(*cb)(100.0 * (float)++numface/(float)m.face.size(), "writing TCoord2");
				}//for
				stream << "# " << CoordIndexTexture.size() << " vertices texture" << std::endl;//stampa numero di vert di coord di text
				stream << std::endl;
			}
				
			//vertexs normal
			std::map<Point3f,int> NormalVertex;
			if(oi.mask & vcg::ply::PLYMask::PM_WEDGNORMAL)//controllare maschera(mask)
			{
				int numvert = 0;
				int value = 1;
				for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
				{
					if(AddNewNormalVertex(NormalVertex,(*vi).N(),value))
					{
						stream << "vn " << (*vi).N()[0] << " " << (*vi).N()[1] << " " << (*vi).N()[2] << std::endl;
						value++;
					}
					if (cb !=NULL)
						(*cb)(100.0 * (float)++numvert/(float)m.vert.size(), "writing vertices normal");					
				}
				stream << "# " << NormalVertex.size() << " normals per vertex " << std::endl;//stampa numero di vert di coord di text
				stream << std::endl;
			}
			
			//faces
			if(oi.mask & vcg::ply::PLYMask::PM_FACEQUALITY)//controllare maschera(mask)
			{
				int numface = 0;
				for(fi=m.face.begin(); fi!=m.face.end(); ++fi) if( !(*fi).IsD() )
				{
					stream << "f "; 					
					unsigned int MAX = 3;
					for(unsigned int k=0;k<MAX;k++)
					{
						int v = -1; 
						v = GetIndexVertex(m, (*fi).V(k)->P());//considera i vertici per faccia
						
						int vt = -1;
						vt = GetIndexVertexTexture(CoordIndexTexture,(*fi).WT(k));//considera le texture per faccia

						int vn = -1;
						vn = GetIndexVertexNormal(m, NormalVertex, v);//considera le normali per faccia per ora non va considerato.

						//scrive elementi sul file obj
						WriteFacesElement(stream,v,vt,vn);

						if(k!=MAX-1)
							stream << " ";
						else
							stream << std::endl;		
					}	
					if (cb !=NULL)
						(*cb)(100.0 * (float)++numface/(float)m.face.size(), "writing faces ");
				}//for
				stream << "# " << m.face.size() << " faces" << std::endl;//stampa numero di facce come commento
				stream << std::endl;
			}
			
			stream.close();
			return true;
		}

		static bool SaveBinary(SaveMeshType &m, const char * filename, ObjInfo &oi)
		{
			//TODO: implementare un eventuale salvataggio in binario!
			return Save(m,filename,oi);
		}

		static bool Save(SaveMeshType &m, const char * filename, ObjInfo &oi)
		{
			return Save(m,filename,false,oi);
		}
		static bool Save(SaveMeshType &m, const char * filename, bool binary, ObjInfo &oi)
		{
			if(binary)
				return SaveBinary(m,filename,oi);
			else 
				return SaveASCII(m,filename,oi);
		}

		static bool Save(SaveMeshType &m, const char * filename, bool binary, CallBackPos *cb=0)
		{
			ObjInfo oi;
			oi.cb=cb;

			if(binary)
				return SaveBinary(m,filename,oi);
			else 
				return SaveASCII(m,filename,oi);
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
		inline static int GetIndexVertex(SaveMeshType &m,Point3f p)
		{
			for(unsigned int i=0;i<m.vert.size();i++)
				if(m.vert[i].P() == p)
					return ++i;
			return-1;
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
		
	}; // end class

} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif
