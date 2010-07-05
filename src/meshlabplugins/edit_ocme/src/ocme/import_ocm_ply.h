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

#ifndef __VCGLIB_IMPORTER_OCME_PLY
#define __VCGLIB_IMPORTER_OCME_PLY
#include <vector>
#include<wrap/callback.h>
#include<wrap/ply/plylib.h>
#include<wrap/io_trimesh/io_mask.h>
#include<wrap/io_trimesh/io_ply.h>
#include<wrap/io_trimesh/import_ply.h>
#include<vcg/complex/trimesh/allocate.h>
#include<vcg/complex/trimesh/create/platonic.h>
#include<vcg/space/color4.h>

// oce
#include "../ooc_vector/ooc_chains.h"
#include "ocme.h"

namespace vcg {
namespace tri {
namespace io {

	
	


template <class GIDX>
void AddFace(OFace & fi, OCME * ocm, Chain<OVertex> * vert,GIDX & gPos,ScaleRange sr,std::vector<Cell*> & toCleanUp){

		static Cell * c = NULL;
		// 1: find the proper level
		float l = -1.0;
		for(int i = 0; i < 3; ++i)
			l = std::max< float>((float)l,( (*vert)[fi[i]].P() - (*vert)[fi[(i+1)%3]].P()).SquaredNorm());
	 
		int h = std::max<float>(std::min<float>(l,sr.max),sr.min); 

		// 2. find the proper cell in the level h 
		/* very first choice, pick the cell that contains the  min corner of the bounding box */
		vcg::Box3<float> facebox;
		vcg::Point3f verts[3],p,n;
		for(int i = 0; i < 3 ; ++i) {verts[i] = (*vert)[fi[i]].P(); facebox.Add(verts[i] );}
		p = (verts[0]+verts[1]+verts[2])/3.f;
		n = ((verts[1]-verts[0])^(verts[2]-verts[0])).Normalize();
		 
		CellKey ck = ComputeCellKey(facebox.min,h);

		// 3. now we know that the cell where this face must to be put is "ck". Set che vertex pointers. 
		if( (c==NULL) || !(c->key == ck))		// check if the current cell is the right one
			c = ocm->GetCellC(ck);					// if not update it


		/*  The vertices can be internal to the cell or external									*/
		/*  In the type OVertex there is bool to indicate this (OVertex::isExternal)				*/
		/*  If it is external its global index is put in the vector Cell::externalReferences		*/
		/*  and the position in Cell::externalReferences is stored as the first coordinate of vertex*/
		/*	position. Amen.																			*/

		OVertex ov;
		int vIndex[3];
		for(int i = 0; i < 3 ; ++i){
			GIndex vp  = gPos[ fi[i] ];										
			if(  vp.IsUnassigned()){												// check if it has already been assigned
				CellKey ckv = ComputeCellKey( (*vert)[fi[i]].P() ,h);					// no: find in which cell is should be stored
				Cell* ext_c = ( ckv == c->key)?c:	ocm->GetCellC(ckv);					// no: get this cell (caching: try if is the one already in c) 
				int vpos =  ext_c-> AddVertex( (*vert)[fi[i]]  );				// no: add the vertex to it
				vp = gPos[fi[i]] = GIndex(ckv,vpos);							// no: get the GIndex
			}

			// here the vertex has been assigned globally for sure
			if (vp.ck == ck)														// if it has been assigned to the the face cell
				vIndex[i] = vp.i;													// just copy its order
			else{
				int ext_pos = c->GetExternalReference(vp,false);					// check if  the reference to this vertex has already been created
				if(ext_pos!=-1)														// yes
					vIndex[i] =  c->GetVertexPointingToExternalReference(ext_pos);	//   yes: find out the corresponding vertex and assign it
				else{																// no
					ext_pos = c->AddExternalReference(vp);							//    no: add the external references						
					vIndex[i] = c->AddExternalVertex(ext_pos);						//	  no: add the corresponding vertex
                    c->ecd->vert2externals.insert(std::pair<unsigned int,unsigned int>(ext_pos,vIndex[i]));
					toCleanUp.push_back(c);
				}
			}

		//	assert(vIndex[i]<c->vert->Size());
			
		}
        // add the face to the cell
		c->AddFace(OFace(vIndex[0],vIndex[1],vIndex[2]));
       // add the sample to the impostor
		c->impostor->AddSample(p,n);

		/* Handling the bounding boxes of the cell.
		The bounding box 3 of a cell must include all the faces that cross that cell and that are
		assigned to the same level. 
		The fourth dimension of the bounding box encodes the level and it will be used at editing time.
	
		When a cell "c" must be enable for editing, we will have to load all the cells of the same level
		that intersect c. Furthermore we will have to load the upper and lower levels as written in sr (ScaleRange).

		This wil guarantee the correct connectivity. For what concerns the geometry, we can decide to load all
		the cells from the set to the root and some more  lower levels. The key idea is that you are not enabled
		to change what you do not see.
		*/

		// update the bounding box of the cell c to contain the bbox of the face	
		const Box4 old_box = c->bbox;
		c->bbox.Add(facebox,sr);
		
		// if the bounding box has enlarged then expand the bounding box of all the cells
		// touched by the bounding box to include the face
		std::vector<CellKey> overlapping;
		std::vector<CellKey>::iterator oi;

		// get the cells at the same level "h" intersected by the bounding box of the face
		ocm->OverlappingBBoxes( facebox, h,overlapping);

		Cell * ovl_cell = NULL;	// pointer to the overlapping cell
		
		for(oi = overlapping.begin(); oi != overlapping.end(); ++oi){
			/* get the overlapping cell. If there is no such overlapping cell it must be created*/
			ovl_cell = ocm->GetCellC((*oi),true);			
			ovl_cell->bbox.Add(facebox, sr);	// update its bounding box
		}
}

/** 
This class encapsulate a filter for opening ply meshes.
The ply file format is quite extensible...
*/
template <class OpenMeshType>
class ImporterOCMPLY
{
public:

typedef ::vcg::ply::PropDescriptor PropDescriptor ;
typedef typename OpenMeshType::VertexPointer VertexPointer;
typedef typename OpenMeshType::ScalarType ScalarType;
typedef typename OpenMeshType::VertexType VertexType;
typedef typename OpenMeshType::FaceType FaceType;
typedef typename OpenMeshType::VertexIterator VertexIterator;
typedef typename OpenMeshType::FaceIterator FaceIterator;

//template <class T> int PlyType () {	assert(0);  return 0;}

#define MAX_USER_DATA 256
// Struttura ausiliaria per la lettura del file ply
struct LoadPly_FaceAux
{
	unsigned char size;
	int v[512];
	int flags;
	float q;
	float texcoord[32];
	unsigned char ntexcoord;
	int texcoordind;
	float colors[32];
	unsigned char ncolors;
	
	unsigned char r;
	unsigned char g;
	unsigned char b;

	unsigned char data[MAX_USER_DATA];  
};

struct LoadPly_TristripAux
{
	int size;
	int *v;
	unsigned char data[MAX_USER_DATA];  
};


// Yet another auxiliary data structure for loading some strange ply files 
// the original stanford range data...
struct LoadPly_RangeGridAux {
      unsigned char num_pts;
      int pts[5];
    };


// Struttura ausiliaria per la lettura del file ply
template<class S>
struct LoadPly_VertAux
{
	S p[3];
	S n[3];
	int flags;
	float q;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char data[MAX_USER_DATA];  
	float radius;
	float u,v,w;
};

// Struttura ausiliaria caricamento camera
struct LoadPly_Camera
{
	float view_px;
	float view_py;
	float view_pz;
	float x_axisx;
	float x_axisy;
	float x_axisz;
	float y_axisx;
	float y_axisy;
	float y_axisz;
	float z_axisx;
	float z_axisy;
	float z_axisz;
	float focal;
	float scalex;
	float scaley;
	float centerx;
	float centery;
	int   viewportx;
	int   viewporty;
	float k1;
	float k2;
	float k3;
	float k4;
};

#define _VERTDESC_LAST_OCM  19
static const  PropDescriptor &VertDesc(int i)  
{
	static const PropDescriptor pv[_VERTDESC_LAST_OCM]={
/*00*/ {"vertex", "x",         ply::T_FLOAT, PlyType<ScalarType>(),offsetof(LoadPly_VertAux<ScalarType>,p),0,0,0,0,0  ,0},
/*01*/ {"vertex", "y",         ply::T_FLOAT, PlyType<ScalarType>(),offsetof(LoadPly_VertAux<ScalarType>,p) + sizeof(ScalarType),0,0,0,0,0  ,0},
/*02*/ {"vertex", "z",         ply::T_FLOAT, PlyType<ScalarType>(),offsetof(LoadPly_VertAux<ScalarType>,p) + 2*sizeof(ScalarType),0,0,0,0,0  ,0},
/*03*/ {"vertex", "flags",     ply::T_INT,   ply::T_INT,           offsetof(LoadPly_VertAux<ScalarType>,flags),0,0,0,0,0  ,0},
/*04*/ {"vertex", "quality",   ply::T_FLOAT, ply::T_FLOAT,         offsetof(LoadPly_VertAux<ScalarType>,q),0,0,0,0,0  ,0},
/*05*/ {"vertex", "red"  ,     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<ScalarType>,r),0,0,0,0,0  ,0},
/*06*/ {"vertex", "green",     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<ScalarType>,g),0,0,0,0,0  ,0},
/*07*/ {"vertex", "blue" ,     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<ScalarType>,b),0,0,0,0,0  ,0},
/*08*/ {"vertex", "diffuse_red"  ,     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<ScalarType>,r),0,0,0,0,0  ,0},
/*09*/ {"vertex", "diffuse_green",     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<ScalarType>,g),0,0,0,0,0  ,0},
/*10*/ {"vertex", "diffuse_blue" ,     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<ScalarType>,b),0,0,0,0,0  ,0},
/*11*/ {"vertex", "confidence",ply::T_FLOAT, ply::T_FLOAT,         offsetof(LoadPly_VertAux<ScalarType>,q),0,0,0,0,0  ,0},
/*12*/ {"vertex", "nx",         ply::T_FLOAT, PlyType<ScalarType>(),offsetof(LoadPly_VertAux<ScalarType>,n)                       ,0,0,0,0,0  ,0},
/*13*/ {"vertex", "ny",         ply::T_FLOAT, PlyType<ScalarType>(),offsetof(LoadPly_VertAux<ScalarType>,n) + 1*sizeof(ScalarType),0,0,0,0,0  ,0},
/*14*/ {"vertex", "nz",         ply::T_FLOAT, PlyType<ScalarType>(),offsetof(LoadPly_VertAux<ScalarType>,n) + 2*sizeof(ScalarType),0,0,0,0,0  ,0},
/*15*/ {"vertex", "radius",     ply::T_FLOAT, ply::T_FLOAT,         offsetof(LoadPly_VertAux<ScalarType>,radius),0,0,0,0,0  ,0},
/*16*/ {"vertex", "texture_u",    ply::T_FLOAT, ply::T_FLOAT,         offsetof(LoadPly_VertAux<ScalarType>,u),0,0,0,0,0  ,0},
/*17*/ {"vertex", "texture_v",    ply::T_FLOAT, ply::T_FLOAT,         offsetof(LoadPly_VertAux<ScalarType>,v),0,0,0,0,0  ,0},
/*18*/ {"vertex", "texture_w",    ply::T_FLOAT, ply::T_FLOAT,         offsetof(LoadPly_VertAux<ScalarType>,w),0,0,0,0,0  ,0},
	};
	return pv[i];
}

#define _FACEDESC_FIRST_  9 // the first descriptor with possible vertex indices
#define _FACEDESC_LAST_  20
static const  PropDescriptor &FaceDesc(int i)  
{		
	static const 	PropDescriptor qf[_FACEDESC_LAST_]=
	{
/*                                       on file 	     on memory                                             on file 	     on memory */
/*  0 */			{"face", "vertex_indices", ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_UCHAR,ply::T_UCHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/*  1 */			{"face", "flags",          ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,flags),     0,0,0,0,0  ,0},
/*  2 */			{"face", "quality",        ply::T_FLOAT, ply::T_FLOAT, offsetof(LoadPly_FaceAux,q),         0,0,0,0,0  ,0},
/*  3 */			{"face", "texcoord",       ply::T_FLOAT, ply::T_FLOAT, offsetof(LoadPly_FaceAux,texcoord),    1,0,ply::T_UCHAR,ply::T_UCHAR,offsetof(LoadPly_FaceAux,ntexcoord)   ,0},
/*  4 */			{"face", "color",          ply::T_FLOAT, ply::T_FLOAT, offsetof(LoadPly_FaceAux,colors),    1,0,ply::T_UCHAR,ply::T_UCHAR,offsetof(LoadPly_FaceAux,ncolors)   ,0},
/*  5 */			{"face", "texnumber",      ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,texcoordind), 0,0,0,0,0  ,0},
/*  6 */			{"face", "red"  ,          ply::T_UCHAR, ply::T_UCHAR, offsetof(LoadPly_FaceAux,r),         0,0,0,0,0  ,0},
/*  7 */			{"face", "green",          ply::T_UCHAR, ply::T_UCHAR, offsetof(LoadPly_FaceAux,g),         0,0,0,0,0  ,0},
/*  8 */			{"face", "blue" ,          ply::T_UCHAR, ply::T_UCHAR, offsetof(LoadPly_FaceAux,b),         0,0,0,0,0  ,0},
/*  9 */			{"face", "vertex_index",   ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_UCHAR,ply::T_CHAR,offsetof(LoadPly_FaceAux,size)  ,0},
/* 10 */			{"face", "vertex_index",   ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_CHAR, ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/* 11 */			{"face", "vertex_index",   ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_INT,  ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},

/* 12 */			{"face", "vertex_indices", ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_CHAR, ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/* 13 */			{"face", "vertex_indices", ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_INT,  ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/* 14 */			{"face", "vertex_indices", ply::T_UINT,  ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_UCHAR,ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/* 15 */			{"face", "vertex_indices", ply::T_UINT,  ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_CHAR, ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/* 16 */			{"face", "vertex_indices", ply::T_UINT,  ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_INT,  ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/* 17 */			{"face", "vertex_indices", ply::T_SHORT, ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_CHAR, ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/* 18 */			{"face", "vertex_indices", ply::T_SHORT, ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_UCHAR,ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
/* 19 */			{"face", "vertex_indices", ply::T_SHORT, ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_INT,  ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0}
	};
	return qf[i];
}
static const  PropDescriptor &TristripDesc(int i)  
{		
	static const 	PropDescriptor qf[1]=
	{
		{"tristrips","vertex_indices", ply::T_INT,  ply::T_INT,  offsetof(LoadPly_TristripAux,v),		  1,1,ply::T_INT,ply::T_INT,offsetof(LoadPly_TristripAux,size) ,0},				
	};
	return qf[i];
}

// Descriptor for the Stanford Data Repository Range Maps.
// In practice a grid with some invalid elements. Coords are saved only for good elements
static const  PropDescriptor &RangeDesc(int i)  
{		
	static const PropDescriptor range_props[1] = {
      {"range_grid","vertex_indices", ply::T_INT, ply::T_INT, offsetof(LoadPly_RangeGridAux,pts), 1, 0, ply::T_UCHAR, ply::T_UCHAR, offsetof(LoadPly_RangeGridAux,num_pts),0},
    };
	return range_props[i];
}


static const  PropDescriptor &CameraDesc(int i)  
{		
	static const PropDescriptor cad[23] =
	{
		{"camera","view_px",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,view_px),0,0,0,0,0  ,0},
		{"camera","view_py",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,view_py),0,0,0,0,0  ,0},
		{"camera","view_pz",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,view_pz),0,0,0,0,0  ,0},
		{"camera","x_axisx",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,x_axisx),0,0,0,0,0  ,0},
		{"camera","x_axisy",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,x_axisy),0,0,0,0,0  ,0},
		{"camera","x_axisz",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,x_axisz),0,0,0,0,0  ,0},
		{"camera","y_axisx",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,y_axisx),0,0,0,0,0  ,0},
		{"camera","y_axisy",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,y_axisy),0,0,0,0,0  ,0},
		{"camera","y_axisz",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,y_axisz),0,0,0,0,0  ,0},
		{"camera","z_axisx",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,z_axisx),0,0,0,0,0  ,0},
		{"camera","z_axisy",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,z_axisy),0,0,0,0,0  ,0},
		{"camera","z_axisz",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,z_axisz),0,0,0,0,0  ,0},
		{"camera","focal"  ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,focal  ),0,0,0,0,0  ,0},
		{"camera","scalex" ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,scalex ),0,0,0,0,0  ,0},
		{"camera","scaley" ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,scaley ),0,0,0,0,0  ,0},
		{"camera","centerx",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,centerx),0,0,0,0,0  ,0},
		{"camera","centery",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,centery),0,0,0,0,0  ,0},
		{"camera","viewportx",ply::T_INT,ply::T_INT  ,offsetof(LoadPly_Camera,viewportx),0,0,0,0,0  ,0},
		{"camera","viewporty",ply::T_INT,ply::T_INT  ,offsetof(LoadPly_Camera,viewporty),0,0,0,0,0  ,0},
		{"camera","k1"     ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,k1 ),0,0,0,0,0  ,0},
		{"camera","k2"     ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,k2 ),0,0,0,0,0  ,0},
		{"camera","k3"     ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,k3 ),0,0,0,0,0  ,0},
		{"camera","k4"     ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,k4 ),0,0,0,0,0  ,0}
	};
	return cad[i];
}
/// Standard call for knowing the meaning of an error code
static const char *ErrorMsg(int error)
{
  static std::vector<std::string> ply_error_msg;
  if(ply_error_msg.empty())
  {
    ply_error_msg.resize(PlyInfo::E_MAXPLYINFOERRORS );
    ply_error_msg[ply::E_NOERROR				]="No errors";
	  ply_error_msg[ply::E_CANTOPEN				]="Can't open file";
    ply_error_msg[ply::E_NOTHEADER ]="Header not found";
	  ply_error_msg[ply::E_UNESPECTEDEOF	]="Eof in header";
	  ply_error_msg[ply::E_NOFORMAT				]="Format not found";
	  ply_error_msg[ply::E_SYNTAX				]="Syntax error on header";
	  ply_error_msg[ply::E_PROPOUTOFELEMENT]="Property without element";
	  ply_error_msg[ply::E_BADTYPENAME		]="Bad type name";
	  ply_error_msg[ply::E_ELEMNOTFOUND		]="Element not found";
	  ply_error_msg[ply::E_PROPNOTFOUND		]="Property not found";
	  ply_error_msg[ply::E_BADTYPE				]="Bad type on addtoread";
	  ply_error_msg[ply::E_INCOMPATIBLETYPE]="Incompatible type";
	  ply_error_msg[ply::E_BADCAST				]="Bad cast";

    ply_error_msg[PlyInfo::E_NO_VERTEX      ]="No vertex field found";
    ply_error_msg[PlyInfo::E_NO_FACE        ]="No face field found";
	  ply_error_msg[PlyInfo::E_SHORTFILE      ]="Unespected eof";
	  ply_error_msg[PlyInfo::E_NO_3VERTINFACE ]="Face with more than 3 vertices";
	  ply_error_msg[PlyInfo::E_BAD_VERT_INDEX ]="Bad vertex index in face";
	  ply_error_msg[PlyInfo::E_NO_6TCOORD     ]="Face with no 6 texture coordinates";
	  ply_error_msg[PlyInfo::E_DIFFER_COLORS  ]="Number of color differ from vertices";
  }

  if(error>PlyInfo::E_MAXPLYINFOERRORS || error<0) return "Unknown error";
  else return ply_error_msg[error].c_str();
};

// to check if a given error is critical or not.
static bool ErrorCritical(int err)
{ 
	if(err == PlyInfo::E_NO_FACE) return false;
	return true;
}
	

/// Standard call for reading a mesh
static int Open( OpenMeshType &m, OCME * ocm,const char * filename, vcg::Matrix44f tra_ma, CallBackPos *cb=0)
{
	PlyInfo pi;
  pi.cb=cb; 
  return Open(m, ocm, filename,tra_ma, pi);
}

/// Read a mesh and store in loadmask the loaded field
/// Note that loadmask is not read! just modified. You cannot specify what fields
/// have to be read. ALL the data for which your mesh HasSomething and are present 
/// in the file are read in. 
static int Open( OpenMeshType &m, OCME * ocm, const char * filename, vcg::Matrix44f tra_ma, int & loadmask, CallBackPos *cb =0)
{
  PlyInfo pi;
  pi.cb=cb; 
  int r =  Open(m,ocm, filename,tra_ma,pi);
  loadmask=pi.mask;
  return r;
}




/// read a mesh with all the possible option specified in the PlyInfo obj.
static int Open( OpenMeshType &m, OCME * ocm, const char * filename, vcg::Matrix44f tra_ma, PlyInfo &pi )
{
  assert(filename!=0);
	std::vector<VertexPointer> index;
	LoadPly_FaceAux fa;
	LoadPly_TristripAux tsa;
	LoadPly_VertAux<ScalarType> va;
  
	LoadPly_RangeGridAux rga;
	std::vector<int> RangeGridAuxVec;
	int RangeGridCols=0;
	int RangeGridRows=0;
	
	/* OOCEnv stuff
	*/
	Chain<OVertex>  * vert;
	Chain<GIndex>  * gPos;
	OOCEnv  oce;
	unsigned long n_faces = 0; 

	pi.mask = 0;
	bool multit = false; // true if texture has a per face int spec the texture index

	va.flags = 42;

  pi.status = ::vcg::ply::E_NOERROR;

	/*
	// TO BE REMOVED: tv not used AND "spurious" vertex declaration causes error if ocf

	// init defaults
	VertexType tv;
	//tv.ClearFlags();

	if (vcg::tri::HasPerVertexQuality(m)) tv.Q() = (typename OpenMeshType::VertexType::QualityType)1.0;
	if (vcg::tri::HasPerVertexColor  (m)) tv.C() = Color4b(Color4b::White);
	*/

	// Descrittori delle strutture

  //bool isvflags = false;	// Il file contiene i flags 


	// The main descriptor of the ply file 
	vcg::ply::PlyFile pf;
  
  // Open the file and parse the header
  if( pf.Open(filename,vcg::ply::PlyFile::MODE_READ)==-1 )
	{
		pi.status = pf.GetError();
		return pi.status;
	}
  pi.header = pf.GetHeader();

	// Descrittori della camera
	{  // Check that all the camera properties are present.
		bool found = true;
		for(int i=0;i<23;++i)
		{
			if( pf.AddToRead(CameraDesc(i))==-1 ) {
				found = false;
				break;
			}
		}
		if(found) pi.mask |= Mask::IOM_CAMERA;
	}

  // Descrittori dati standard (vertex coord e faces)
  if( pf.AddToRead(VertDesc(0))==-1 ) { pi.status = PlyInfo::E_NO_VERTEX; return pi.status; }
	if( pf.AddToRead(VertDesc(1))==-1 ) { pi.status = PlyInfo::E_NO_VERTEX; return pi.status; }
	if( pf.AddToRead(VertDesc(2))==-1 ) { pi.status = PlyInfo::E_NO_VERTEX; return pi.status; }
	if( pf.AddToRead(FaceDesc(0))==-1 ) // Se fallisce si prova anche la sintassi di rapidform con index al posto di indices
			{
				int ii; 
				for(ii=_FACEDESC_FIRST_;ii< _FACEDESC_LAST_;++ii)
					if( pf.AddToRead(FaceDesc(ii))!=-1 ) break;
					
			  if(ii==_FACEDESC_LAST_) 
					if(pf.AddToRead(TristripDesc(0))==-1) // Se fallisce tutto si prova a vedere se ci sono tristrip alla levoy.
						if(pf.AddToRead(RangeDesc(0))==-1) // Se fallisce tutto si prova a vedere se ci sono rangemap alla levoy.
									{ 
										pi.status = PlyInfo::E_NO_FACE;   
										//return pi.status;  no face is not a critical error. let's continue.
									}
									
			}
		// Descrittori facoltativi dei flags
	
  if(VertexType::HasFlags() && pf.AddToRead(VertDesc(3))!=-1 ) 
			pi.mask |= Mask::IOM_VERTFLAGS;

	 if( VertexType::HasNormal() )
	 {
		 if(		pf.AddToRead(VertDesc(12))!=-1 
				 && pf.AddToRead(VertDesc(13))!=-1 
				 && pf.AddToRead(VertDesc(14))!=-1 )
			 pi.mask |= Mask::IOM_VERTNORMAL;
	 }
	 
  if( VertexType::HasQuality() )
	{
		if( pf.AddToRead(VertDesc(4))!=-1 ||
		    pf.AddToRead(VertDesc(11))!=-1 )
			pi.mask |= Mask::IOM_VERTQUALITY;
	}

	if( VertexType::HasColor() )
	{
		if( pf.AddToRead(VertDesc(5))!=-1 )
		{
			pf.AddToRead(VertDesc(6));
			pf.AddToRead(VertDesc(7));
			pi.mask |= Mask::IOM_VERTCOLOR;
		}
		if( pf.AddToRead(VertDesc(8))!=-1 )
		{
			pf.AddToRead(VertDesc(9));
			pf.AddToRead(VertDesc(10));
			pi.mask |= Mask::IOM_VERTCOLOR;
		}
	}

    if(tri::HasPerVertexRadius(m))
	{
		if( pf.AddToRead(VertDesc(15))!=-1 )
				pi.mask |= Mask::IOM_VERTRADIUS;
	}
			// se ci sono i flag per vertice ci devono essere anche i flag per faccia
	if( pf.AddToRead(FaceDesc(1))!=-1 )
		pi.mask |= Mask::IOM_FACEFLAGS;

  if( FaceType::HasFaceQuality())
	{
		if( pf.AddToRead(FaceDesc(2))!=-1 )
			pi.mask |= Mask::IOM_FACEQUALITY;
	}

	if( FaceType::HasFaceColor() )
	{
		if( pf.AddToRead(FaceDesc(6))!=-1 )
		{
			pf.AddToRead(FaceDesc(7));
			pf.AddToRead(FaceDesc(8));
			pi.mask |= Mask::IOM_FACECOLOR;
		}
	}


	if( FaceType::HasWedgeTexCoord() )
	{
		if( pf.AddToRead(FaceDesc(3))!=-1 )
		{
			if(pf.AddToRead(FaceDesc(5))==0) {
				multit=true; // try to read also the multi texture indicies
				pi.mask |= Mask::IOM_WEDGTEXMULTI;
			}
			pi.mask |= Mask::IOM_WEDGTEXCOORD;
		}
	}

  if( FaceType::HasWedgeColor() || FaceType::HasFaceColor() || VertexType::HasColor())
	{
		if( pf.AddToRead(FaceDesc(4))!=-1 )
		{
			pi.mask |= Mask::IOM_WEDGCOLOR;
		}
	}

	// Descrittori definiti dall'utente, 
	std::vector<PropDescriptor> VPV(pi.vdn); // property descriptor relative al tipo LoadPly_VertexAux
  std::vector<PropDescriptor> FPV(pi.fdn); // property descriptor relative al tipo LoadPly_FaceAux
	if(pi.vdn>0){
	// Compute the total size needed to load additional per vertex data.
		size_t totsz=0;
		for(int i=0;i<pi.vdn;i++){
			VPV[i] = pi.VertexData[i];
			VPV[i].offset1=offsetof(LoadPly_VertAux<ScalarType>,data)+totsz;
			totsz+=pi.VertexData[i].memtypesize();
			if( pf.AddToRead(VPV[i])==-1 ) { pi.status = pf.GetError(); return pi.status; }
		}
		if(totsz > MAX_USER_DATA) 
		{ 
			pi.status = vcg::ply::E_BADTYPE; 
			return pi.status; 
		}
	}
	if(pi.fdn>0){
		size_t totsz=0;
		for(int i=0;i<pi.fdn;i++){
			FPV[i] = pi.FaceData[i];
			FPV[i].offset1=offsetof(LoadPly_FaceAux,data)+totsz;
			totsz+=pi.FaceData[i].memtypesize();
			if( pf.AddToRead(FPV[i])==-1 ) { pi.status = pf.GetError(); return pi.status; }
		}
		if(totsz > MAX_USER_DATA) 
		{ 
      pi.status = vcg::ply::E_BADTYPE; 
			return pi.status; 
		}
	}

  /**************************************************************/
  /* Main Reading Loop */
  /**************************************************************/
  m.Clear();
  for(int i=0;i<int(pf.elements.size());i++)
	{
		int n = pf.ElemNumber(i);

		if( !strcmp( pf.ElemName(i),"camera" ) )
		{
			pf.SetCurElement(i);

			LoadPly_Camera ca;

			for(int j=0;j<n;++j)
			{
				if( pf.Read( (void *)&(ca) )==-1 )
				{
					pi.status = PlyInfo::E_SHORTFILE;
					return pi.status;
				}	
				//camera.valid     = true;

				// extrinsic
				m.shot.Extrinsics.SetIdentity();
				// view point
				m.shot.Extrinsics.SetTra(Point3<ScalarType>( ca.view_px,ca.view_py,ca.view_pz));
	
				// axis (i.e. rotation).
				Matrix44<ScalarType> rm;
				rm[0][0] = ca.x_axisx;
				rm[0][1] = ca.x_axisy;
				rm[0][2] = ca.x_axisz;

				rm[1][0] = ca.y_axisx;
				rm[1][1] = ca.y_axisy;
				rm[1][2] = ca.y_axisz;

				rm[2][0] = ca.z_axisx;
				rm[2][1] = ca.z_axisy;
				rm[2][2] = ca.z_axisz;

				m.shot.Extrinsics.SetRot(rm);

				//intrinsic
				m.shot.Intrinsics.FocalMm        = ca.focal;
				m.shot.Intrinsics.PixelSizeMm[0] = ca.scalex;
				m.shot.Intrinsics.PixelSizeMm[1] = ca.scaley;
				m.shot.Intrinsics.CenterPx[0]    = ca.centerx;
				m.shot.Intrinsics.CenterPx[1]    = ca.centery;
				m.shot.Intrinsics.ViewportPx[0]  = ca.viewportx;
				m.shot.Intrinsics.ViewportPx[1]  = ca.viewporty;
				m.shot.Intrinsics.k[0]           = ca.k1;
				m.shot.Intrinsics.k[1]           = ca.k2;
				m.shot.Intrinsics.k[2]           = ca.k3;
				m.shot.Intrinsics.k[3]           = ca.k4;

			}
		}
		else if( !strcmp( pf.ElemName(i),"vertex" ) )
		{
			int j;

			pf.SetCurElement(i);

			/*
			VertexIterator vi=Allocator<OpenMeshType>::AddVertices(m,n);
			*/

			/*
			OOCEnv stuff
			*/
		
			oce.cache_policy->memory_limit = 20*(1<<20);
                        oce.params.blockSizeBytes = 2048;
			remove("buffer");
			oce.Create("buffer");
			vert = oce.GetChain<OVertex>(std::string("vert"),true);
			vert->Resize(n);
			gPos = oce.GetChain<GIndex>(std::string("GIndex"),true);
			gPos->Resize(n);
			
			ocm->stat.n_vertices +=n;
      for(j=0;j<n;++j)
			{
				if(pi.cb && (j%1000)==0) pi.cb(j*50/n,"Vertex Loading");
				//(*vi).UberFlags()=0; // No more necessary, since 9/2005 flags are set to zero in the constuctor.
			  if( pf.Read( (void *)&(va) )==-1 )
				{
					pi.status = PlyInfo::E_SHORTFILE;
					return pi.status;
				}
			  
			  (*vert)[j].P()  = tra_ma* vcg::Point3f(va.p[0],va.p[1],va.p[2]) ;
	//			  (*vert)[j].P()[1] = tra_ma* va.p[1];
	//			  (*vert)[j].P()[2] = tra_ma* va.p[2];
/*				
				(*vi).P()[0] = va.p[0];
				(*vi).P()[1] = va.p[1];
				(*vi).P()[2] = va.p[2];

				if( m.HasPerVertexFlags() &&  (pi.mask & Mask::IOM_VERTFLAGS) )
					(*vi).UberFlags() = va.flags;

				if( pi.mask & Mask::IOM_VERTQUALITY )
					(*vi).Q() = (typename OpenMeshType::VertexType::QualityType)va.q;

				if( pi.mask & Mask::IOM_VERTNORMAL )
					{
						(*vi).N()[0]=va.n[0];
						(*vi).N()[1]=va.n[1];
						(*vi).N()[2]=va.n[2];
					}
				
				if( pi.mask & Mask::IOM_VERTCOLOR )
				{
					(*vi).C()[0] = va.r;
					(*vi).C()[1] = va.g;
					(*vi).C()[2] = va.b;
					(*vi).C()[3] = 255;
				}
				if( pi.mask & Mask::IOM_VERTRADIUS )
						(*vi).R() = va.radius; 

				
				for(int k=0;k<pi.vdn;k++)	
					memcpy((char *)(&*vi) + pi.VertexData[k].offset1,
									(char *)(&va) + VPV[k].offset1, 
									VPV[k].memtypesize());
			++vi;
*/
      }

			//index.resize(n);
			//for(j=0,vi=m.vert.begin();j<n;++j,++vi)
			//	index[j] = &*vi;
		}
		else if( !strcmp( pf.ElemName(i),"face") && (n>0) )/************************************************************/
		{
			int j;
			n_faces = n;
			ocm->stat.n_triangles +=n;

			/*
		      FaceIterator fi=Allocator<OpenMeshType>::AddFaces(m,n);
			*/
			std::vector<vcg::Point3< vcg::Point3f > > tris;
			std::vector<int> indices;

			ScaleRange sr;
			std::vector<Cell*> toCleanUp;
			typename OpenMeshType::FaceType   f;

			pf.SetCurElement(i);

			for(j=0;j<std::min(1000,n);++j)
			{
				int k;

				if(pi.cb && (j%1000)==0) pi.cb(50+j*50/n,"Face Loading");
				if( pf.Read(&fa)==-1 )
				{
					pi.status = PlyInfo::E_SHORTFILE;
					return pi.status;
				}
 
				tris.push_back(vcg::Point3<vcg::Point3f> () );
				for(k=0;k<3;++k)
				{
					if( fa.v[k]<0 || fa.v[k]>=vert->Size() )
					{
						pi.status = PlyInfo::E_BAD_VERT_INDEX;
						return pi.status;
					}
					for(unsigned int cc = 0 ; cc < 3; ++cc)
					{
						tris.back()[k][cc] = (*vert)[fa.v[k]].P()[cc];  
					}
					indices.push_back(fa.v[k]);
				}
			}
			// here we have tris with the first 1000 triangles
			sr = ocm->ScaleRangeOfTris(tris);
			sr.min = sr.max ;

			for(j=0;j<std::min(1000,n);++j) {
				OFace oface(indices[j*3],indices[j*3+1],indices[j*3+2]);
				AddFace(oface, ocm,vert,*gPos , sr, toCleanUp);
			}

			for( j =  std::min(1000,n); j < n;++j){
				
				int k;

				if(pi.cb && (j%1000)==0) pi.cb(50+j*50/n,"Face Loading");
				if( pf.Read(&fa)==-1 )
				{
					pi.status = PlyInfo::E_SHORTFILE;
					return pi.status;
				}
 
				for(k=0;k<3;++k)
				{
					if( fa.v[k]<0 || fa.v[k]>=vert->Size() )
					{
						pi.status = PlyInfo::E_BAD_VERT_INDEX;
						return pi.status;
					}
				}
				OFace oface(fa.v[0],fa.v[1],fa.v[2]);
				AddFace(oface,ocm,vert, *gPos , sr, toCleanUp);
			}

			RemoveDuplicates(toCleanUp);
			for(std::vector<Cell*>::iterator i=toCleanUp.begin();i != toCleanUp.end();++i)
                                (*i)->ecd->vert2externals.clear();

			oce.TrashEverything( );
}
else if( !strcmp( pf.ElemName(i),"tristrips") )//////////////////// LETTURA TRISTRIP DI STANFORD
		{ 
			// Warning the parsing of tristrips could not work if OCF types are used
			FaceType tf;  
			if( HasPerFaceQuality(m) )  tf.Q()=(typename OpenMeshType::FaceType::QualityType)1.0;
			if( FaceType::HasWedgeColor() )   tf.WC(0)=tf.WC(1)=tf.WC(2)=Color4b(Color4b::White);
			if( HasPerFaceColor(m) )    tf.C()=Color4b(Color4b::White);			

			int j;
			pf.SetCurElement(i);
			int numvert_tmp = (int)m.vert.size();
			for(j=0;j<n;++j)
			{
				int k;
				if(pi.cb && (j%1000)==0) pi.cb(50+j*50/n,"Tristrip Face Loading");
				if( pf.Read(&tsa)==-1 )
				{
					pi.status = PlyInfo::E_SHORTFILE;
					return pi.status;
				}
				int remainder=0;
				//int startface=m.face.size();
				for(k=0;k<tsa.size-2;++k)
				{
					if(pi.cb && (k%1000)==0) pi.cb(50+k*50/tsa.size,"Tristrip Face Loading");				
          if(tsa.v[k]<0 || tsa.v[k]>=numvert_tmp )	{
						pi.status = PlyInfo::E_BAD_VERT_INDEX;
						return pi.status;
					}
				  if(tsa.v[k+2]==-1)
					{
						k+=2;
						if(k%2) remainder=0;
							 else remainder=1;
						continue;
					}
					tf.V(0) = index[ tsa.v[k+0] ];
					tf.V(1) = index[ tsa.v[k+1] ];
					tf.V(2) = index[ tsa.v[k+2] ];
					if((k+remainder)%2) math::Swap (tf.V(0), tf.V(1) );
					m.face.push_back( tf );
				}
			}
		}
		else if( !strcmp( pf.ElemName(i),"range_grid") )//////////////////// LETTURA RANGEMAP DI STANFORD
		{
			//qDebug("Starting Reading of Range Grid");
			if(RangeGridCols==0) // not initialized. 
			{
				for(int co=0;co<int(pf.comments.size());++co)
				{
					std::string num_cols = "num_cols";
					std::string num_rows = "num_rows";
					std::string &c = pf.comments[co];
					std::string bufstr,bufclean;
					if( num_cols == c.substr(0,num_cols.length()) ) 
					{
						bufstr = c.substr(num_cols.length()+1);
						RangeGridCols = atoi(bufstr.c_str());
					}
					if( num_rows == c.substr(0,num_cols.length()) ) 
					{
						bufstr = c.substr(num_rows.length()+1);
						RangeGridRows = atoi(bufstr.c_str());
					}
				}
				//qDebug("Rows %i Cols %i",RangeGridRows,RangeGridCols);
			}
			int totPnt = RangeGridCols*RangeGridRows;
			// standard reading;
			pf.SetCurElement(i);
			for(int j=0;j<totPnt;++j)
			{
				if(pi.cb && (j%1000)==0) pi.cb(50+j*50/totPnt,"RangeMap Face Loading");
				if( pf.Read(&rga)==-1 )
				{
					//qDebug("Error after loading %i elements",j);
					pi.status = PlyInfo::E_SHORTFILE;
					return pi.status;
				}
				else 
				{
					if(rga.num_pts == 0)
						RangeGridAuxVec.push_back(-1);
					else 
						RangeGridAuxVec.push_back(rga.pts[0]);
				}
			} 
			//qDebug("Completed the reading of %i indexes",RangeGridAuxVec.size());
			tri::FaceGrid(m, RangeGridAuxVec, RangeGridCols,RangeGridRows);
		}
		else
		{
				// Skippaggio elementi non gestiti
			int n = pf.ElemNumber(i);
			pf.SetCurElement(i);

			for(int j=0;j<n;j++)
			{
				if( pf.Read(0)==-1)
				{
					pi.status = PlyInfo::E_SHORTFILE;
					return pi.status;
				}
			}
		}
	}

  // Parsing texture names
	m.textures.clear();
	m.normalmaps.clear();

	for(int co=0;co<int(pf.comments.size());++co)
	{
    std::string TFILE = "TextureFile";
		std::string NFILE = "TextureNormalFile";
		std::string &c = pf.comments[co];
//		char buf[256];
    std::string bufstr,bufclean;
		int i,n;

    if( TFILE == c.substr(0,TFILE.length()) ) 
		{
      bufstr = c.substr(TFILE.length()+1);
			n = static_cast<int>(bufstr.length());
			for(i=0;i<n;i++)
				if( bufstr[i]!=' ' && bufstr[i]!='\t' && bufstr[i]>32 && bufstr[i]<125 )	bufclean.push_back(bufstr[i]);
			
			char buf2[255];
      ply::interpret_texture_name( bufclean.c_str(),filename,buf2 );
			m.textures.push_back( std::string(buf2) );
		}
		/*if( !strncmp(c,NFILE,strlen(NFILE)) )
		{
			strcpy(buf,c+strlen(NFILE)+1);
			n = strlen(buf);
			for(i=j=0;i<n;i++)
				if( buf[i]!=' ' && buf[i]!='\t' && buf[i]>32 && buf[i]<125 )	buf[j++] = buf[i];
			
			buf[j] = 0;
			char buf2[255];
			__interpret_texture_name( buf,filename,buf2 );
			m.normalmaps.push_back( string(buf2) );
		}*/
	}

  // vn and fn should be correct but if someone wrongly saved some deleted elements they can be wrong.
	m.vn = 0;
	VertexIterator vi;
	for(vi=m.vert.begin();vi!=m.vert.end();++vi)
		if( ! (*vi).IsD() )
			++m.vn;

	m.fn = 0;
	FaceIterator fi;
	for(fi=m.face.begin();fi!=m.face.end();++fi)
		if( ! (*fi).IsD() )
			++m.fn;

	return n_faces;
}


	// Caricamento camera da un ply
int LoadCamera(const char * filename)
{
	vcg::ply::PlyFile pf;
	if( pf.Open(filename,vcg::ply::PlyFile::MODE_READ)==-1 )
	{
		this->pi.status = pf.GetError();
		return this->pi.status;
	}


	bool found = true;
	int i;
	for(i=0;i<23;++i)
	{
		if( pf.AddToRead(CameraDesc(i))==-1 )
		{
			found = false;
			break;
		}
	}

	if(!found)
		return this->pi.status;

	for(i=0;i<int(pf.elements.size());i++)
	{
		int n = pf.ElemNumber(i);

		if( !strcmp( pf.ElemName(i),"camera" ) )
		{
			pf.SetCurElement(i);

			LoadPly_Camera ca;

			for(int j=0;j<n;++j)
			{
				if( pf.Read( (void *)&(ca) )==-1 )
				{
					this->pi.status = PlyInfo::E_SHORTFILE;
					return this->pi.status;
				}	
				this->camera.valid     = true;
				this->camera.view_p[0] = ca.view_px;
				this->camera.view_p[1] = ca.view_py;
				this->camera.view_p[2] = ca.view_pz;
				this->camera.x_axis[0] = ca.x_axisx;
				this->camera.x_axis[1] = ca.x_axisy;
				this->camera.x_axis[2] = ca.x_axisz;
				this->camera.y_axis[0] = ca.y_axisx;
				this->camera.y_axis[1] = ca.y_axisy;
				this->camera.y_axis[2] = ca.y_axisz;
				this->camera.z_axis[0] = ca.z_axisx;
				this->camera.z_axis[1] = ca.z_axisy;
				this->camera.z_axis[2] = ca.z_axisz;
				this->camera.f         = ca.focal;
				this->camera.s[0]      = ca.scalex;
				this->camera.s[1]      = ca.scaley;
				this->camera.c[0]      = ca.centerx;
				this->camera.c[1]      = ca.centery;
				this->camera.viewport[0] = ca.viewportx;
				this->camera.viewport[1] = ca.viewporty;
				this->camera.k[0]      = ca.k1;
				this->camera.k[1]      = ca.k2;
				this->camera.k[2]      = ca.k3;
				this->camera.k[3]      = ca.k4;
			}
			break;
		}
	}

	return 0;
}


static bool LoadMask(const char * filename, int &mask)
{
  PlyInfo pi;
  return LoadMask(filename, mask,pi);
}
static bool LoadMask(const char * filename, int &mask, PlyInfo &pi)
{
	mask=0;
	vcg::ply::PlyFile pf;
	if( pf.Open(filename,vcg::ply::PlyFile::MODE_READ)==-1 )
	{
		pi.status = pf.GetError();
		return false;
	}

	if( pf.AddToRead(VertDesc(0))!=-1 && 
	    pf.AddToRead(VertDesc(1))!=-1 && 
	    pf.AddToRead(VertDesc(2))!=-1 )   mask |= Mask::IOM_VERTCOORD;

	if( pf.AddToRead(VertDesc(12))!=-1 && 
	    pf.AddToRead(VertDesc(13))!=-1 && 
	    pf.AddToRead(VertDesc(14))!=-1 )   mask |= Mask::IOM_VERTNORMAL;

	if( pf.AddToRead(VertDesc(3))!=-1 )		mask |= Mask::IOM_VERTFLAGS;
	if( pf.AddToRead(VertDesc(4))!=-1 )		mask |= Mask::IOM_VERTQUALITY;
	if( pf.AddToRead(VertDesc(11))!=-1 )	mask |= Mask::IOM_VERTQUALITY;
	if( pf.AddToRead(VertDesc(15))!=-1 )	mask |= Mask::IOM_VERTRADIUS;
	if( ( pf.AddToRead(VertDesc(5))!=-1 ) && 
		  ( pf.AddToRead(VertDesc(6))!=-1 ) &&
			( pf.AddToRead(VertDesc(7))!=-1 )  )  mask |= Mask::IOM_VERTCOLOR;
	
	if( pf.AddToRead(FaceDesc(0))!=-1 ) mask |= Mask::IOM_FACEINDEX;
	if( pf.AddToRead(FaceDesc(1))!=-1 ) mask |= Mask::IOM_FACEFLAGS;

	if( pf.AddToRead(FaceDesc(2))!=-1 ) mask |= Mask::IOM_FACEQUALITY;
	if( pf.AddToRead(FaceDesc(3))!=-1 ) mask |= Mask::IOM_WEDGTEXCOORD;
	if( pf.AddToRead(FaceDesc(5))!=-1 ) mask |= Mask::IOM_WEDGTEXMULTI;
	if( pf.AddToRead(FaceDesc(4))!=-1 ) mask |= Mask::IOM_WEDGCOLOR;
	if( ( pf.AddToRead(FaceDesc(6))!=-1 ) && 
		  ( pf.AddToRead(FaceDesc(7))!=-1 ) &&
			( pf.AddToRead(FaceDesc(8))!=-1 )  )  mask |= Mask::IOM_FACECOLOR;


 return true;
}


}; // end class



} // end namespace tri
} // end namespace io
} // end namespace vcg

#endif

