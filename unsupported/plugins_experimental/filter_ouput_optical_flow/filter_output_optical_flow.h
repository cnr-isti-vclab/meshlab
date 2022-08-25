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

#ifndef FILTER_OUTPUT_OPTICAL_FLOW_PLUGIN_H
#define FILTER_OUTPUT_OPTICAL_FLOW_PLUGIN_H




#include <QObject>
#include <common/interfaces.h>
#include "GPU/GPU.h"
#include "DominancyClassifier.h"
#include "OOCRaster.h"




class FilterOutputOpticalFlowPlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
	Q_INTERFACES( MeshFilterInterface )

	enum
    {
        FP_OUTPUT_OPTICAL_FLOW  ,
    };

    typedef std::set<CFaceO*> NeighbSet;
    typedef GPU::VBO< GPU::DistinctBuffers,
                      GPU::Vertex3f       ,
                      GPU::Indexui        > MyVBO;


    void                getNeighbors( CVertexO *v,
                                      NeighbSet &neighb ) const;
    void                getNeighbors( CFaceO *f,
                                      NeighbSet &neighb ) const;
    void                expands( Patch &patch,
                                 int nbGrows );

    void                setupMeshVBO();
    void                setupShader( GPU::Shader &shader );
    void                shadowTextureMatrices( OOCRaster *rr,
                                               vcg::Matrix44f &proj,
                                               vcg::Matrix44f &pose,
                                               vcg::Matrix44f &shadowProj );
    void                setupShadowAndColorTextures( GPU::Texture2D &shadowMap,
                                                     GPU::Texture2D &colorMap,
                                                     OOCRaster *rr );
    void                paintShadowTexture( GPU::Texture2D &shadowMap,
                                            vcg::Matrix44f &proj,
                                            vcg::Matrix44f &pose );
    void                retroProjection( RasterFaceMap &rpatches,
                                         float coverageThreshold,
                                         QMap<int,QVector<int>> &validPairs );
    bool                loadRasterList( QString &filename,
                                        QList<OOCRaster> &rasters );

    void                saveXMLProject( const QString &filename,
                                        MeshModel *mm,
                                        RasterFaceMap &rpatches,
                                        QMap<int,QVector<int>> &validPairs );


    CMeshO      *m_Mesh;
    MyVBO       m_MeshVBO;


public:
	FilterOutputOpticalFlowPlugin();

	virtual QString     filterName( FilterIDType id ) const;
	virtual QString     filterInfo( FilterIDType id ) const;

	virtual FilterClass getClass( QAction *act );

    virtual void        initParameterSet( QAction *act,
                                          MeshDocument &md,
                                          RichParameterSet &par );

    virtual int         getRequirements( QAction *act );

    virtual bool        applyFilter( QAction *act,
                                     MeshDocument &md,
                                     RichParameterSet &par,
                                     vcg::CallBackPos *cb );
};




#endif // FILTER_OUTPUT_OPTICAL_FLOW_PLUGIN_H
