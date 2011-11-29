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

#ifndef FILTER_IMG_PATCH_PARAM_PLUGIN__DOMINANCYCLASSIFIER_H
#define FILTER_IMG_PATCH_PARAM_PLUGIN__DOMINANCYCLASSIFIER_H




#include <common/meshmodel.h>
#include "GPU/GPU.h"
#include "OOCRaster.h"


typedef QVector<CFaceO*> FaceVec;
typedef QMap<OOCRaster*,FaceVec> RasterFaceMap;


class DominancyClassifier
{
public:
    enum WeightMask
    {
        W_ORIENTATION   = 0x01,
        W_DISTANCE      = 0x02,
        W_IMG_BORDER    = 0x04,
        W_SILHOUETTE    = 0x08,
        W_ALL           = 0x0F,
    };

    enum FaceDomMode
    {
        FD_STRICT       ,
        FD_AGRESSIVE    ,
    };

    struct VDominancy
    {
        float       weight;
        OOCRaster   *dominant;

        inline VDominancy() :
        weight(-1.0f),
        dominant(NULL)
        {}
    };

private:
    typedef GPU::VBO<GPU::InterlacedBuffers,
                     GPU::Vertex3f         ,
                     GPU::Normal3f         ,
                     GPU::TexCoord1i       ,
                     GPU::Indexui          > MyVBO;

    typedef std::vector<VDominancy> VertDomVector;



    CMeshO                      &m_Mesh;
    QList<OOCRaster>            &m_RasterList;

    MyVBO                       m_MeshVBO;

    int                         m_WeightMask;
    GPU::Shader                 m_WeightShader;
    GPU::Shader                 m_VisCheckShader;

    float                       m_DepthMin;
    float                       m_DepthMax;

    GPU::Texture2D              m_WeightMap;
    GPU::Texture2D              m_WeightMapSilh;
    GPU::Texture2D              m_ShadowMap;
    vcg::Matrix44f              m_Proj;
    vcg::Matrix44f              m_Pose;
    vcg::Matrix44f              m_TexProj;

    VertDomVector               m_VertexDom;


    void                        updateDepthRange();
    void                        updateMeshVBO();
    bool                        initShaders();
    void                        generateWeightsAndShadowMap();
    void                        projectiveTexMatrices( OOCRaster &rr );
    void                        setupShadowTexture( OOCRaster &rr );
    void                        checkDominancy( OOCRaster &rr );
    void                        releaseAll();

    inline int                  id( const CVertexO& v ) const                           { return &v - &m_Mesh.vert[0]; }

public:
    DominancyClassifier( CMeshO &mesh, QList<OOCRaster> &rasterList, int weightMask );

    inline const VDominancy&    operator[]( const int v ) const                     { return m_VertexDom[v];      }
    inline VDominancy&          operator[]( const int v )                           { return m_VertexDom[v];      }
    inline const VDominancy&    operator[]( const CVertexO& v ) const               { return m_VertexDom[id(v)];  }
    inline VDominancy&          operator[]( const CVertexO& v )                     { return m_VertexDom[id(v)];  }
    inline const VDominancy&    operator[]( const CVertexO* v ) const               { return m_VertexDom[id(*v)]; }
    inline VDominancy&          operator[]( const CVertexO* v )                     { return m_VertexDom[id(*v)]; }
    inline const VDominancy&    operator[]( const CMeshO::VertexIterator& v ) const { return m_VertexDom[id(*v)]; }
    inline VDominancy&          operator[]( const CMeshO::VertexIterator& v )       { return m_VertexDom[id(*v)]; }

    void                        facesWithDominant( FaceVec &faces,
                                                   const OOCRaster *rr,
                                                   FaceDomMode mode = FD_AGRESSIVE ) const;

    void                        dominancyCoverage( RasterFaceMap &faces,
                                                   FaceDomMode mode = FD_AGRESSIVE ) const;
};




#endif // FILTER_IMG_PATCH_PARAM_PLUGIN__DOMINANCYCLASSIFIER_H
