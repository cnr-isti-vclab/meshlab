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
Revision 1.1  2005/11/08 17:45:26  buzzelli
Added first working implementation of OBJ file importer.

Revision 1.1  2005/11/07 11:00:00  buzzelli
First working version (for simplest objects)

****************************************************************************/

#ifndef __VCGLIB_IMPORT_STL
#define __VCGLIB_IMPORT_STL

#include <stdio.h>
#include <wrap/callback.h>
#include <vcg/complex/trimesh/allocate.h>

#include <fstream>
#include <string>
#include <vector>


namespace vcg {
namespace tri {
namespace io {

/** 
This class encapsulate a filter for importing obj (Alias Wavefront) meshes.
Warning: this code assume little endian (PC) architecture!!!
*/
template <class OpenMeshType>
class ImporterOBJ
{
public:

typedef typename OpenMeshType::VertexPointer VertexPointer;
typedef typename OpenMeshType::ScalarType ScalarType;
typedef typename OpenMeshType::VertexType VertexType;
typedef typename OpenMeshType::FaceType FaceType;
typedef typename OpenMeshType::VertexIterator VertexIterator;
typedef typename OpenMeshType::FaceIterator FaceIterator;


class OBJFacet
{
public:
  Point3f n;
	Point3f t;
  Point3f v[3];
//  short attr;
};

enum OBJError {
	E_NOERROR,				// 0
		// Errori di open
	E_CANTOPEN,				// 1
	E_UNESPECTEDEOF		// 2
};

static const char* ErrorMsg(int error)
{
  static const char* obj_error_msg[] =
  {
		"No errors",
		"Can't open file",
		"Premature End of file",
	};

  if(error>2 || error<0) return "Unknown error";
  else return obj_error_msg[error];
};

static int Open( OpenMeshType &m, const char * filename, CallBackPos *cb=0)
{
	// obj file should be in ascii format
  return OpenAscii(m,filename,cb);
}


static int OpenAscii( OpenMeshType &m, const char * filename, CallBackPos *cb=0)
{
	m.Clear();
	
	bool bNormalDefined		= false;
	bool bColorDefined		= false;
	bool bTexCoordDefined = false;

	std::ifstream stream(filename);
	if (stream.fail())
		return E_CANTOPEN;


	std::vector< std::string > tokens;
	std::string header;

	int numVertices = 0;  // stores the number of vertices read till now

	while (!stream.eof())  // same as !( stream.rdstate( ) & ios::eofbit )
	{
		tokens.clear();
		TokenizeNextLine(stream, tokens);
		
		if (tokens.size() > 0)
		{
			header.clear();
			header = tokens[0];

			if (header.compare("v")==0)
			{
				VertexIterator vi = Allocator<OpenMeshType>::AddVertices(m,1);
				(*vi).P()[0] = (ScalarType) atof(tokens[1].c_str());
				(*vi).P()[1] = (ScalarType) atof(tokens[2].c_str());
				(*vi).P()[2] = (ScalarType) atof(tokens[3].c_str());
				++numVertices;
			}
			else if (header.compare("f")==0)
			{
				int v1_index = atoi(tokens[1].c_str());
				int v2_index = atoi(tokens[2].c_str());
				int v3_index = atoi(tokens[3].c_str());
			
				if (v1_index < 0) v1_index += numVertices; else v1_index--;  // since index start from 1
				if (v2_index < 0) v2_index += numVertices; else v2_index--;  // instead of 0, as stored
				if (v3_index < 0) v3_index += numVertices; else v3_index--;  // int the vertices vector
			
				FaceIterator fi = Allocator<OpenMeshType>::AddFaces(m,1);
				(*fi).V(0) = &(m.vert[ v1_index ]);
				(*fi).V(1) = &(m.vert[ v2_index ]);
				(*fi).V(2) = &(m.vert[ v3_index ]);
				
				int vertexesPerFace = tokens.size() -1;
				if (vertexesPerFace == 4)  // add the other triangle
				{
					int v4_index = atoi(tokens[4].c_str());
					
					if (v4_index < 0) v4_index += numVertices; else v4_index--;
	
					fi=Allocator<OpenMeshType>::AddFaces(m,1);
					(*fi).V(0) = &(m.vert[ v1_index ]);
					(*fi).V(1) = &(m.vert[ v3_index ]);
					(*fi).V(2) = &(m.vert[ v4_index ]);
				}
				// TODO: verticesPerFace may be more than 4..
				// TODO: gestire opportunamente presenza di errori nel file
			}
			// for now, we simply ignore other situations
		}
	}


  return E_NOERROR;
} // end Open


	/*!
	* Read the next valid line and parses it into "tokens", allowing the tokens to be read one at a time.
	* \param stream	The object providing the input stream
	*	\param tokens	The "tokens" in the next line
	*/
	inline static const void TokenizeNextLine(std::ifstream &stream, std::vector< std::string > &tokens)
	{
		std::string line;
		do
			std::getline(stream, line, '\n');
		while ((line[0] == '#' || line.length()==0) && !stream.eof());  // skip comments and empty lines
		
		if ((line[0] == '#') || (line.length() == 0))  // can be true only on last line of file
			return;

		size_t from		= 0; 
		size_t to			= 0;
		size_t length = line.size();
		tokens.clear();
		do
		{
			while (line[from]==' ' && from!=length)
				from++;
      if(from!=length)
      {
				to = from+1;
				while (line[to]!=' ' && to!=length)
					to++;
				tokens.push_back(line.substr(from, to-from).c_str());
				from = to;
      }
		}
		while (from<length);
	} // end TokenizeNextLine

}; // end class
} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif
