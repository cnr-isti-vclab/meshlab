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
Revision 1.19  2006/01/13 00:40:26  buzzelli
added support for files with negative references to texCoord and vertex normals inside face definition

Revision 1.18  2006/01/12 23:46:04  buzzelli
solved a small bug in material loading

Revision 1.17  2006/01/11 17:19:03  buzzelli
code cleaning

Revision 1.16  2006/01/10 16:52:20  fmazzant
update ply::PlyMask -> io::Mask

Revision 1.15  2005/12/23 02:31:28  buzzelli
mask is filled also with infos about used colouring method (per vertex, per face)

Revision 1.14  2005/12/22 23:37:26  buzzelli
storing per wedge normals into model when opened file provides them

Revision 1.13  2005/12/22 22:10:18  buzzelli
using face::ComputeNormalizedNormal to compute face normal when no per wedge normal is provided

Revision 1.12  2005/12/22 02:23:11  buzzelli
added face normals computation

Revision 1.11  2005/12/21 23:23:33  buzzelli
code cleaning

Revision 1.10  2005/12/21 00:42:32  buzzelli
Better handling of errors inside opened file

Revision 1.9  2005/12/12 17:10:13  buzzelli
face loading process speeded up

Revision 1.8  2005/12/08 15:48:51  buzzelli
Added correct calculation of texture indexes

Revision 1.7  2005/12/08 02:28:36  buzzelli
solved a bug into LoadMask function, since now texture loading begins to work properly

Revision 1.6  2005/12/07 17:42:38  buzzelli
Progress bar counter unified for both vertices and faces

Revision 1.5  2005/12/06 05:07:39  buzzelli
Object file importer now performs also materials and texture names loading

Revision 1.4  2005/11/27 16:46:13  buzzelli
Added test of callback with null

Revision 1.3  2005/11/24 01:43:25  cignoni
Ho committato la versione come si era sistemata a lezione.
Se non va bene sovrascrivetela pure...

Revision 1.2  2005/11/09 11:01:10  buzzelli
OBJ file importing mechanism extended for faces with more than four vertices.

Revision 1.1  2005/11/08 17:45:26  buzzelli
Added first working implementation of OBJ file importer.

Revision 1.1  2005/11/07 11:00:00  buzzelli
First working version (for simplest objects)

****************************************************************************/

#ifndef __VCGLIB_IMPORT_OBJ
#define __VCGLIB_IMPORT_OBJ

#include <stdio.h>
#include <wrap/callback.h>
#include <vcg/complex/trimesh/allocate.h>
#include "io_obj.h"
#include <wrap/io_trimesh/io_mask.h>

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


struct OBJFacet
{
  Point3f n;
	Point3f t;
  Point3f v[3];

	short attr;  // material index
};

struct TexCoord
{
	float u;
	float v;
	// float w; // not used
};

struct Material
{
	Material()
	{
		strcpy(name, "default_material");
		ambient		= Point3f( .2f,  .2f,  .2f);
		diffuse		= Point3f(1.0f, 1.0f, 1.0f);
		specular	= Point3f(1.0f, 1.0f, 1.0f);
		
		shininess =		 0;
		alpha			= 1.0f;
		
		strcpy(textureFileName, "");
		textureIdx = -1;
	};

  char		name[FILENAME_MAX];

  Point3f ambient;
  Point3f diffuse;
  Point3f specular;

  int			shininess;
  float		alpha;

  bool		bSpecular;

  char		textureFileName[FILENAME_MAX];
	short		textureIdx;
};

enum OBJError {
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
  static const char* obj_error_msg[] =
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
  else return obj_error_msg[error];
};

static int Open( OpenMeshType &m, const char * filename, ObjInfo &oi)
{
	return OpenAscii(m, filename, oi);
}

static int Open( OpenMeshType &m, const char * filename, int &mask, CallBackPos *cb=0)
{
	ObjInfo oi;
  oi.cb=cb; 
	int result = OpenAscii(m, filename, oi);
	mask = oi.mask;
	return result;
}

/*!
* Opens an object file (in ascii format) and populates the mesh passed as first
* accordingly to read data
* \param m The mesh model to be populated with data stored into the file
* \param filename The name of the file to open
* \param oi A structure containing infos about the object to be opened
*/
static int OpenAscii( OpenMeshType &m, const char * filename, ObjInfo &oi)
{
	m.Clear();
	
	CallBackPos *cb = oi.cb;

	// if LoadMask has not been called yet, we call it here
	if (oi.mask == -1)
		LoadMask(filename, oi.mask, oi);

	if (oi.numVertices == 0)
		return E_NO_VERTEX;

	if (oi.numTriangles == 0)
		return E_NO_FACE;

	std::ifstream stream(filename);
	if (stream.fail())
		return E_CANTOPEN;

	std::vector<Material>	materials;  // materials vector
	std::vector<TexCoord>	texCoords;  // texture coordinates
	std::vector<Point3f>  normals;		// vertex normals

	std::vector< std::string > tokens;
	std::string	header;

	short currentMaterialIdx = 0;			// index of current material into materials vector
	Material defaultMaterial;					// default material: white
	materials.push_back(defaultMaterial);

	int numVertices  = 0;  // stores the number of vertices been read till now
	int numTriangles = 0;  // stores the number of faces been read till now
	int numTexCoords = 0;  // stores the number of texture coordinates been read till now
	int numVNormals	 = 0;  // stores the number of vertex normals been read till now

	int numVerticesPlusFaces = oi.numVertices + oi.numTriangles;

	// vertices and faces allocatetion
	VertexIterator vi = Allocator<OpenMeshType>::AddVertices(m,oi.numVertices);
	FaceIterator   fi = Allocator<OpenMeshType>::AddFaces(m,oi.numTriangles);

	// parsing file
	while (!stream.eof())
	{
		tokens.clear();
		TokenizeNextLine(stream, tokens);
		
		unsigned numTokens = tokens.size();
		if (numTokens > 0)
		{
			header.clear();
			header = tokens[0];

			if (header.compare("v")==0)	// vertex
			{
				(*vi).P()[0] = (ScalarType) atof(tokens[1].c_str());
				(*vi).P()[1] = (ScalarType) atof(tokens[2].c_str());
				(*vi).P()[2] = (ScalarType) atof(tokens[3].c_str());
				++numVertices;
				
				// assigning vertex color
				// ----------------------
				if( oi.mask & vcg::tri::io::Mask::IOM_VERTCOLOR)
				{
					Material material = materials[currentMaterialIdx];
					Point3f diffuseColor = material.diffuse;
					unsigned char r = (unsigned char) (diffuseColor[0] * 255.0);
					unsigned char g = (unsigned char) (diffuseColor[1] * 255.0);
					unsigned char b = (unsigned char) (diffuseColor[2] * 255.0);
					unsigned char alpha = (unsigned char) (material.alpha * 255.0);
					Color4b vertexColor = Color4b(r, g, b, alpha);
					(*vi).C()[0] = vertexColor[0];
					(*vi).C()[1] = vertexColor[1];
					(*vi).C()[2] = vertexColor[2];
					(*vi).C()[3] = vertexColor[3];
				}

				++vi;  // move to next vertex iterator

				// callback invocation, abort loading process if the call returns false
				if ((cb !=NULL) && (((numTriangles + numVertices)%100)==0) && !(*cb)(100.0 * (float)(numTriangles + numVertices)/(float)numVerticesPlusFaces, "Vertex Loading"))
					return E_ABORTED;
			}
			else if (header.compare("vt")==0)	// vertex texture coords
			{
				TexCoord t;
				t.u = (ScalarType) atof(tokens[1].c_str());
				t.v = (ScalarType) atof(tokens[2].c_str());
				//t.w = (ScalarType) atof(tokens[3].c_str());	
				texCoords.push_back(t);
				
				numTexCoords++;
			}
			else if (header.compare("vn")==0)  // vertex normal
			{
				Point3f n;
				n[0] = (ScalarType) atof(tokens[1].c_str());
				n[1] = (ScalarType) atof(tokens[2].c_str());
				n[2] = (ScalarType) atof(tokens[3].c_str());	
				normals.push_back(n);

				numVNormals++;
			}
			else if (header.compare("f")==0)  // face
			{
				if (numTokens < 4) return E_LESS_THAN_3VERTINFACE;
				
				int v1_index, v2_index, v3_index;
				int vt1_index, vt2_index, vt3_index;
				int vn1_index, vn2_index, vn3_index;

				if (( oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD ) &&
						( oi.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL ) )
				{
					std::string vertex;
					std::string texcoord;
					std::string normal;

					SplitVVTVNToken(tokens[1], vertex, texcoord, normal);
					v1_index = atoi(vertex.c_str());
					vt1_index = atoi(texcoord.c_str());
					if (vt1_index < 0) vt1_index += numTexCoords; else --vt1_index;
					vn1_index = atoi(normal.c_str());
					if (vn1_index < 0) vn1_index += numVNormals;	else --vn1_index;

					SplitVVTVNToken(tokens[2], vertex, texcoord, normal);
					v2_index = atoi(vertex.c_str());
					vt2_index = atoi(texcoord.c_str());
					if (vt2_index < 0) vt2_index += numTexCoords; else --vt2_index;
					vn2_index = atoi(normal.c_str());
					if (vn2_index < 0) vn2_index += numVNormals;	else --vn2_index;

					SplitVVTVNToken(tokens[3], vertex, texcoord, normal);
					v3_index = atoi(vertex.c_str());
					vt3_index = atoi(texcoord.c_str());
					if (vt3_index < 0) vt3_index += numTexCoords; else --vt3_index;
					vn3_index = atoi(normal.c_str());
					if (vn3_index < 0) vn3_index += numVNormals;	else --vn3_index;
				}
				else if ( oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD )
				{
					std::string vertex;
					std::string texcoord;
					
					SplitVVTToken(tokens[1], vertex, texcoord);
					v1_index = atoi(vertex.c_str());
					vt1_index = atoi(texcoord.c_str());
					if (vt1_index < 0) vt1_index += numTexCoords; else --vt1_index;

					SplitVVTToken(tokens[2], vertex, texcoord);
					v2_index = atoi(vertex.c_str());
					vt2_index = atoi(texcoord.c_str());
					if (vt2_index < 0) vt2_index += numTexCoords; else --vt2_index;
					
					SplitVVTToken(tokens[3], vertex, texcoord);
					v3_index = atoi(vertex.c_str());
					vt3_index = atoi(texcoord.c_str());
					if (vt3_index < 0) vt3_index += numTexCoords; else --vt3_index;
				}
				else if ( oi.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL )
				{
					std::string vertex;
					std::string normal;
					
					SplitVVNToken(tokens[1], vertex, normal);
					v1_index = atoi(vertex.c_str());
					vn1_index = atoi(normal.c_str());
					if (vn1_index < 0) vn1_index += numVNormals;	else --vn1_index;

					SplitVVNToken(tokens[2], vertex, normal);
					v2_index = atoi(vertex.c_str());
					vn2_index = atoi(normal.c_str());
					if (vn2_index < 0) vn2_index += numVNormals;	else --vn2_index;
					
					SplitVVNToken(tokens[3], vertex, normal);
					v3_index = atoi(vertex.c_str());
					vn3_index = atoi(normal.c_str());
					if (vn3_index < 0) vn3_index += numVNormals;	else --vn3_index;
				}
				else
				{
					v1_index = atoi(tokens[1].c_str());
					v2_index = atoi(tokens[2].c_str());
					v3_index = atoi(tokens[3].c_str());
				}
			
				// assigning wedge texture coordinates
				// -----------------------------------
				if ( oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD )
				{
					Material material = materials[currentMaterialIdx];
					
					TexCoord t = texCoords[vt1_index];
					(*fi).WT(0).u() = t.u;
					(*fi).WT(0).v() = t.v;
					/*if(multit) */(*fi).WT(0).n() = material.textureIdx;

					t = texCoords[vt2_index];
					(*fi).WT(1).u() = t.u;
					(*fi).WT(1).v() = t.v;
					/*if(multit) */(*fi).WT(1).n() = material.textureIdx;

					t = texCoords[vt3_index];
					(*fi).WT(2).u() = t.u;
					(*fi).WT(2).v() = t.v;
					/*if(multit) */(*fi).WT(2).n() = material.textureIdx;
				}
				

				if (v1_index < 0) v1_index += numVertices;
				else if (v1_index > numVertices)	return E_BAD_VERT_INDEX;
				else v1_index--;  // since index starts from 1 instead of 0

				if (v2_index < 0) v2_index += numVertices;
				else if (v2_index > numVertices)	return E_BAD_VERT_INDEX;
				else v2_index--;  // since index starts from 1 instead of 0

				if (v3_index < 0) v3_index += numVertices;
				else if (v3_index > numVertices)	return E_BAD_VERT_INDEX;
				else v3_index--;	// since index starts from 1 instead of 0
			
				// assigning face vertices
				// -----------------------
				(*fi).V(0) = &(m.vert[ v1_index ]);
				(*fi).V(1) = &(m.vert[ v2_index ]);
				(*fi).V(2) = &(m.vert[ v3_index ]);
				
				// assigning face normal
				// ---------------------
				if ( oi.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL )
				{
					// face normal is computed as an average of wedge normals
					Point3f n = (normals[vn1_index] + normals[vn2_index] + normals[vn3_index]);
					n.Normalize();

					(*fi).N() = n;

					(*fi).WN(0) = normals[vn1_index];
					(*fi).WN(1) = normals[vn2_index];
					(*fi).WN(2) = normals[vn3_index];
				}
				else	// computing face normal from position of face vertices
					face::ComputeNormalizedNormal(*fi);

				// assigning face color
				// --------------------
				Color4b faceColor;	// declare it outside code block since other triangles
														// of this face will share the same color
				if( oi.mask & vcg::tri::io::Mask::IOM_FACECOLOR)
				{
					Material material = materials[currentMaterialIdx];
					Point3f diffuseColor = material.diffuse;
					unsigned char r = (unsigned char) (diffuseColor[0] * 255.0);
					unsigned char g = (unsigned char) (diffuseColor[1] * 255.0);
					unsigned char b = (unsigned char) (diffuseColor[2] * 255.0);
					unsigned char alpha = (unsigned char) (material.alpha * 255.0);
					faceColor = Color4b(r, g, b, alpha);
					(*fi).C()[0] = faceColor[0];
					(*fi).C()[1] = faceColor[1];
					(*fi).C()[2] = faceColor[2];
					(*fi).C()[3] = faceColor[3];
				}

				++fi;
				++numTriangles;

				int vertexesPerFace = tokens.size() -1;
				int iVertex = 3;
				while (iVertex < vertexesPerFace)  // add other triangles
				{
					int v4_index;
					int vt4_index;
					int vn4_index;

					if (( oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD ) &&
							( oi.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL ) )
					{
						std::string vertex;
						std::string texcoord;
						std::string normal;

						SplitVVTVNToken(tokens[++iVertex], vertex, texcoord, normal);
						v4_index	= atoi(vertex.c_str());
						vt4_index = atoi(texcoord.c_str());
						if (vt4_index < 0) vt4_index += numTexCoords; else --vt4_index;
						vn4_index = atoi(normal.c_str());
						if (vn4_index < 0) vn4_index += numVNormals;	else --vn4_index;
					}
					else if ( oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD )
					{
						std::string vertex;
						std::string texcoord;
						
						SplitVVTToken(tokens[++iVertex], vertex, texcoord);
						v4_index	= atoi(vertex.c_str());
						vt4_index = atoi(texcoord.c_str());
						if (vt4_index < 0) vt4_index += numTexCoords; else --vt4_index;
					}
					else if ( oi.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL )
					{
						std::string vertex;
						std::string normal;
						
						SplitVVNToken(tokens[++iVertex], vertex, normal);
						v4_index = atoi(vertex.c_str());
						vn4_index = atoi(normal.c_str());
						if (vn4_index < 0) vn4_index += numVNormals;	else --vn4_index;
					}
					else
						v4_index	= atoi(tokens[++iVertex].c_str());

					// assigning wedge texture coordinates
					// -----------------------------------
					if( oi.mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD )
					{
						Material material = materials[currentMaterialIdx];
						TexCoord t = texCoords[vt1_index];
						(*fi).WT(0).u() = t.u;
						(*fi).WT(0).v() = t.v;
						/*if(multit) */(*fi).WT(0).n() = material.textureIdx;

						t = texCoords[vt3_index];
						(*fi).WT(1).u() = t.u;
						(*fi).WT(1).v() = t.v;
						/*if(multit) */(*fi).WT(1).n() = material.textureIdx;

						t = texCoords[vt4_index];
						(*fi).WT(2).u() = t.u;
						(*fi).WT(2).v() = t.v;
						/*if(multit) */(*fi).WT(2).n() = material.textureIdx;

						vt3_index = vt4_index;
					}
						
					if (v4_index < 0) v4_index += numVertices;
					else if (v4_index > numVertices)	return E_BAD_VERT_INDEX;
					else v4_index--;	// since index starts from 1 instead of 0

					// assigning face vertices
					// -----------------------
					(*fi).V(0) = &(m.vert[ v1_index ]);
					(*fi).V(1) = &(m.vert[ v3_index ]);
					(*fi).V(2) = &(m.vert[ v4_index ]);

					// assigning face normal
					// ---------------------
					if ( oi.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL )
					{
						// face normal is computed as an average of wedge normals
						Point3f n = (normals[vn1_index] + normals[vn3_index] + normals[vn4_index]);
						n.Normalize();

						(*fi).N() = n;

						(*fi).WN(0) = normals[vn1_index];
						(*fi).WN(1) = normals[vn3_index];
						(*fi).WN(2) = normals[vn4_index];

						vn3_index = vn4_index;
					}
					else	// computing face normal from position of face vertices
						face::ComputeNormalizedNormal(*fi);

					// assigning face color
					// --------------------
					if( oi.mask & vcg::tri::io::Mask::IOM_FACECOLOR)
					{
						(*fi).C()[0] = faceColor[0];
						(*fi).C()[1] = faceColor[1];
						(*fi).C()[2] = faceColor[2];
						(*fi).C()[3] = faceColor[3];
					}

					// La faccia composta da piu' di tre vertici viene suddivisa in triangoli
					// secondo lo schema seguente:
					//                     v5
					//                    /  \
					//                   /    \
					//                  /      \ 
					//                 v1------v4 
					//                 |\      /
					//                 | \    /
					//                 |  \  /
					//                v2---v3
					//
					// Nell'esempio in figura, il poligono di 5 vertici (v1,v2,v3,v4,v5)
					// viene suddiviso nei triangoli (v1,v2,v3), (v1,v3,v4) e (v1,v4,v5).
					// In questo modo il vertice v1 diventa il vertice comune di tutti i
					// triangoli in cui il poligono originale viene suddiviso, e questo
					// puo' portare alla generazione di triangoli molto 'sottili' (tuttavia
					// con questo metodo si ha la garanzia che tutti i triangoli generati
					// conservino la stessa orientazione).

					++fi;
					++numTriangles;

					v3_index = v4_index;
				}
				
				// callback invocation, abort loading process if the call returns false
				if ((cb !=NULL) && (((numTriangles + numVertices)%100)==0) && !(*cb)(100.0 * (float)(numTriangles + numVertices)/(float)numVerticesPlusFaces, "Face Loading"))
					return E_ABORTED;
			}
			else if (header.compare("mtllib")==0)	// material library
			{
				// obtain the name of the file containing materials library
				std::string materialFileName = tokens[1];
				LoadMaterials( materialFileName.c_str(), materials, m.textures);
			}
			else if (header.compare("usemtl")==0)	// material usage
			{
				std::string materialName = tokens[1];
				bool found = false;
				unsigned i = 0;
				while (!found && (i < materials.size()))
				{
					std::string currentMaterialName = materials[i].name;
					if (currentMaterialName == materialName)
					{
						currentMaterialIdx = i;
						found = true;
					}
					++i;
				}
			}
			// we simply ignore other situations
		}
	}
	
  return E_NOERROR;
} // end of Open


	/*!
	* Read the next valid line and parses it into "tokens", allowing
	*	the tokens to be read one at a time.
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
			while ((line[from]==' ' || line[from]=='\t') && from!=length)
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

	inline static const void SplitVVTToken(std::string token, std::string &vertex, std::string &texcoord)
	{
		vertex.clear();
		texcoord.clear();

		size_t from		= 0; 
		size_t to			= 0;
		size_t length = token.size();
		
		if(from!=length)
    {
			char c = token[from];
			vertex.push_back(c);

			to = from+1;
			while (to!=length && ((c = token[to]) !='/'))
			{
				vertex.push_back(c);
				++to;
			}
			++to;
			while (to!=length && ((c = token[to]) !=' '))
			{
				texcoord.push_back(c);
				++to;
			}
		}
	}	// end of SplitVVTToken

	inline static const void SplitVVNToken(std::string token, std::string &vertex, std::string &normal)
	{
		vertex.clear();
		normal.clear();

		size_t from		= 0; 
		size_t to			= 0;
		size_t length = token.size();
		
		if(from!=length)
    {
			char c = token[from];
			vertex.push_back(c);

			to = from+1;
			while (to!=length && ((c = token[to]) !='/'))
			{
				vertex.push_back(c);
				++to;
			}
			++to;
			++to;  // should be the second '/'
			while (to!=length && ((c = token[to]) !=' '))
			{
				normal.push_back(c);
				++to;
			}
		}
	}	// end of SplitVVNToken

	inline static const void SplitVVTVNToken(std::string token, std::string &vertex, std::string &texcoord, std::string &normal)
	{
		vertex.clear();
		texcoord.clear();
		normal.clear();
		
		size_t from		= 0; 
		size_t to			= 0;
		size_t length = token.size();
		
		if(from!=length)
    {
			char c = token[from];
			vertex.push_back(c);

			to = from+1;
			while (to!=length && ((c = token[to]) !='/'))
			{
				vertex.push_back(c);
				++to;
			}
			++to;
			while (to!=length && ((c = token[to]) !='/'))
			{
				texcoord.push_back(c);
				++to;
			}
			++to;
			while (to!=length && ((c = token[to]) !=' '))
			{
				normal.push_back(c);
				++to;
			}
		}
	}	// end of SplitVVTVNToken


	/*!
	* Retrieves kind of data stored into the file and fills a mask appropriately
	* \param filename The name of the file to open
	*	\param mask	A mask which will be filled according to type of data found in the object
	*/
	static bool LoadMask(const char * filename, int &mask)
	{
		ObjInfo oi;
		return LoadMask(filename, mask,oi);
	}

	
	/*!
	* Retrieves kind of data stored into the file and fills a mask appropriately
	* \param filename The name of the file to open
	*	\param mask	A mask which will be filled according to type of data found in the object
	* \param oi A structure which will be filled with infos about the object to be opened
	*/
	static bool LoadMask(const char * filename, int &mask, ObjInfo &oi)
	{
		std::ifstream stream(filename);
		if (stream.fail())
			return false;

		bool bHasPerWedgeTexCoord = false;
		bool bHasPerWedgeNormal		= false;
		bool bUsingMaterial				= false;
		bool bHasPerVertexColor		= false;
		bool bHasPerFaceColor			= false;

		std::string header;
		std::vector<std::string> tokens;
		
		int numVertices		= 0;  // stores the number of vertexes been read till now
		int numTriangles	= 0;	// stores the number of triangular faces been read till now

		// cycle till we encounter first face
		while (!stream.eof())
		{
			tokens.clear();
			header.clear();
			TokenizeNextLine(stream, tokens);
			
			if (tokens.size() > 0)
			{
				header = tokens[0];

				if (header.compare("v")==0)
				{
					++numVertices;
					if (bUsingMaterial)	bHasPerVertexColor = true;
				}
				else if (header.compare("f")==0)
				{
					numTriangles += (tokens.size() - 3);
					std::string remainingText = tokens[1];
					
					// we base our assumption on the fact that the way vertex data is
					// referenced into faces must be consistent among the entire file
					int charIdx = 0;
					size_t length = remainingText.size();
					char c;
					while((charIdx != length) && ((c = remainingText[charIdx])!='/') && (c != ' '))
						++charIdx;

					if (c == '/')
					{
						++charIdx;
						if ((charIdx != length) && ((c = remainingText[charIdx])!='/'))
						{
							bHasPerWedgeTexCoord = true;

							++charIdx;
							while((charIdx != length) && ((c = remainingText[charIdx])!='/') && (c != ' '))
								++charIdx;
							
							if (c == '/')
								bHasPerWedgeNormal   = true;
							break;
						}
						else
						{
							bHasPerWedgeNormal   = true;
							break;
						}					
					}

					if (bUsingMaterial)	bHasPerFaceColor = true;
				}
				else if (header.compare("usemtl")==0)
					bUsingMaterial = true;
			}
		}

		// after the encounter of first face we avoid to do additional tests
		while (!stream.eof())
		{
			tokens.clear();
			header.clear();
			TokenizeNextLine(stream, tokens);
			
			if (tokens.size() > 0)
			{
				header = tokens[0];
				
				if (header.compare("v")==0)
				{
					++numVertices;
					if (bUsingMaterial)	bHasPerVertexColor = true;
				}
				else if (header.compare("f")==0)
				{
					numTriangles += (tokens.size() - 3);
					if (bUsingMaterial)	bHasPerFaceColor = true;
				}
				else if (header.compare("usemtl")==0)
					bUsingMaterial = true;
			}
		}

		mask=0;
		
		if (bHasPerWedgeTexCoord)
			mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
		if (bHasPerWedgeNormal)
			mask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;
		if (bHasPerVertexColor)
			mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
		if (bHasPerFaceColor)
			mask |= vcg::tri::io::Mask::IOM_FACECOLOR;
	
		/*
		mask |= vcg::tri::io::Mask::IOM_VERTCOORD;
		mask |= vcg::tri::io::Mask::IOM_VERTFLAGS;
		mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
		mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
		mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
		
		mask |= vcg::tri::io::Mask::IOM_FACEINDEX;
		mask |= vcg::tri::io::Mask::IOM_FACEFLAGS;

		mask |= vcg::tri::io::Mask::IOM_FACEQUALITY;
		mask |= vcg::tri::io::Mask::IOM_WEDGTEXMULTI;
		mask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;
		mask |= vcg::tri::io::Mask::IOM_FACECOLOR;
		*/

		oi.mask = mask;
		oi.numVertices	= numVertices;
		oi.numTriangles = numTriangles;

		return true;
	}


	static bool LoadMaterials(const char * filename, std::vector<Material> &materials, std::vector<std::string> &textures)
	{
		// nel chiamante garantire che ci si trovi nella directory giusta 

		std::ifstream stream(filename);
		if (stream.fail())
			return false;
    
		std::vector< std::string > tokens;
		std::string	header;
	
		materials.clear();
		Material currentMaterial;
		
		bool first = true;
		while (!stream.eof())
		{
			tokens.clear();
			TokenizeNextLine(stream, tokens);
		
			if (tokens.size() > 0)
			{
				header.clear();
				header = tokens[0];

				if (header.compare("newmtl")==0)
				{
					if (!first)
					{
						materials.push_back(currentMaterial);
						currentMaterial = Material();
					}
					else
						first = false;

					strcpy(currentMaterial.name, tokens[1].c_str());
				}
				else if (header.compare("Ka")==0)
				{
					float r = (float) atof(tokens[1].c_str());
					float g = (float) atof(tokens[2].c_str());
					float b = (float) atof(tokens[3].c_str());

					currentMaterial.ambient = Point3f(r, g, b); 
				}
				else if (header.compare("Kd")==0)
				{
					float r = (float) atof(tokens[1].c_str());
					float g = (float) atof(tokens[2].c_str());
					float b = (float) atof(tokens[3].c_str());

          currentMaterial.diffuse = Point3f(r, g, b); 
				}
				else if (header.compare("Ks")==0)
				{
					float r = (float) atof(tokens[1].c_str());
					float g = (float) atof(tokens[2].c_str());
					float b = (float) atof(tokens[3].c_str());

          currentMaterial.specular = Point3f(r, g, b); 
				}
				else if (	(header.compare("d")==0) ||
									(header.compare("Tr")==0)	)	// alpha
				{
          currentMaterial.alpha = (float) atof(tokens[1].c_str());
				}
				else if (header.compare("Ns")==0)  // shininess        
				{
					currentMaterial.shininess = atoi(tokens[1].c_str());
				}
				else if (header.compare("illum")==0)	// specular illumination on/off
				{
					int illumination = atoi(tokens[1].c_str());
          currentMaterial.bSpecular = (illumination == 2);
				}
				else if (header.compare("map_Kd")==0)	// texture name
				{
					std::string textureName = tokens[1];
					strcpy(currentMaterial.textureFileName, textureName.c_str());
					
					// adding texture name into textures vector (if not already present)
					// avoid adding the same name twice
					bool found = false;
					unsigned size = textures.size();
					unsigned j = 0;
					while (!found && (j < size))
					{
						if (textureName.compare(textures[j])==0)
						{
							currentMaterial.textureIdx = (int)j;
							found = true;
						}
						++j;
					}
					if (!found)
					{
						textures.push_back(textureName);
						currentMaterial.textureIdx = (int)size;
					}
				}
				// we simply ignore other situations
			}
		}
		materials.push_back(currentMaterial);  // add last read material

		stream.close();

		return true;
	}

}; // end class
} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif  // ndef __VCGLIB_IMPORT_OBJ