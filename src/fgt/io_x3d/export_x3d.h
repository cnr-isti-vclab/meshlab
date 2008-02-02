/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
 Revision 1.1  2008/02/02 13:52:54  gianpaolopalma
 Defined X3D exporter interface

 
 *****************************************************************************/
#ifndef EXPORTERX3D
#define EXPORTERX3D

#include<util_x3d.h>

namespace vcg {
namespace tri {
namespace io {

	template<typename SaveMeshType>
	class ExporterX3D : public UtilX3D
	{
	private:

	public:

		static int Save(SaveMeshType &m, const char * filename,AdditionalInfo*& in, const int mask)
		{
			return E_NOERROR;	
		}


		static int GetExportMaskCapability()
		{
			int capability = 0;
			//vert
			capability |= MeshModel::IOM_VERTNORMAL;
			capability |= MeshModel::IOM_VERTTEXCOORD;
			capability |= MeshModel::IOM_VERTCOLOR;
			////face
			capability |= MeshModel::IOM_FACECOLOR;
			capability |= MeshModel::IOM_FACENORMAL;
			////wedg
			capability |= MeshModel::IOM_WEDGTEXCOORD;
			capability |= MeshModel::IOM_WEDGNORMAL;
			capability |= MeshModel::IOM_WEDGCOLOR;
			//capability |= MeshModel::IOM_WEDGTEXMULTI;

			return capability;
		}
	};
}
}
}

#endif
