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
#include <wrap/ply/io_mask.h>

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
	// float w; // not used for now
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
	};

  char		name[FILENAME_MAX];

  Point3f ambient;
  Point3f diffuse;
  Point3f specular;

  int			shininess;
  float		alpha;

  bool		bSpecular;

  char		textureFileName[FILENAME_MAX];
};

enum OBJError {
	E_NOERROR,				// 0
		// Errori di open
	E_CANTOPEN,				// 1
	E_UNESPECTEDEOF,	// 2
	E_ABORTED					// 3
};

static const char* ErrorMsg(int error)
{
  static const char* obj_error_msg[] =
  {
		"No errors",
		"Can't open file",
		"Premature End of file",
		"File opening aborted"
	};

  if(error>2 || error<0) return "Unknown error";
  else return obj_error_msg[error];
};

static int Open( OpenMeshType &m, const char * filename, ObjInfo &oi)
{
	// obj file should be in ascii format
	return OpenAscii(m, filename, oi);
}

static int Open( OpenMeshType &m, const char * filename, CallBackPos *cb=0)
{
	ObjInfo oi;
  oi.cb=cb; 
	// obj file should be in ascii format
  return OpenAscii(m, filename, oi);
}

// here mask is an out parameter
static int Open( OpenMeshType &m, const char * filename, int &mask, CallBackPos *cb=0)
{
	ObjInfo oi;
  oi.cb=cb; 
	// obj file should be in ascii format
  int result = OpenAscii(m, filename, oi);
	mask = oi.mask;
	return result;
}

static int OpenAscii( OpenMeshType &m, const char * filename, ObjInfo &oi)
{
	m.Clear();
	
	CallBackPos *cb = oi.cb;
	// if LoadMask has not been called yet, we call it here
	if (oi.mask == -1)
	{
		int mask;
		LoadMask(filename, mask, oi);
	}

	// creating an input file stream
	std::ifstream stream(filename);
	if (stream.fail())
		return E_CANTOPEN;

	std::vector<Material>	materials;  // materials vector
	std::vector<TexCoord>	texCoords;  // texture coordinates

	std::vector< std::string > tokens;
	std::string	header;

	short currentMaterialIdx = 0;
	Material defaultMaterial;
	materials.push_back(defaultMaterial);

	int numVertices = 0;  // stores the number of vertices been read till now
	int numFaces		= 0;  // stores the number of faces been read till now

	// vertexes allocation is done once a time
	VertexIterator vi = Allocator<OpenMeshType>::AddVertices(m,oi.numVertexes);
	
	while (!stream.eof())  // same as !( stream.rdstate( ) & ios::eofbit )
	{
		tokens.clear();
		TokenizeNextLine(stream, tokens);
		
		if (tokens.size() > 0)
		{
			header.clear();
			header = tokens[0];

			if (header.compare("v")==0)	// vertex
			{
				(*vi).P()[0] = (ScalarType) atof(tokens[1].c_str());
				(*vi).P()[1] = (ScalarType) atof(tokens[2].c_str());
				(*vi).P()[2] = (ScalarType) atof(tokens[3].c_str());
				++numVertices;
				
				++vi;  // move to next vertex iterator

				// callback invocation, abort loading process if result is false
				if ((cb !=NULL) && ((numVertices%100)==0) && !(*cb)(100.0 * (float)numVertices/(float)oi.numVertexes, "Lettura vertici"))
					return E_ABORTED;
			}
			else if (header.compare("vt")==0)	// vertex texture coords
			{
				TexCoord t;
				t.u = (ScalarType) atof(tokens[1].c_str());
				t.v = (ScalarType) atof(tokens[2].c_str());
				//t.w = (ScalarType) atof(tokens[3].c_str());	
				texCoords.push_back(t);
			}
			else if (header.compare("vn")==0)  // vertex normal
			{
				// do nothing (for now)
			}
			else if (header.compare("f")==0)  // face
			{
				int v1_index, v2_index, v3_index;
				int vt1_index, vt2_index, vt3_index;
				
				// note that, due to face triangulation, real number of triangular faces
				// may be greater than the number of faces been read by loadmask, 
				FaceIterator fi = Allocator<OpenMeshType>::AddFaces(m,1);
				if( oi.mask & ply::PLYMask::PM_WEDGTEXCOORD )
				{
					std::string vertex;
					std::string texcoord;
					
					SplitVVTToken(tokens[1], vertex, texcoord);
					v1_index = atoi(vertex.c_str());
					vt1_index = atoi(texcoord.c_str());

					SplitVVTToken(tokens[2], vertex, texcoord);
					v2_index = atoi(vertex.c_str());
					vt2_index = atoi(texcoord.c_str());
					
					SplitVVTToken(tokens[3], vertex, texcoord);
					v3_index = atoi(vertex.c_str());
					vt3_index = atoi(texcoord.c_str());

					// also texcoord index starts from 1 instead of 0, so we decrement it by 1
					TexCoord t = texCoords[--vt1_index];
					(*fi).WT(0).u() = t.u;
					(*fi).WT(0).v() = t.v;
					//if(multit) (*fi).WT(0).n() = fa.tcoordind;

					t = texCoords[--vt2_index];
					(*fi).WT(1).u() = t.u;
					(*fi).WT(1).v() = t.v;
					//if(multit) (*fi).WT(1).n() = fa.tcoordind;

					t = texCoords[--vt3_index];
					(*fi).WT(2).u() = t.u;
					(*fi).WT(2).v() = t.v;
					//if(multit) (*fi).WT(2).n() = fa.tcoordind;
				}
				else
				{
					v1_index = atoi(tokens[1].c_str());
					v2_index = atoi(tokens[2].c_str());
					v3_index = atoi(tokens[3].c_str());
				}

				if (v1_index < 0) v1_index += numVertices; else v1_index--;  // since index start from 1
				if (v2_index < 0) v2_index += numVertices; else v2_index--;  // instead of 0, as stored
				if (v3_index < 0) v3_index += numVertices; else v3_index--;  // int the vertices vector
			

				(*fi).V(0) = &(m.vert[ v1_index ]);
				(*fi).V(1) = &(m.vert[ v2_index ]);
				(*fi).V(2) = &(m.vert[ v3_index ]);
				
				
				Color4b faceColor;	// declare it outside code block since other triangles
														// of this face will share the same color
				//TODO: da usare
				//if( oi.mask & ply::PLYMask::PM_FACECOLOR)
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

				int vertexesPerFace = tokens.size() -1;
				int iVertex = 3;
				while (iVertex < vertexesPerFace)  // add other triangles
				{
					int v4_index;
					int vt4_index;

					fi=Allocator<OpenMeshType>::AddFaces(m,1);
					if( oi.mask & ply::PLYMask::PM_WEDGTEXCOORD )
					{
						std::string vertex;
						std::string texcoord;
						
						SplitVVTToken(tokens[++iVertex], vertex, texcoord);
						v4_index	= atoi(vertex.c_str());
						vt4_index = atoi(texcoord.c_str());

						// also texcoord index starts from 1 instead of 0, so we decrement it by 1
						TexCoord t = texCoords[vt1_index];
						(*fi).WT(0).u() = t.u;
						(*fi).WT(0).v() = t.v;
						//if(multit) (*fi).WT(0).n() = fa.tcoordind;

						t = texCoords[vt3_index];
						(*fi).WT(1).u() = t.u;
						(*fi).WT(1).v() = t.v;
						//if(multit) (*fi).WT(1).n() = fa.tcoordind;

						t = texCoords[--vt4_index];
						(*fi).WT(2).u() = t.u;
						(*fi).WT(2).v() = t.v;
						//if(multit) (*fi).WT(2).n() = fa.tcoordind;

						vt3_index = vt4_index;
					}
					else
						v4_index	= atoi(tokens[++iVertex].c_str());
						
					if (v4_index < 0) v4_index += numVertices; else v4_index--;
			
					(*fi).V(0) = &(m.vert[ v1_index ]);
					(*fi).V(1) = &(m.vert[ v3_index ]);
					(*fi).V(2) = &(m.vert[ v4_index ]);

					//TODO: da usare
					//if( oi.mask & ply::PLYMask::PM_FACECOLOR)
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
					// TODO: provare a suddividere il poligono in triangoli in maniera piu'
					// uniforme...

					v3_index = v4_index;
				}
				// TODO: gestire opportunamente presenza di errori nel file

				++numFaces;
				// callback invocation, abort loading process if result is false
				if ((cb !=NULL) && ((numFaces%100)==0) && !(*cb)(100.0 * (float)numFaces/(float)oi.numFaces, "Lettura facce"))
					return E_ABORTED;
			}
			else if (header.compare("mtllib")==0)	// material library
			{
				// obtain the name of the file  containing materials library
				std::string materialFileName = tokens[1];
				LoadMaterials( materialFileName.c_str(), materials );
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
			// for now, we simply ignore other situations
		}
	}

	// populating vector of texture names stored into the model
	for (unsigned i=0; i< materials.size(); ++i)
	{
		std::string textureName = materials[i].textureFileName;
		if (!textureName.empty())
		{
			// avoid adding the same name twice
			bool found = false;
			unsigned j = 0;
			while (!found && (j < m.textures.size()))
			{
				if (textureName.compare(m.textures[j])==0)
					found = true;
				++j;
			}
			if (!found)
				m.textures.push_back(textureName);
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
			char c = line[from];
			vertex.push_back(c);

			to = from+1;
			while (to!=length && ((c = line[to]) !='/'))
			{
				vertex.push_back(c);
				++to;
			}
			++to;
			++to;  // should be the second '/'
			while (to!=length && ((c = line[to]) !=' '))
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
			char c = line[from];
			vertex.push_back(c);

			to = from+1;
			while (to!=length && ((c = line[to]) !='/'))
			{
				vertex.push_back(c);
				++to;
			}
			++to;
			while (to!=length && ((c = line[to]) !='/'))
			{
				texcoord.push_back(c);
				++to;
			}
			++to;
			while (to!=length && ((c = line[to]) !=' '))
			{
				normal.push_back(c);
				++to;
			}
		}
	}	// end of SplitVVTVNToken


	inline static const void GetNextLineHeader(std::ifstream &stream, std::string &header, std::string &remainingText)
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
		
		char c = line[from];
		while (line[from]==' ' && from!=length)
			from++;
    if(from!=length)
    {
			to = from+1;
			header.push_back(c);
			while (to!=length && ((c = line[to]) !=' '))
			{
				header.push_back(c);
				++to;
			}
			++to;
			while (to!=length && ((c = line[to]) !=' '))
			{
				remainingText.push_back(c);
				++to;
			}
		}
	} // end of GetNextLineHeader


	// Da preferire nell'utilizzo la versione che prende come parametro
	// di ingresso anche una struttura ObjInfo di cui riempira' i campi
	static bool LoadMask(const char * filename, int &mask)
	{
		ObjInfo oi;
		return LoadMask(filename, mask,oi);
	}

	
	static bool LoadMask(const char * filename, int &mask, ObjInfo &oi)
	{
		std::ifstream stream(filename);
		if (stream.fail())
			return false;

		bool bHasPerWedgeTexCoord = false;
		bool bHasPerWedgeNormal		= false;
		
		std::string header;
		std::string remainingText;
		
		int numVertexes = 0;  // stores the number of vertexes been read till now
		int numFaces = 0;			// stores the number of faces been read till now

		// cycle till we encounter first face
		while (!stream.eof())  // same as !( stream.rdstate( ) & ios::eofbit )
		{
			header.clear();
			remainingText.clear();
			GetNextLineHeader(stream, header, remainingText);
			
			if (header.compare("v")==0)
				++numVertexes;
			//else if (header.compare("vt")==0)
			//{	
			//}
			//else if (header.compare("vn")==0)
			//{
			//}
			else if (header.compare("f")==0)
			{
				numFaces++;
				
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
			}
		}

		// after the encounter of first face we avoid to do additional tests
		while (!stream.eof())  // same as !( stream.rdstate( ) & ios::eofbit )
		{
			header.clear();
			remainingText.clear();
			GetNextLineHeader(stream, header, remainingText);
			
			if (header.compare("v")==0)
				++numVertexes;
			//else if (header.compare("vt")==0)
			//{	
			//}
			//else if (header.compare("vn")==0)
			//{
			//}
			else if (header.compare("f")==0)
				numFaces++;
		}


		mask=0;
		
		if (bHasPerWedgeTexCoord)
			mask |= ply::PLYMask::PM_WEDGTEXCOORD;
		if (bHasPerWedgeNormal)
			mask |= ply::PLYMask::PM_WEDGNORMAL;
		// TODO: AGGIUNGERE
		// mask |= ply::PLYMask::PM_FACECOLOR

		oi.mask = mask;
		oi.numVertexes = numVertexes;
		oi.numFaces = numFaces;

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
		if( pf.AddToRead(FaceDesc(3))!=-1 ) mask |= ply::PLYMask::PM_WEDGTEXCOORD;
		if( pf.AddToRead(FaceDesc(5))!=-1 ) mask |= ply::PLYMask::PM_WEDGTEXMULTI;
		if( pf.AddToRead(FaceDesc(4))!=-1 ) mask |= ply::PLYMask::PM_WEDGCOLOR;
		if( ( pf.AddToRead(FaceDesc(6))!=-1 ) && 
				( pf.AddToRead(FaceDesc(7))!=-1 ) &&
				( pf.AddToRead(FaceDesc(8))!=-1 )  )  mask |= ply::PLYMask::PM_FACECOLOR;
		*/

		return true;
	}


	static bool LoadMaterials(const char * filename, std::vector<Material> &materials)
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
		while (!stream.eof())  // same as !( stream.rdstate( ) & ios::eofbit )
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
					float r = atof(tokens[1].c_str());
					float g = atof(tokens[2].c_str());
					float b = atof(tokens[3].c_str());

					currentMaterial.ambient = Point3f(r, g, b); 
				}
				else if (header.compare("Kd")==0)
				{
					float r = atof(tokens[1].c_str());
					float g = atof(tokens[2].c_str());
					float b = atof(tokens[3].c_str());

          currentMaterial.diffuse = Point3f(r, g, b); 
				}
				else if (header.compare("Ks")==0)
				{
					float r = atof(tokens[1].c_str());
					float g = atof(tokens[2].c_str());
					float b = atof(tokens[3].c_str());

          currentMaterial.specular = Point3f(r, g, b); 
				}
				else if (	(header.compare("d")==0) ||
									(header.compare("Tr")==0)	)	// alpha
				{
          currentMaterial.alpha = atof(tokens[1].c_str());
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
					strcpy(currentMaterial.textureFileName, tokens[1].c_str());
				}
				// for now, we simply ignore other situations
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