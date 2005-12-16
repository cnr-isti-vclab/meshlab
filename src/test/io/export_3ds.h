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
#include <wrap/ply/io_mask.h>

#include <lib3ds/file.h>
#include <lib3ds/io.h>
#include <lib3ds/mesh.h>
#include <lib3ds/types.h>

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
			Lib3dsMesh *mesh = lib3ds_mesh_new("mesh");

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

				mesh->faceL[f_index]=face;
				
				//SALVARE COORDINATE TEXTURE
				//SALVARE MATERIALE.

				if (cb !=NULL)
					(*cb)(100.0 * (float)++current/(float)max, "writing faces ");
				else 
					return false;
				f_index++;
			}
			
			//salva la mesh in 3ds
			Lib3dsFile *file = lib3ds_file_new();//crea un nuovo file
			lib3ds_file_insert_mesh (file, mesh);//inserisce la mesh al file
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
	}; // end class

} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif
