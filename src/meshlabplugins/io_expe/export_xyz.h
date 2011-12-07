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

/**
@name Save in XYZ format
*/
//@{

#ifndef __VCGLIB_EXPORT_XYZ
#define __VCGLIB_EXPORT_XYZ

#include <stdio.h>
#include <wrap/callback.h>
#include <wrap/io_trimesh/io_mask.h>

namespace vcg {
namespace tri {
namespace io {

template <class SaveMeshType>
class ExporterXYZ
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
    bool savenormals = false;

    vcg::face::Pos<FaceType> he;
    vcg::face::Pos<FaceType> hei;

    FILE * fpout = fopen(filename,"w");
		if(fpout==NULL)	return 1; // 1 is the error code for cant'open, see the ErrorMsg function


    if(mask & io::Mask::IOM_VERTNORMAL)
      savenormals = true;
				
		//vertices
		VertexPointer  vp;
		VertexIterator vi;
		for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
		{
		  vp=&(*vi);

      if( ! vp->IsD() )
      {
        fprintf(fpout,"%f %f %f " ,vp->P()[0],vp->P()[1],vp->P()[2]);
        if(savenormals)
          fprintf(fpout,"%f %f %f " ,vp->N()[0],vp->N()[1],vp->N()[2]);

        fprintf(fpout,"\n");
      }
    }

    fclose(fpout);

    return 0;
  }

  static const char *ErrorMsg(int error)
  {
    static std::vector<std::string> xyz_error_msg;
    if(xyz_error_msg.empty())
    {
      xyz_error_msg.resize(2 );
      xyz_error_msg[0]="No errors";
      xyz_error_msg[1]="Can't open file";
    }

    if(error>1 || error<0) return "Unknown error";
    else return xyz_error_msg[error].c_str();
  }
    
  /*
  returns mask of capability one define with what are the saveable information of the format.
  */
  static int GetExportMaskCapability()
  {
    int capability = 0;			
    capability |= vcg::tri::io::Mask::IOM_VERTCOORD;
    capability |= vcg::tri::io::Mask::IOM_VERTNORMAL;
    return capability;
  }

}; // end class
} // end namespace tri
} // end namespace io
} // end namespace vcg
//@}
#endif
