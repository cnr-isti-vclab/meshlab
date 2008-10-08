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

/* This code has been adapted from the OFF importer and Gael's GTS loader from Expe */

#ifndef __VCGLIB_IMPORT_GTS
#define __VCGLIB_IMPORT_GTS

#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <vcg/space/color4.h>
#include<vcg/complex/trimesh/allocate.h>
#include <wrap/callback.h>
#include <wrap/io_trimesh/io_mask.h>

namespace vcg
{
	namespace tri
	{
		namespace io
		{
			// /** \addtogroup  */
			// /* @{ */
			/**
			This class encapsulate a filter for importing GTS meshes.
			The GTS file format of the GNU Triangulated Surface library http://gts.sourceforge.net/
			*/
			template<class MESH_TYPE>
			class ImporterGTS
			{
			public:

				typedef typename MESH_TYPE::VertexType			VertexType;
				typedef typename MESH_TYPE::VertexIterator	VertexIterator;
				typedef typename MESH_TYPE::VertexPointer		VertexPointer;
				typedef typename MESH_TYPE::FaceType				FaceType;
				typedef typename MESH_TYPE::FaceIterator		FaceIterator;
				typedef typename MESH_TYPE::FacePointer			FacePointer;
				typedef typename MESH_TYPE::CoordType				CoordType;
				typedef typename MESH_TYPE::ScalarType			ScalarType;

				enum GTSCodes {NoError=0, CantOpen, InvalidFile,
					UnsupportedFormat, ErrorNotTriangularFace};

				struct Options
				{
					Options()
						: onlyMaskFlag(false), onlyPoints(false), flipFaces(false)
					{}
					bool onlyMaskFlag;
					bool onlyPoints;
					bool flipFaces;
				};

				/*!
				*	Standard call for knowing the meaning of an error code
				* \param message_code	The code returned by <CODE>Open</CODE>
				*	\return							The string describing the error code
				*/
				static const char* ErrorMsg(int message_code)
				{
					static const char* error_msg[] =
					{
						"No errors", "Can't open file", "Invalid file",
						"Unsupported format", "Face with more than 3 vertices"
					};

					if(message_code>4 || message_code<0)
						return "Unknown error";
					else
						return error_msg[message_code];
				};

				/**
				 * Load only the properties of the 3D objects.
				 *
				 * \param filename    the name of the file to read from
				 * \param loadmask    the mask which encodes the properties
				 * \return            the operation result
				 */
				static bool LoadMask(const char *filename, int &loadmask)
				{
					// To obtain the loading mask all the file must be parsed
					// to distinguish between per-vertex and per-face color attribute.
          loadmask=0;
					MESH_TYPE dummyMesh;
					return (Open(dummyMesh, filename, loadmask,0,true)==NoError);
				}

				static int Open(MESH_TYPE &mesh, const char *filename, CallBackPos *cb=0)
				{
					int loadmask;
					return Open(mesh,filename,loadmask,cb);
				}

				/*!
				 *  Standard call for reading a mesh.
				 *
				 *  \param mesh         the destination mesh
				 *  \param filename     the name of the file to read from
				 *  \return             the operation result
				 */
				static int Open(MESH_TYPE &mesh, const char *filename, int &loadmask,
					CallBackPos *cb=0, bool onlyMaskFlag=false)
				{
					Options opt;
					opt.onlyMaskFlag = onlyMaskFlag;
					return Open(mesh, filename, loadmask, opt, cb);
				}

				static int Open(MESH_TYPE &mesh, const char *filename, int &loadmask,
					const Options& options, CallBackPos *cb=0)
				{
					QFile device(filename);
					if ( (!device.open(QFile::ReadOnly)) )
						return CantOpen;

					QTextStream stream(&device);

					loadmask = Mask::IOM_VERTCOORD | Mask::IOM_FACEINDEX;

					int nofVertices, nofFaces, nofEdges;
					int id0, id1, id2;
					int eid0, eid1, eid2;
					Point3f v;

					QString line;
					QStringList sa;
					bool done = false;
					do
					{
							line = stream.readLine();
							line = line.trimmed();
							sa = line.split(' ');
							if ((!line.startsWith("#")) && (sa.size()>=3))
							{
									nofVertices = sa[0].toInt();
									nofEdges    = sa[1].toInt();
									nofFaces    = sa[2].toInt();
									done = true;
							}
					} while (!done);

					if(options.onlyMaskFlag) return NoError;

					int total = nofVertices + nofEdges + nofFaces;

					std::cerr << "GtsMeshReader::nofVertices = " << nofVertices << std::endl;
					std::cerr << "GtsMeshReader::nofEdges = " << nofEdges << std::endl;
					std::cerr << "GtsMeshReader::nofFaces = " << nofFaces << std::endl;

					mesh.Clear();
					VertexIterator v_iter = Allocator<MESH_TYPE>::AddVertices(mesh, nofVertices);
					for(int i=0 ; i<nofVertices ;)
					{
						if (cb && (i%1000)==0)
							cb(i/total, "Vertex Loading");

						line = stream.readLine().trimmed();

						if (line.startsWith("#"))
						{
							continue;
						}

						sa = line.split(' ');
						if (!sa.size()>=3)
						{
							std::cerr << "Error parsing vertex " << line.toLocal8Bit().data() << "\n";
							return InvalidFile;
						}
						v.X() = sa[0].toDouble();
						v.Y() = sa[1].toDouble();
						v.Z() = sa[2].toDouble();

						(*v_iter).P() = v;
						++v_iter;
						i++;
					}

					// load edges
					std::vector< std::pair<int,int> > edges;
					for(int i=0 ; i<nofEdges ; ++i)
					{
						if (cb && (i%1000)==0)
							cb((i+nofVertices)/total, "Edge Loading");

						line = stream.readLine();
						if (line.startsWith("#"))
						{
							continue;
						}

						sa = line.split(' ');
						if (!sa.size()>=2)
						{
							std::cerr << "Error parsing edge " << line.toLocal8Bit().data() << "\n";
							return InvalidFile;
						}
						id0 = sa[0].toInt();
						id1 = sa[1].toInt();

						edges.push_back(std::pair<int,int>(id0-1,id1-1));
					}

					// load faces
					Allocator<MESH_TYPE>::AddFaces(mesh, nofFaces);
					for(int i=0 ; i<nofFaces ; )
					{
						if (cb && (i%1000)==0)
							cb((nofVertices+nofEdges+i)/total, "Face Loading");

						line = stream.readLine();
						if (line.startsWith("#"))
						{
							continue;
						}

						sa = line.split(' ');
						if (!sa.size()>=3)
						{
							std::cerr << "Error parsing face " << line.toLocal8Bit().data() << "\n";
							return InvalidFile;
						}
						eid0 = sa[0].toInt();
						eid1 = sa[1].toInt();
						eid2 = sa[2].toInt();

						int e11 = edges[eid0-1].first;
						int e12 = edges[eid0-1].second;
						int e21 = edges[eid1-1].first;
						int e22 = edges[eid1-1].second;
						if (e12 == e21) {
							id0 = e11;
							id1 = e12;
							id2 = e22;
						}
						else if (e12 == e22) {
							id0 = e11;
							id1 = e12;
							id2 = e21;
						}
						else if (e11 == e21) {
							id0 = e12;
							id1 = e11;
							id2 = e22;
						}
						else {
							id0 = e12;
							id1 = e11;
							id2 = e21;
						}

						if (options.flipFaces)
						{
							mesh.face[i].V(0) = &(mesh.vert[id0]);
							mesh.face[i].V(1) = &(mesh.vert[id2]);
							mesh.face[i].V(2) = &(mesh.vert[id1]);
						}
						else
						{
							mesh.face[i].V(0) = &(mesh.vert[id0]);
							mesh.face[i].V(1) = &(mesh.vert[id1]);
							mesh.face[i].V(2) = &(mesh.vert[id2]);
						}

						++i;
					}
					return NoError;
				} // end Open

    protected:

			};
			// /*! @} */
		} //namespace io
	}//namespace tri
} // namespace vcg

#endif //__VCGLIB_IMPORT_GTS
