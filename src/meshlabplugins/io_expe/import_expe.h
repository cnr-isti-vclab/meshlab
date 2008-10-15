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

#ifndef __VCGLIB_IMPORT_EXPE
#define __VCGLIB_IMPORT_EXPE

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

static const char * Header_EPSB01 = "[Expe/PointSet/Binary/0.1]";
static const char * Header_EPSB02 = "[Expe/PointSet/Binary/0.2]";
static const char * Header_EPSA02 = "[Expe/PointSet/Ascii/0.2]";

// /** \addtogroup  */
// /* @{ */
/**
This class encapsulate a filter for importing Expes's .pts point sets.
*/
template<class MESH_TYPE>
class ImporterExpePTS
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


		static uint getSizeOfPropertyType(const QByteArray& type)
		{
			uint size = 0;
			if(type=="r32" || type=="ui32" || type=="i32")
			{
				size = 4;
			}
			else if(type=="r16" || type=="ui16" || type=="i16")
			{
				size = 2;
			}
			else if(type=="ui8" || type=="i8")
			{
				size = 1;
			}
			return size;
		}

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

			// check the header file and version
			QString tmp;
			QByteArray propertyName, command;
			QByteArray dataFormat;
			int propertySize, nofPoints = -1;
			stream >> tmp;

			QStringList header = tmp.split(QRegExp("[\\[/\\]]"),QString::SkipEmptyParts);
			if(header.size()!=4)
				return InvalidFile;

			if(header[0] != "Expe")
				return InvalidFile;

			if(header[1] != "PointSet")
				return InvalidFile;

			if(header[2]!="Ascii" && header[2]!="Binary")
				return InvalidFile;

			if(header[3]!="0.1" && header[3]!="0.2")
				return UnsupportedVersion;

			loadmask = 0;
			FileProperties fileProperties;
			int pointSize = 0;

			stream >> command;
			while(command!="data")
			{
				if(command=="property")
				{
					if(header[3]=="0.1")
					{
						stream >> propertyName >> propertySize;
					}
					else if(header[3]=="0.2")
					{
						QByteArray propertyType;
						// name / nof atomic element / type of an atomic element
						stream >> propertyName >> propertySize >> propertyType;
						propertySize *= getSizeOfPropertyType(propertyType);
					}

					fileProperties.push_back(FileProperty(propertyName,propertySize));

					if(propertyName=="position")
					{
						loadmask |= Mask::IOM_VERTCOORD;
						fileProperties.back().hasProperty = true;
					}
					else if(propertyName=="radius")
					{
						loadmask |= Mask::IOM_VERTRADIUS;
            // FIXME check dynamically if the radius is enabled
 						fileProperties.back().hasProperty = VertexType::HasRadius();
					}
					else if(propertyName=="normal")
					{
						loadmask |= Mask::IOM_VERTNORMAL;
						fileProperties.back().hasProperty = tri::HasPerVertexNormal(mesh);
					}
					else if(propertyName=="color")
					{
						loadmask |= Mask::IOM_VERTCOLOR;
						fileProperties.back().hasProperty = tri::HasPerVertexColor(mesh);
					}
					else
					{
						// unsupported property
						fileProperties.back().hasProperty = false;
					}
					pointSize += propertySize;
				}
				else if(command=="nofpoints")
				{
					stream >> nofPoints;
				}
				else
				{
					std::cerr << "Unknow command " << command.data() << " (skip)" << "\n";
				}
				stream >> command;
			}

			if (options.onlyMaskFlag)
				return 0;

			if (pointSize<0)
				return InvalidFile;

			qint64 streamPos = stream.pos();
			stream.setDevice(0);
			device.reset();
			device.seek(streamPos);

			if(header[2]=="Binary")
				return appendBinaryData(mesh, nofPoints, fileProperties, pointSize, device);
			else if(header[2]=="Ascii")
				return appendAsciiData(mesh, nofPoints, fileProperties, device);

			return 0;
		} // end Open

		template<typename VectorType>
		static bool parse_vector(const QString& str, VectorType& v)
		{
			bool ok = true;
			QRegExp xtrimming("^.*([-\\d].*\\d).*$");
			xtrimming.indexIn(str);
			QString trimmed = xtrimming.cap(1);
			QList<QString> elements = trimmed.split(QRegExp("[ \t]+|[ \t]*,[ \t]*"));
			int expectedSize = v.size();
			if (elements.size() != expectedSize)
				return false;
			for (uint k=0 ; k<expectedSize && ok ; ++k)
				v[k] = elements[k].toDouble(&ok);
			return ok;
		}

		static int appendAsciiData(MESH_TYPE& mesh, uint nofPoints, const FileProperties& fileProperties, QIODevice& device)
		{
			QTextStream stream(&device);
			std::vector<double> v(3);
			std::vector<double> c(4);
			QStringList line;
			uint i=0;
			VertexIterator v_iter = Allocator<MESH_TYPE>::AddVertices(mesh, nofPoints);
			while(i<nofPoints && !stream.atEnd())
			{
				line = stream.readLine(1024).split(";");
				if (line.size()==int(fileProperties.size()))
				{
					for(uint k=0 ; k<fileProperties.size() ; ++k)
					{
						if (fileProperties[k].hasProperty)
						{
							if (fileProperties[k].name=="position")
							{
								if (!parse_vector(line[k], v))
								{
									std::cerr << "Error parsing position " << line[k].toLocal8Bit().data() << "\n";
									return InvalidFile;
								}
								for (int j=0; j<3; ++j)
									v_iter->P()[j] = v[j];
							}
							else if(fileProperties[k].name=="normal")
							{
								if (!parse_vector(line[k], v))
								{
									std::cerr << "Error parsing normal " << line[k].toLocal8Bit().data() << "\n";
									return InvalidFile;
								}
								for (int j=0; j<3; ++j)
									v_iter->N()[j] = v[j];
							}
							else if(fileProperties[k].name=="radius")
							{
								bool ok = true;
								v_iter->R() = line[k].toFloat(&ok);
								if (!ok)
								{
									std::cerr << "Error parsing radius " << line[k].toLocal8Bit().data() << "\n";
									return InvalidFile;
								}
							}
							else if(fileProperties[k].name=="color")
							{
								if (!parse_vector(line[k], c))
								{
									std::cerr << "Error parsing color " << line[k].toLocal8Bit().data() << "\n";
									return InvalidFile;
								}
								vcg::Color4f color(c[0],c[1],c[2],c[3]);
								(*v_iter).C().Import(color);
							}
							else
							{
								std::cerr << "unsupported property " << fileProperties[k].name.data() << "\n";
							}
						}
					}
					++v_iter;
				}
				else if(line.size()!=0)
				{
					std::cerr << "\tInvalid line : " << line[0].toLocal8Bit().data() << "... (skip)\n";
				}
			}
			return 0;
		}

		static int appendBinaryData(MESH_TYPE& mesh, uint nofPoints, const FileProperties& fileProperties,
																int pointSize, QIODevice& device)
		{
			QDataStream stream(&device);
			std::vector<unsigned char> data(pointSize);

			// skip \n
			stream.skipRawData(1);

			std::vector<float> v(3);
			VertexIterator v_iter = Allocator<MESH_TYPE>::AddVertices(mesh, nofPoints);
			for(uint i=0 ; i<nofPoints ; ++i)
			{
				stream.readRawData(reinterpret_cast<char*>(&data[0]), pointSize);
				int b = 0; // offset in byte
				for(uint k=0 ; k<fileProperties.size() ; ++k)
				{
					if (fileProperties[k].hasProperty)
					{
						if (fileProperties[k].name=="position")
						{
							float* p = reinterpret_cast<float*>(&data[b]);
							for (int j=0; j<3; ++j)
								v_iter->P()[j] = p[j];
						}
						else if(fileProperties[k].name=="normal")
						{
							float* n = reinterpret_cast<float*>(&data[b]);
							for (int j=0; j<3; ++j)
								v_iter->N()[j] = n[j];
						}
						else if(fileProperties[k].name=="radius")
						{
							v_iter->R() = *reinterpret_cast<float*>(&data[b]);
						}
						else if(fileProperties[k].name=="color")
						{
							vcg::Color4b color(data[b],data[b+1],data[b+2],data[b+3]);
							(*v_iter).C().Import(color);
						}
						else
						{
							std::cerr << "unsupported property " << fileProperties[k].name.data() << "\n";
						}
					}
					b += fileProperties[k].size;
				}
				++v_iter;
			}
			return 0;
		}

	protected:

};
// /*! @} */

} //namespace io
} //namespace tri
} // namespace vcg

#endif //__VCGLIB_IMPORT_EXPE
