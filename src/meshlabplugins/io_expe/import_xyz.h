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

/* This code has been adapted from the OFF importer and Gael's .pts loader from Expe */

#ifndef __VCGLIB_IMPORT_XYZ
#define __VCGLIB_IMPORT_XYZ

#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <QString>
#include <QFile>
#include <QTextStream>
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
This class encapsulate a filter for importing Expes's .pts point sets.
*/
template<class MESH_TYPE>
class ImporterXYZ
{
	public:

		typedef typename MESH_TYPE::VertexType			VertexType;
		typedef typename MESH_TYPE::VertexIterator	VertexIterator;
		typedef typename MESH_TYPE::VertexPointer		VertexPointer;
		typedef typename MESH_TYPE::CoordType				CoordType;
		typedef typename MESH_TYPE::ScalarType			ScalarType;

		enum ExpeCodes {NoError=0, CantOpen, InvalidFile, UnsupportedVersion};

		struct Options
		{
			Options()
				: onlyMaskFlag(false)
			{}
			bool onlyMaskFlag;
		};

		struct FileProperty
		{
			FileProperty(QByteArray _name, uint _size)
				: name(_name), size(_size)
			{}
			QByteArray name;		// name of the property
			uint size;					// size in byte (binary mode)
			bool hasProperty;		// true if the target mesh has the property
		};
		typedef std::vector<FileProperty> FileProperties;


		/*!
		*	Standard call for knowing the meaning of an error code
		* \param message_code	The code returned by <CODE>Open</CODE>
		*	\return							The string describing the error code
		*/
		static const char* ErrorMsg(int message_code)
		{
			static const char* error_msg[] =
			{
				"No errors", "Can't open file", "Invalid file", "Unsupported version"
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

			loadmask = 0;

      QString buf;
      QStringList line;

			if (options.onlyMaskFlag)
      {
        // check the first line
        line = (buf = stream.readLine(1024)).split(QRegExp("[ |\t]"));

        if (line.size()==6)
        {
          loadmask |= Mask::IOM_VERTCOORD;
          loadmask |= Mask::IOM_VERTNORMAL;
        }
        else if (line.size()==3)
        {
          loadmask |= Mask::IOM_VERTCOORD;
        }
				return 0;
      }

      std::vector<CoordType> pos,normals;
      CoordType p,n;
      while(!stream.atEnd())
      {
        line = (buf = stream.readLine(1024)).split(QRegExp("[ |\t]"));
        if (line.size()==6)
        {
          loadmask |= Mask::IOM_VERTCOORD;
          loadmask |= Mask::IOM_VERTNORMAL;
          for (int k=0; k<3; ++k)
          {
            p[k] = line[k].toDouble();
            n[k] = line[3+k].toDouble();
          }
          pos.push_back(p);
          normals.push_back(n);
        }
        else if (line.size()==3)
        {
          loadmask |= Mask::IOM_VERTCOORD;
          // there is no normal information
          for (int k=0; k<3; ++k)
          {
            p[k] = line[k].toDouble();
            n[k] = 0;
          }
          pos.push_back(p);
          normals.push_back(n);
        }
        else
        {
          std::cerr << "error: skip line " << buf.toAscii().data() << "\n";
          for (int i=0; i<line.size(); ++i)
            std::cerr << line[i].toAscii().data() << " $ ";
          std::cerr << "\n";
        }
      }
      VertexIterator v_iter = Allocator<MESH_TYPE>::AddVertices(mesh,pos.size());
      for (int i=0; i<pos.size(); ++i)
      {
        v_iter->P() = pos[i];
        v_iter->N() = normals[i];
        ++v_iter;
      }

			return 0;
		} // end Open

	protected:

};
// /*! @} */

} //namespace io
} //namespace tri
} // namespace vcg

#endif //__VCGLIB_IMPORT_EXPE
