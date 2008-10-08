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

/* This code has been adapted from the GTS exporter and Gael's GTS exporter from Expe */

/**
@name Save in GTS format
*/
//@{

#ifndef __VCGLIB_EXPORT_GTS
#define __VCGLIB_EXPORT_GTS

#include <QFile>
#include <QTextStream>
#include <map>
#include <wrap/io_trimesh/io_mask.h>


namespace vcg {
	namespace tri {
		namespace io {
			template <class SaveMeshType>
			class ExporterGTS
			{

			public:
				typedef typename SaveMeshType::VertexPointer VertexPointer;
				typedef typename SaveMeshType::ScalarType ScalarType;
				typedef typename SaveMeshType::VertexType VertexType;
				typedef typename SaveMeshType::FaceType FaceType;
				typedef typename SaveMeshType::FacePointer FacePointer;
				typedef typename SaveMeshType::VertexIterator VertexIterator;
				typedef typename SaveMeshType::FaceIterator FaceIterator;

				static int Save(SaveMeshType &m, const char * filename, int mask=0 )
				{
					QFile device(filename);
					if (!device.open(QFile::WriteOnly))
						return 1;

					QTextStream stream(&device);

					// update vertex indices
					std::vector<int> FlagV;
					VertexPointer  vp;
					VertexIterator vi;
					int j;
					for(j=0,vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
					{
						vp = &(*vi);
						FlagV.push_back(vp->UberFlags());
						if (!vp->IsD())
						{
							vp->UberFlags() = j;
							j++;
						}
					}
					assert(j==m.vn);

					// build the edge list, and count the number of edges
					typedef std::pair<int,int> Edge;
					typedef std::map<Edge,int> Edge2Index;
					Edge2Index edge2index;

					// loop over all edge's faces
					FacePointer fp;
					int edgeCount = 0;
					FaceIterator fi;
					for (j=0,fi=m.face.begin(); fi!=m.face.end(); ++fi)
					{
						fp = &(*fi);
						if (!fp->IsD() )
						{
							for (int k=0; k<3; ++k)
							{
								int a = fp->cV(k)->UberFlags();
								int b = fp->cV((k+1)%3)->UberFlags();
								if (a>b)
									std::swap(a,b);
								Edge e(a,b);
								Edge2Index::iterator it = edge2index.find(e);
								if (it==edge2index.end())
								{
									edge2index[e] = edgeCount;
									edgeCount++;
								}
							}
						}
					}

					stream  << m.vn << " "
									<< edgeCount << " "
									<< m.fn << " "
									<< "GtsSurface GtsFace GtsEdge GtsVertex\n";

					// export vertices
					for (vi=m.vert.begin();vi!=m.vert.end();++vi)
					{
						vp=&(*vi);
						if (!vp->IsD())
						{
							stream << vp->P()[0] << " " << vp->P()[1] << " " << vp->P()[2] << "\n";
						}
					}

					// export the edges
					for (j=0,fi=m.face.begin(); fi!=m.face.end(); ++fi)
					{
						fp = &(*fi);
						if (!fp->IsD() )
						{
							for (int k=0; k<3; ++k)
							{
								int a = fp->cV(k)->UberFlags();
								int b = fp->cV((k+1)%3)->UberFlags();
								if (a>b)
									std::swap(a,b);
								Edge e(a,b);
								Edge2Index::iterator it = edge2index.find(e);
								if (it!=edge2index.end())
								{
									stream << a+1 << " " << b+1 << "\n";
								}
							}
						}
					}

					// third pass to export the face to edge connectivity
					for (j=0,fi=m.face.begin(); fi!=m.face.end(); ++fi)
					{
						fp = &(*fi);
						if (!fp->IsD() )
						{
							for (int k=0; k<3; ++k)
							{
								int a = fp->cV(k)->UberFlags();
								int b = fp->cV((k+1)%3)->UberFlags();
								if (a>b)
									std::swap(a,b);
								Edge e(a,b);
								Edge2Index::iterator it = edge2index.find(e);
								if (it!=edge2index.end())
									stream << it->second+1 << " ";
								else
									return 2; // internal error
							}
							stream << "\n";
						}
					}

					// Recupera i flag originali
					for(j=0,vi=m.vert.begin();vi!=m.vert.end();++vi)
						(*vi).UberFlags()=FlagV[j++];

					return 0;
				}

        static const char *ErrorMsg(int error)
        {
          static std::vector<std::string> off_error_msg;
          if(off_error_msg.empty())
          {
            off_error_msg.resize(2 );
            off_error_msg[0]="No errors";
	          off_error_msg[1]="Can't open file";
						off_error_msg[2]="Internal error";
					}

          if(error>2 || error<0) return "Unknown error";
          else return off_error_msg[error].c_str();
        }
        /*
	        returns mask of capability one define with what are the saveable information of the format.
        */
        static int GetExportMaskCapability()
        {
	        int capability = 0;
	        capability |= vcg::tri::io::Mask::IOM_VERTCOORD;
          capability |= vcg::tri::io::Mask::IOM_FACEINDEX;
	        return capability;
        }

			}; // end class
		} // end namespace tri
	} // end namespace io
} // end namespace vcg
//@}
#endif
