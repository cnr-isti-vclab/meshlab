#ifndef INTERACTIVEEDIT_H_
#define INTERACTIVEEDIT_H_

/****************************************************************************
* Rgb Triangulations Plugin                                                 *
*                                                                           *
* Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
* Copyright(C) 2007                                                         *
* DISI - Department of Computer Science                                     *
* University of Genova                                                      *
*                                                                           *
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
****************************************************************************/

/****************************************************************************
 * The code of the class InteractiveEdit is based on the plugin editpaint,   *
 * developed by Gfrei Andreas.                                               *
 ****************************************************************************/


#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include <stdio.h>
#include <wrap/gl/pick.h>
#include <limits>

#include<vcg/complex/trimesh/update/topology.h>
#include<vcg/complex/trimesh/update/bounding.h>

#include "rgbPrimitives.h"
#include "topologicalOp.h"

namespace rgbt
{

/** the different painting types */
typedef enum {PEN, FILL, PICK, NONE, GRADIENT, SMOOTH, SELECT, POLY_SMOOTH} PaintThing;

/** contains some info about the current paint util */
struct Penn {
	float radius;
	int paintutensil;
	int painttype;
	bool backface;
	bool invisible;
};

struct Vert_Data {
	CVertexO * v;
	float distance;
};

struct Vert_Data_2 {
	float distance;
	Color4b color;
};

struct Vert_Data_3{
	float pos[3];
	Color4b color;
};

class InteractiveEdit
{
public:
	InteractiveEdit();
	virtual ~InteractiveEdit();

	QPoint start; // mousepos at press
	QPoint prev;
	QPoint cur; // current mousepos

	bool isDragging;
	int pressed; // to check in decorate if it is the first call after a mouse down or mouse up
	bool first; // to check in decorate if it is the first call after a mouse down
	double mvmatrix[16]; //modelview
	double projmatrix[16]; //projection
	GLint viewport[4]; //viewport
	GLfloat *pixels; // the z-buffer
	int inverse_y; // gla->curSiz.height()-cur.y() TODO probably removable
	vector<CMeshO::FacePointer> tempSel; //to use when needed
	list<int> curSel; //the faces i am painting on
	QHash<CVertexO *,Vert_Data_2> visited_vertexes; //the vertexes i am painting on
	Penn pen; //contains informations about the painting mode, color, type ...
	Qt::MouseButton curr_mouse; // which mouse button is selected
	GLArea* current_gla; // the glarea
	
	bool has_track; // to restore the trackball settings
	 
	
	void DrawXORCircle(GLArea * gla, bool doubleDraw); // draws the circle which represents the pen
	void getInternFaces(MeshModel & m,list<int> *actual,vector<Vert_Data> * risult, vector<CMeshO::FacePointer> * face_risult,
		GLArea * gla,Penn &pen,QPoint &cur, QPoint &prev, GLfloat * pixels,
		double mvmatrix[16],double projmatrix[16],GLint viewport[4]);

	
	
	inline void updateMatrixes() {		
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
	}

};


class RgbInteractiveEdit
{
public:
    /// The tetrahedral mesh type
    typedef CMeshO TriMeshType;
    /// The face type
    typedef TriMeshType::FaceType FaceType;
    /// The vertex type
    typedef FaceType::VertexType VertexType;
    /// The vertex type pointer
    typedef FaceType::VertexType* VertexPointer;
    /// The vertex iterator type
    typedef TriMeshType::VertexIterator VertexIterator;
    /// The tetra iterator type
    typedef TriMeshType::FaceIterator FaceIterator;
    /// The coordinate type
    typedef FaceType::VertexType::CoordType CoordType;
    /// The scalar type
    typedef TriMeshType::VertexType::ScalarType ScalarType;
    ///the container of tetrahedron type
    typedef TriMeshType::FaceContainer FaceContainer;
    ///the container of vertex type
    typedef TriMeshType::VertContainer VertContainer;
    ///half edge type
    typedef TriMeshType::FaceType::EdgeType EdgeType;
    /// vector of pos
    typedef std::vector<EdgeType> EdgeVec;
    /// Face Pointer
    typedef TriMeshType::FacePointer FacePointer;
    /// Edge defined by Face and Index
    typedef EdgeFI<FacePointer> EdgeFIType;
    /// Topological Operation Class
    typedef TopologicalOp<CMeshO,RgbInfo::VERTEXC,RgbInfo::FACEC > TopologicalOpC;
    /// A Point
    typedef vcg::Point3f Point;
	/// RGB Triangle
    typedef RgbTriangle<CMeshO> RgbTriangleC;
    /// RGB Vertex
    typedef RgbVertex<CMeshO> RgbVertexC;
    /// RGB Edge
    typedef RgbEdge<CMeshO> RgbEdgeC;

	RgbInteractiveEdit(CMeshO& m, RgbInfo& i, TopologicalOpC& to);
	~RgbInteractiveEdit();
	
	static bool vertexToRemove(RgbVertexC& v, int* level, double* lenght);
	static bool edgeToSplit(RgbTriangleC& t,int index, int* level, double* lenght);
	
	/// Extract the minimum level of incident edges
	static int minEdgeLevel(RgbVertexC& v);
	/// Extract the maximum level of incident edges
	static bool maxEdgeLevel(RgbVertexC& v);

	void processVertex(int v, int* level, double* lenght);
	void processEdge(int v1, int v2, int* level, double* lenght);
	
	static double maxEdge(RgbVertexC& v);
	static double edgeLenght(RgbTriangleC& t, int index);
	static bool IsValidVertex(int vp, CMeshO* m,RgbInfo* info, RgbTriangleC* t, int* ti, bool ignoreNew);
	static bool IsValidEdge(int v1,int v2, CMeshO* m,RgbInfo* info, RgbTriangleC* t, int* ti);
	static void VE(RgbVertexC& v, vector<double>& vv);
	
private:
	
	/// Topological object used
	TopologicalOpC* to;
	/// Mesh
	CMeshO* m;
	/// Rgb Information
	RgbInfo* info; 
};

}

#endif /*INTERACTIVEEDIT_H_*/
