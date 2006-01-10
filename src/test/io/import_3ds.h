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

#ifndef __VCGLIB_IMPORT_3DS
#define __VCGLIB_IMPORT_3DS

#include <wrap/callback.h>
#include <vcg/complex/trimesh/allocate.h>
#include "io_3ds.h"
#include <wrap/io_trimesh/io_mask.h>

// lib3ds headers
#include <lib3ds/file.h>                        
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>



namespace vcg {
namespace tri {
namespace io {

/** 
This class encapsulate a filter for importing 3ds meshes.
It uses the lib3ds library.
*/
template <class OpenMeshType>
class Importer3DS
{
public:

typedef typename OpenMeshType::VertexPointer VertexPointer;
typedef typename OpenMeshType::ScalarType ScalarType;
typedef typename OpenMeshType::VertexType VertexType;
typedef typename OpenMeshType::FaceType FaceType;
typedef typename OpenMeshType::VertexIterator VertexIterator;
typedef typename OpenMeshType::FaceIterator FaceIterator;


enum _3DSError {
		// Successfull opening
	E_NOERROR,								// 0
		// Opening Errors
	E_CANTOPEN,								// 1
	E_UNESPECTEDEOF,					// 2
	E_ABORTED,								// 3

	E_NO_VERTEX,							// 4
	E_NO_FACE,								// 5
	E_LESS_THAN_3VERTINFACE,	// 6
	E_BAD_VERT_INDEX,					// 7
	E_BAD_TEX_VERT_INDEX 			// 8
};

static const char* ErrorMsg(int error)
{
  static const char* _3ds_error_msg[] =
  {
		"No errors",												// 0
		"Can't open file",									// 1
		"Premature End of file",						// 2
		"File opening aborted",							// 3
		"No vertex field found",						// 4
		"No face field found",							// 5
		"Face with less than 3 vertices",		// 6
		"Bad vertex index in face",					// 7
		"Bad texture index in face"					// 8
	};

  if(error>8 || error<0) return "Unknown error";
  else return _3ds_error_msg[error];
};

/*!
* Opens a 3ds file and populates the mesh passed as first parameter
* accordingly to read data
* \param m The mesh model to be populated with data stored into the file
* \param filename The name of the file to open
* \param oi A structure containing infos about the object to be opened
*/
static int Open( OpenMeshType &m, const char * filename, _3dsInfo &info)
{
	m.Clear();

	CallBackPos *cb = info.cb;

	// TODO: call LoadMask
	// if LoadMask has not been called yet, we call it here
	/*if (oi.mask == -1)
		LoadMask(filename, oi.mask, oi);
	
	if (info.numVertices == 0)
		return E_NO_VERTEX;

	if (info.numTriangles == 0)
		return E_NO_FACE;
	*/

	static Lib3dsFile *file=0;
	file = lib3ds_file_load(filename);
  if (!file)
   return E_CANTOPEN;
	lib3ds_file_eval(file,0);

	
	// vertices and faces allocatetion
	VertexIterator vi;// = Allocator<OpenMeshType>::AddVertices(m,oi.numVertices);
	FaceIterator   fi;// = Allocator<OpenMeshType>::AddFaces(m,oi.numTriangles);

	
	Lib3dsNode *p;
  for (p=file->nodes; p!=0; p=p->next) {
      ReadNode(m, file, p, vi, fi);
  }
	
  return E_NOERROR;
} // end of Open


	static bool ReadNode(OpenMeshType &m, Lib3dsFile* file, Lib3dsNode *node, VertexIterator &vi, FaceIterator &fi)
	{
		ASSERT(file);

		{
			Lib3dsNode *p;
			for (p=node->childs; p!=0; p=p->next)
				ReadNode(m, file, p, vi, fi);
		}
	  
		if (node->type==LIB3DS_OBJECT_NODE)
		{
			if (strcmp(node->name,"$$$DUMMY") == 0)
				return false;
		
			if (!node->user.d)
			{
				Lib3dsMesh * mesh = lib3ds_file_mesh_by_name(file, node->name);
				ASSERT(mesh);
				if (!mesh)
				  return false;
      
				
				
				Lib3dsVector *normalL= (Lib3dsVector*) malloc(3*sizeof(Lib3dsVector)*mesh->faces);

				{
					Lib3dsMatrix M;
					lib3ds_matrix_copy(M, mesh->matrix);
					lib3ds_matrix_inv(M);

					// TODO: fare qualcosa con la matrice
					// forse bisogna moltiplicarla alle coordinate
					// dei punti e alle normali
					//glMultMatrixf(&M[0][0]);
				}
				lib3ds_mesh_calculate_normals(mesh, normalL);


				// allocazione spazio per i vertici e le facce della mesh corrente
				vi = Allocator<OpenMeshType>::AddVertices(m ,mesh->points);
				fi = Allocator<OpenMeshType>::AddFaces(m ,mesh->faces);

				
				for (unsigned v=0; v<mesh->points; ++v) {
					Lib3dsPoint			*p		= &mesh->pointL[v];
					
					(*vi).P()[0] = p->pos[0];
					(*vi).P()[1] = p->pos[1];
					(*vi).P()[2] = p->pos[2];

					++vi;
				}

				for (unsigned p=0; p<mesh->faces; ++p) {
					Lib3dsFace			*f		= &mesh->faceL[p];
					Lib3dsMaterial	*mat	=	0;
					if (f->material[0])
						mat = lib3ds_file_material_by_name(file, f->material);
						
					Point4f faceColor;

					if (mat)
					{
						//static GLfloat a[4]={0,0,0,1};
						faceColor = Point4f(mat->diffuse);
						//mat->specular;
						//float s = pow(2, 10.0*mat->shininess);
						//if (s>128.0)	s=128.0;
					}
					else {
						//Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
						faceColor = Point4f(0.8, 0.8, 0.8, 1.0);
						//Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
					}
						

					// assigning face color
					// --------------------
					(*fi).C()[0] = faceColor[0];
					(*fi).C()[1] = faceColor[1];
					(*fi).C()[2] = faceColor[2];
					(*fi).C()[3] = faceColor[3];
				

					// assigning face normal
					// ---------------------
					(*fi).N() = f->normal;
					
					for (int i=0; i<3; ++i)
					{
						// TODO: normale per wedge
						(*fi).WN(i) = normalL[3*p+i];
								
						// assigning face vertices
						// -----------------------
						(*fi).V(i) = &(m.vert[ f->points[i] ]);
					}

					++fi;
				}

        free(normalL);
      }
    }

    /*if (node->user.d) {
      Lib3dsObjectData *d;

      glPushMatrix();
      d=&node->data.object;
      glMultMatrixf(&node->matrix[0][0]);
      glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
      glCallList(node->user.d);
      glPopMatrix();
    }*/
	}

	/*!
	* Retrieves kind of data stored into the file and fills a mask appropriately
	* \param filename The name of the file to open
	*	\param mask	A mask which will be filled according to type of data found in the object
	*/
	static bool LoadMask(const char * filename, int &mask)
	{
		_3dsInfo info;
		return LoadMask(filename, mask, info);
	}

	
	/*!
	* Retrieves kind of data stored into the file and fills a mask appropriately
	* \param filename The name of the file to open
	*	\param mask	A mask which will be filled according to type of data found in the object
	* \param oi A structure which will be filled with infos about the object to be opened
	*/
	static bool LoadMask(const char * filename, int &mask, _3dsInfo &info)
	{
		std::ifstream stream(filename);
		if (stream.fail())
			return false;

		bool bHasPerWedgeTexCoord = false;
		bool bHasPerWedgeNormal		= false;
		bool bUsingMaterial				= false;
		bool bHasPerVertexColor		= false;
		bool bHasPerFaceColor			= false;

		// TODO: add code

		mask=0;
		
		if (bHasPerWedgeTexCoord)
			mask |= ply::PLYMask::PM_WEDGTEXCOORD;
		if (bHasPerWedgeNormal)
			mask |= ply::PLYMask::PM_WEDGNORMAL;
		if (bHasPerVertexColor)
			mask |= ply::PLYMask::PM_VERTCOLOR;
		if (bHasPerFaceColor)
			mask |= ply::PLYMask::PM_FACECOLOR;
		
		oi.mask = mask;
		oi.numVertices	= numVertices;
		oi.numTriangles = numTriangles;

		/*if( pf.AddToRead(VertDesc(0))!=-1 && 
				pf.AddToRead(VertDesc(1))!=-1 && 
				pf.AddToRead(VertDesc(2))!=-1 )   mask |= ply::PLYMask::PM_VERTCOORD;

		if( pf.AddToRead(VertDesc(3))!=-1 )		mask |= ply::PLYMask::PM_VERTFLAGS;
		if( pf.AddToRead(VertDesc(4))!=-1 )		mask |= ply::PLYMask::PM_VERTQUALITY;
		if( pf.AddToRead(VertDesc(8))!=-1 )		mask |= ply::PLYMask::PM_VERTQUALITY;
		if( ( pf.AddToRead(VertDesc(5))!=-1 ) && 
				( pf.AddToRead(VertDesc(6))!=-1 ) &&
				( pf.AddToRead(VertDesc(7))!=-1 )  )  mask |= ply::PLYMask::PM_VERTCOLOR;
		
		if( pf.AddToRead(FaceDesc(0))!=-1 ) mask |= ply::PLYMask::PM_FACEINDEX;
		if( pf.AddToRead(FaceDesc(1))!=-1 ) mask |= ply::PLYMask::PM_FACEFLAGS;

		if( pf.AddToRead(FaceDesc(2))!=-1 ) mask |= ply::PLYMask::PM_FACEQUALITY;
		if( pf.AddToRead(FaceDesc(5))!=-1 ) mask |= ply::PLYMask::PM_WEDGTEXMULTI;
		if( pf.AddToRead(FaceDesc(4))!=-1 ) mask |= ply::PLYMask::PM_WEDGCOLOR;
		if( ( pf.AddToRead(FaceDesc(6))!=-1 ) && 
				( pf.AddToRead(FaceDesc(7))!=-1 ) &&
				( pf.AddToRead(FaceDesc(8))!=-1 )  )  mask |= ply::PLYMask::PM_FACECOLOR;
		*/

		return true;
	}

}; // end class
} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif  // ndef __VCGLIB_IMPORT_3DS