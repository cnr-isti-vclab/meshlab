/****************************************************************************
* MeshLab                                                           o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#ifndef MESHMODEL_H
#define MESHMODEL_H
#include <GL/glew.h>

#include <stdio.h>
#include <time.h>
#include <map>

#include "cmesh.h"

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/position.h>
#include <vcg/complex/algorithms/update/quality.h>
#include <vcg/complex/algorithms/update/selection.h>
#include <vcg/complex/algorithms/update/topology.h>


//#include <wrap/gl/trimesh.h>



#include <wrap/callback.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/additionalinfo.h>

#include <QList>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QReadWriteLock>
#include <QImage>
#include <QAction>
#include "../GLLogStream.h"
#include "../filterscript.h"
#include "../ml_shared_data_context.h"

/*
MeshModel Class
The base class for representing a single mesh.
It contains a single vcg mesh object with some additional information for keeping track of its origin and of what info it has.
*/

class MeshDocument;

class MeshModel
{
public:
    /*
    This enum specify the various simplex components
    It is used in various parts of the framework:
    - to know what elements are currently active and therefore can be saved on a file
    - to know what elements are required by a filter and therefore should be made ready before starting the filter (e.g. if a
    - to know what elements are changed by a filter and therefore should be saved/restored in case of dynamic filters with a preview
    */
    enum MeshElement{
        MM_NONE             = 0x00000000,
        MM_VERTCOORD        = 0x00000001,
        MM_VERTNORMAL       = 0x00000002,
        MM_VERTFLAG         = 0x00000004,
        MM_VERTCOLOR        = 0x00000008,
        MM_VERTQUALITY      = 0x00000010,
        MM_VERTMARK	        = 0x00000020,
        MM_VERTFACETOPO     = 0x00000040,
        MM_VERTCURV	        = 0x00000080,
        MM_VERTCURVDIR      = 0x00000100,
        MM_VERTRADIUS       = 0x00000200,
        MM_VERTTEXCOORD     = 0x00000400,
        MM_VERTNUMBER       = 0x00000800,

        MM_FACEVERT         = 0x00001000,
        MM_FACENORMAL       = 0x00002000,
        MM_FACEFLAG	        = 0x00004000,
        MM_FACECOLOR        = 0x00008000,
        MM_FACEQUALITY      = 0x00010000,
        MM_FACEMARK	        = 0x00020000,
        MM_FACEFACETOPO     = 0x00040000,
        MM_FACENUMBER       = 0x00080000,
        MM_FACECURVDIR      = 0x00100000,

        MM_WEDGTEXCOORD     = 0x00200000,
        MM_WEDGNORMAL       = 0x00400000,
        MM_WEDGCOLOR        = 0x00800000,

        // 	Selection
        MM_VERTFLAGSELECT   = 0x01000000,
        MM_FACEFLAGSELECT   = 0x02000000,

        // Per Mesh Stuff....
        MM_CAMERA			= 0x08000000,
        MM_TRANSFMATRIX     = 0x10000000,
        MM_COLOR            = 0x20000000,
        MM_POLYGONAL        = 0x40000000,

		// unknown - will raise exceptions, to be avoided, here just for compatibility
        MM_UNKNOWN          = 0x80000000,

		// geometry change (for filters that remove stuff or modify geometry or topology, but not touch face/vertex color or face/vertex quality)
		MM_GEOMETRY_AND_TOPOLOGY_CHANGE  = 0x431e7be7,

		// everything - dangerous, will add unwanted data to layer (e.g. if you use MM_ALL it could means that it could add even color or quality)
        MM_ALL				= 0xffffffff
    };

    MeshModel(MeshDocument *parent, unsigned int id, const QString& fullFileName, const QString& labelName);
	//MeshModel(MeshModel* cp);
    ~MeshModel()
    {
    }

	MeshDocument *parent;

    CMeshO cm;




    /*vcg::GlTrimesh<CMeshO> glw;*/
    
    
    
    /*
    Bitmask denoting what fields are currently used in the mesh
    it is composed by MeshElement enums.
    it should be changed by only mean the following functions:

    updateDataMask(neededStuff)
    clearDataMask(no_needed_stuff)
    hasDataMask(stuff)

    Note that if an element is active means that is also allocated
    Some unactive elements (vertex color) are usually already allocated
    other elements (FFAdj or curvature data) not necessarily.

    */

private:
    int currentDataMask;
    QString fullPathFileName;
    QString _label;
    unsigned int _id;
    bool modified;

public:
    void Clear();
    void UpdateBoxAndNormals(); // This is the STANDARD method that you should call after changing coords.
    inline int id() const {return _id;}
    
    
    // Some notes about the files and naming.
    // Each mesh when shown in the layer dialog has a label.
    // By default the label is just the name of the file, but the

    // in a future the path should be moved outside the meshmodel into the meshdocument (and assume that all the meshes resides in a common subtree)
    // currently we just fix the interface and make the pathname private for avoiding future hassles.

    QString label() const { if(_label.isEmpty()) return shortName(); else return _label;}

    /// The whole full path name of the mesh
    QString fullName() const {return fullPathFileName;}

    /// just the name of the file
    QString shortName() const { return QFileInfo(fullPathFileName).fileName(); }

    /// the full path without the name of the file (e.g. the dir where the mesh and often its textures are)
    QString pathName() const {QFileInfo fi(fullName()); return fi.absolutePath();}

    /// just the extension.
    QString suffixName() const {QFileInfo fi(fullName()); return fi.suffix();}

    /// the relative path with respect to the current project
    QString relativePathName() const;

    /// the absolute path of the current project
    QString documentPathName() const;

    void setFileName(QString newFileName) {
        QFileInfo fi(newFileName);
        if(!fi.isAbsolute()) qWarning("Someone is trying to put a non relative filename");
        fullPathFileName = fi.absoluteFilePath();
    }
    void setLabel(QString newName) {_label=newName;}

    bool visible; // used in rendering; Needed for toggling on and off the meshes
	bool isVisible() const { return visible; }

    // This function is roughly equivalent to the updateDataMask,
    // but it takes in input a mask coming from a filetype instead of a filter requirement (like topology etc)
    void Enable(int openingFileMask);

    bool hasDataMask(const int maskToBeTested) const;
    void updateDataMask(MeshModel *m);
    void updateDataMask(int neededDataMask);
    void clearDataMask(int unneededDataMask);
    int dataMask() const;


	bool meshModified() const;
	void setMeshModified(bool b = true);
    static int io2mm(int single_iobit);
};// end class MeshModel





#endif
