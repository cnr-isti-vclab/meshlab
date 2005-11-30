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
 Revision 1.4  2005/11/30 00:44:07  fmazzant
 added:
 1. save TCoord2 with struct map
 2. callback
 3. define static member to the access class Exporter

 Revision 1.3  2005/11/10 00:15:15  fmazzant
 bug-fix History
 

 *****************************************************************************/

// mesh definition 
#include <vcg/simplex/vertex/with/vn.h>
#include <vcg/simplex/face/with/af.h>
#include <vcg/complex/trimesh/base.h>

#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/flag.h>

#include "import_obj.h"
#include "export_obj.h"

#include <iostream>

using namespace vcg;
using namespace std;

struct MyFace;
struct MyEdge;
struct MyVertex: public VertexVN<float,MyEdge,MyFace>{};
struct MyFace: public FaceAF<MyVertex,MyEdge,MyFace>{};
struct MyMesh: public vcg::tri::TriMesh< vector<MyVertex>, vector<MyFace> >{};

bool callback(const int pos, const char * str)
{
	if(pos<10)
		printf("\r%s [  %d%%] ", str, pos);
	else
		if(pos!=100)
			printf("\r%s [ %d%%] ", str, pos);
		else
			printf("\r%s [ OK ] \n", str, pos);
	return true;
}

int main(int argc, char **argv)
{
	if(argc < 3)
	{
	  std::cout << "Using: " << std::endl << "[prompt]-> io_debug infile.obj outfile.obj" << std::endl;
	  return-1;
	}
	
	MyMesh m;
	
	//open file OBJ
	vcg::tri::io::ObjInfo oi;
	oi.cb=&callback;
	vcg::tri::io::ImporterOBJ<MyMesh>::Open(m,argv[1],oi);

	//write copy file OBJ
	bool result = false;
	result = vcg::tri::io::ExporterOBJ<MyMesh>::Save(m,argv[2],false,oi);
    
	return 0;
}
