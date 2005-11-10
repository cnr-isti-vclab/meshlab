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

int main(int argc, char **argv)
{
	if(argc < 3){return 0;}
	
	MyMesh m;
	
	//open file OBJ 
	vcg::tri::io::ImporterOBJ<MyMesh>::OpenAscii(m,argv[1],0); 

	//write copy file OBJ
	bool result = vcg::tri::io::ExporterOBJ<MyMesh>::Save(m,argv[2],false,0);

	//print result
	if(result)
		std::cout << "file is copied!" << std::endl;
	else
		std::cout << " file is not copied!" << std::endl;

    return 0;
}
