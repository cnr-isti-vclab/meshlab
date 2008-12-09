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
 Revision 1.7  2008/02/04 00:17:43  cignoni
 Now the add of the various types check for the existence of the needed stuff (e.g. per wedge normal are added only if the mesh support them)

 Revision 1.6  2007/11/28 10:28:39  cignoni
 switched to lib3ds v1.3.0 stable.

 Revision 1.5  2006/02/09 17:20:38  buzzelli
 cleaning some comments

 Revision 1.4  2006/01/30 23:10:36  buzzelli
 solved a bug regarding face color loading


*****************************************************************************/
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
static int Open( OpenMeshType &m, const char * filename, Lib3dsFile *file, _3dsInfo &info)
{
	int result = E_NOERROR;

	m.Clear();

	if (file == 0)
	{
		file = lib3ds_file_load(filename);
		if (!file)
			return E_CANTOPEN;
		lib3ds_file_eval(file,0);
	}

	if (info.mask == 0)
		LoadMask(filename, file, info);

	if (info.numVertices == 0)
		return E_NO_VERTEX;

	if (info.numTriangles == 0)
		return E_NO_FACE;
	
	
	// vertices and faces iterators
	VertexIterator vi;
	FaceIterator   fi;

	
	Lib3dsNode *p;
	
	int numVertices = 0;
	int numFaces		= 0;

	p=file->nodes;
  for (p=file->nodes; p!=0; p=p->next)
      if (ReadNode(m, file, p, vi, fi, info, numVertices, numFaces) == E_ABORTED)
			{
				lib3ds_file_free(file);
				return E_ABORTED;
			}
	
	// freeing memory
	lib3ds_file_free(file);

	return result;
} // end of Open


	static int ReadNode(OpenMeshType &m, Lib3dsFile* file, Lib3dsNode *node, VertexIterator &vi, FaceIterator &fi, _3dsInfo &info, int &numVertices, int &numFaces)
	{
		int result = E_NOERROR;
		ASSERT(file);

		{
			Lib3dsNode *p;
			for (p=node->childs; p!=0; p=p->next)
				if (ReadNode(m, file, p, vi, fi, info, numVertices, numFaces) == E_ABORTED)
					return E_ABORTED;
		}

		if (node->type==LIB3DS_OBJECT_NODE)
		{
			if (strcmp(node->name,"$$$DUMMY") == 0)
				return E_NOERROR;
		
			if (!node->user.d)
			{
				Lib3dsMesh * mesh = lib3ds_file_mesh_by_name(file, node->name);
				ASSERT(mesh);
				if (!mesh)
				  return E_NOERROR;
      
				int numVerticesPlusFaces = info.numVertices + info.numTriangles;				
				
				Lib3dsVector *normalL= (Lib3dsVector*) malloc(3*sizeof(Lib3dsVector)*mesh->faces);

				// Obtain current transformation matrix
				// ------------------------------------
				Lib3dsMatrix matrix;
				//Lib3dsMatrix translatedMatrix;
				Lib3dsMatrix inverseMatrix;
				
				Lib3dsObjectData *d;
				d=&node->data.object;

				// Version for lib3ds-1.2.0
				//lib3ds_matrix_copy(translatedMatrix, mesh->matrix);
				//lib3ds_matrix_copy(inverseMatrix, mesh->matrix);
				//lib3ds_matrix_inv(inverseMatrix);
				//lib3ds_matrix_translate_xyz(translatedMatrix, -d->pivot[0], -d->pivot[1], -d->pivot[2]);
				//lib3ds_matrix_mult(matrix, translatedMatrix, inverseMatrix);
				
				// Version for lib3ds-1.3.0
				lib3ds_matrix_copy(matrix, mesh->matrix);
				lib3ds_matrix_copy(inverseMatrix, mesh->matrix);
				lib3ds_matrix_inv(inverseMatrix);
				lib3ds_matrix_translate_xyz(matrix, -d->pivot[0], -d->pivot[1], -d->pivot[2]);
				lib3ds_matrix_mult(matrix, inverseMatrix);
				

				lib3ds_mesh_calculate_normals(mesh, normalL);


				// allocazione spazio per i vertici e le facce della mesh corrente
				vi = Allocator<OpenMeshType>::AddVertices(m ,mesh->points);
				fi = Allocator<OpenMeshType>::AddFaces(m ,mesh->faces);
				
				for (unsigned v=0; v<mesh->points; ++v) {
					Lib3dsVector	*p		= &mesh->pointL[v].pos;
					
					Lib3dsVector	transformedP;
					lib3ds_vector_transform( transformedP, matrix, *p);
					
					(*vi).P()[0] = transformedP[0];
					(*vi).P()[1] = transformedP[1];
					(*vi).P()[2] = transformedP[2];

					++vi;

					// callback invocation, abort loading process if the call returns false
					if (	(info.cb !=NULL) && 
								(((numFaces + numVertices + v)%100)==0) && 
								!(*info.cb)(100.0f * (float)(numFaces + numVertices + v)/(float)numVerticesPlusFaces, "Vertex Loading")	)
						return E_ABORTED;
				}				

				for (unsigned p=0; p<mesh->faces; ++p) {
					Lib3dsFace			*f		= &mesh->faceL[p];
					Lib3dsMaterial	*mat	=	0;
					if (f->material[0])
						mat = lib3ds_file_material_by_name(file, f->material);
						
					if (mat)
					{
						// considering only diffuse color component
						if( info.mask & vcg::tri::io::Mask::IOM_FACECOLOR)
						{
							// assigning face color
							// --------------------
							(*fi).C()[0] = (unsigned char) (mat->diffuse[0] * 255.0f);
							(*fi).C()[1] = (unsigned char) (mat->diffuse[1] * 255.0f);
							(*fi).C()[2] = (unsigned char) (mat->diffuse[2] * 255.0f);
							(*fi).C()[3] = (unsigned char) (mat->diffuse[3] * 255.0f);
						}

						// texture map 1
						if (mat->texture1_map.name[0])
						{
							std::string textureName = mat->texture1_map.name;
							int textureIdx = 0;
							
							// adding texture name into textures vector (if not already present)
							// avoid adding the same name twice
							bool found = false;
							unsigned size = m.textures.size();
							unsigned j = 0;
							while (!found && (j < size))
							{
								if (textureName.compare(m.textures[j])==0)
								{
									textureIdx = (int)j;
									found = true;
								}
								++j;
							}
							if (!found)
							{
								m.textures.push_back(textureName);
								textureIdx = (int)size;
							}

							if (tri::HasPerWedgeTexCoord(m) && (info.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD) )
							{
								// texture coordinates
								for (int i=0; i<3; ++i)
								{
									(*fi).WT(i).u() = mesh->texelL[f->points[i]][0];
									(*fi).WT(i).v() = mesh->texelL[f->points[i]][1];

									(*fi).WT(i).n() = textureIdx;
								}
							}
						}
					}
					else {
						// we consider only diffuse color component, using default value
						if(tri::HasPerFaceColor(m) && (info.mask & vcg::tri::io::Mask::IOM_FACECOLOR) )
						{
							// assigning default face color
							// ----------------------------
							(*fi).C()[0] = 204;
							(*fi).C()[1] = 204;
							(*fi).C()[2] = 204;
							(*fi).C()[3] = 255;
						}
					}

					if ( info.mask & vcg::tri::io::Mask::IOM_FACENORMAL )
					{
						// assigning face normal
						// ---------------------
						// we do not have to multiply normal for current matrix (as we did for vertices)
						// since translation operations do not affect normals
						(*fi).N() = f->normal;
					}
					
					for (int i=0; i<3; ++i)
					{
						if (tri::HasPerWedgeNormal(m) && ( info.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL ) )
						{
							// assigning per wedge normal
							// --------------------------
							(*fi).WN(i) = normalL[3*p+i];
						}

						// assigning face vertices
						// -----------------------
						(*fi).V(i) = &(m.vert[ (numVertices + f->points[i]) ]);
					}

					++fi;
					++numFaces;
					// callback invocation, abort loading process if the call returns false
					if (	(info.cb !=NULL) && 
								(((numFaces + numVertices + mesh->points)%100)==0) && 
								!(*info.cb)(100.0f * (float)(numFaces + numVertices + mesh->points)/(float)numVerticesPlusFaces, "Face Loading")	)
						return E_ABORTED;
				}

        free(normalL);

				numVertices += mesh->points;
      }

		}

		return result;
	}
	
	/*!
	* Retrieves kind of data stored into the file and fills a mask appropriately
	* \param filename The name of the file to open
	*	\param mask	A mask which will be filled according to type of data found in the object
	* \param oi A structure which will be filled with infos about the object to be opened
	*/
	static bool LoadMask(const char * filename, Lib3dsFile *file, _3dsInfo &info)
	{
		if (file == 0)
		{
			file = lib3ds_file_load(filename);
			if (!file)
				return false;
			lib3ds_file_eval(file,0);
		}
		
		bool bHasPerWedgeTexCoord = true;
		bool bHasPerFaceNormal		= true;
		bool bHasPerWedgeNormal		= true;
		bool bHasPerVertexColor		= false;
		bool bHasPerFaceColor			= true;
		
		info.mask = 0;
		info.numVertices	= 0;
		info.numTriangles	= 0;

		Lib3dsNode *p;
		p=file->nodes;
		for (p=file->nodes; p!=0; p=p->next) {
			LoadNodeMask(file, p, info);
		}
		
		if (bHasPerWedgeTexCoord)
			info.mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
		if (bHasPerFaceNormal)
			info.mask |= vcg::tri::io::Mask::IOM_FACENORMAL;
		if (bHasPerWedgeNormal)
			info.mask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;
		if (bHasPerVertexColor)
			info.mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
		if (bHasPerFaceColor)
			info.mask |= vcg::tri::io::Mask::IOM_FACECOLOR;
		
		/*
		mask |= vcg::tri::io::Mask::IOM_VERTCOORD;
		mask |= vcg::tri::io::Mask::IOM_VERTFLAGS;
		mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
		mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
		
		mask |= vcg::tri::io::Mask::IOM_FACEINDEX;
		mask |= vcg::tri::io::Mask::IOM_FACEFLAGS;

		mask |= vcg::tri::io::Mask::IOM_FACEQUALITY;
		mask |= vcg::tri::io::Mask::IOM_WEDGTEXMULTI;
		mask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;
		mask |= vcg::tri::io::Mask::IOM_FACECOLOR;
		*/

		return true;
	}

	static void LoadNodeMask(Lib3dsFile *file, Lib3dsNode *node, _3dsInfo &info)
	{
		{
			Lib3dsNode *p;
			for (p=node->childs; p!=0; p=p->next)
				LoadNodeMask(file, p, info);
		}

		if (node->type==LIB3DS_OBJECT_NODE)
		{
			if (strcmp(node->name,"$$$DUMMY") == 0)
				return;
		
			if (!node->user.d)
			{
				Lib3dsMesh * mesh = lib3ds_file_mesh_by_name(file, node->name);
				ASSERT(mesh);
				if (!mesh)
				  return;
				
				info.numVertices	+= mesh->points;
				info.numTriangles	+= mesh->faces;
				++info.numMeshes;
      }
		}
	}

}; // end class
} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif  // ndef __VCGLIB_IMPORT_3DS