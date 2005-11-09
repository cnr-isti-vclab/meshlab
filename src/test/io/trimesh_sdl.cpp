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
Revision 1.1  2005/11/09 11:04:56  buzzelli
Added sample project used to test OBJ importer class.

Revision 1.1  2005/09/21 10:29:33  cignoni
Initial Relase

Revision 1.1  2005/03/15 07:00:54  ganovell
*** empty log message ***


****************************************************************************/

#include "mysdl.h"

#ifdef _SHOW_A_MESH
// mesh definition
#include "mesh_type.h"
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>

//#include <wrap/io_trimesh/import_ply.h>
#include "import_obj.h"

#endif //_SHOW_A_MESH

int main(int argc, char *argv[]) {
  int level = 0;
  int apatch = -1;
  float error = 4;

  vcg::Trackball track;
#ifdef _SHOW_A_MESH
	// declare the mesh
	MyMesh mesh;
	MyMesh::VertexIterator vi;

	// load from disk
	vcg::tri::io::ImporterOBJ<MyMesh>::Open(mesh,argv[1]);
	//vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh,argv[1]);

	// update bounding box
	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);

	// update normals
	vcg::tri::UpdateNormals<MyMesh>::PerVertex(mesh);

	glWrap.m = &mesh;
#endif //_SHOW_A_MESH


   if(!init("SDL_minimal_viewer")) {
		 std::cerr << "Could not init SDL window\n";
    return -1;
  }

	glClearColor(0, 0, 0, 0); 
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
 
	sdl_idle();
	exit(0);
}



