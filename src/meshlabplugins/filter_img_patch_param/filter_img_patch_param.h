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

#ifndef FILTER_IMG_PATCH_PARAM_PLUGIN_H
#define FILTER_IMG_PATCH_PARAM_PLUGIN_H




#include <QObject>
#include <common/interfaces.h>
#include <vcg/math/similarity2.h>
#include "Patch.h"
#include <wrap/glw/glw.h>
class VisibleSet;


class FilterImgPatchParamPlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
    Q_INTERFACES( MeshFilterInterface )

    enum
    {
        FP_PATCH_PARAM_ONLY             ,
        FP_PATCH_PARAM_AND_TEXTURING    ,
        FP_RASTER_VERT_COVERAGE         ,
        FP_RASTER_FACE_COVERAGE         ,
    };

    typedef std::set<CFaceO*> NeighbSet;
    glw::Context        *m_Context;


    void                getNeighbors( CVertexO *v,
                                      NeighbSet &neighb ) const;
    void                getFaceNeighbors( CFaceO *f,
                                      NeighbSet &neighb ) const;

    void                boundaryOptimization( CMeshO &mesh,
                                              VisibleSet &faceVis,
                                              bool mostFrontFacing );

    int                 cleanIsolatedTriangles( CMeshO &mesh,
                                                VisibleSet &faceVis );

    int                 extractPatches( RasterPatchMap &patches,
                                        PatchVec &nullPatches,
                                        CMeshO &mesh,
                                        VisibleSet &faceVis,
                                        QList<RasterModel*> &rasterList );

    void                constructPatchBoundary( Patch &p,
                                                VisibleSet &faceVis );

    void                computePatchUV( CMeshO &mesh,
                                        RasterModel *rm,
                                        PatchVec &patches );

    void                mergeOverlappingPatches( PatchVec &patches );

    void                patchPacking( RasterPatchMap &patches,
                                      int textureGutter,
                                      bool allowUVStretching );

    void                patchBasedTextureParameterization( RasterPatchMap &patches,
                                                           PatchVec &nullPatches,
                                                           CMeshO &mesh,
                                                           QList<RasterModel*> &rasterList,
                                                           RichParameterSet &par );

    float               computeTotalPatchArea( RasterPatchMap &patches );
    int                 computePatchCount( RasterPatchMap &patches );

public:
    FilterImgPatchParamPlugin();
    ~FilterImgPatchParamPlugin();

    virtual QString     filterName( FilterIDType id ) const;
    virtual QString     filterInfo( FilterIDType id ) const;

    virtual FilterClass getClass( QAction *act );

    virtual void        initParameterSet( QAction *act,
                                          MeshDocument &md,
                                          RichParameterSet &par );

    virtual int         getRequirements( QAction *act );
    //virtual int         postCondition( QAction *act ) const;

    virtual bool        applyFilter( QAction *act,
                                     MeshDocument &md,
                                     RichParameterSet &par,
                                     vcg::CallBackPos *cb );
};




#endif // FILTER_IMG_PATCH_PARAM_PLUGIN_H
