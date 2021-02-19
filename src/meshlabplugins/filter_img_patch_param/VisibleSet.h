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

#ifndef FILTER_IMG_PATCH_PARAM_PLUGIN__VISIBLESET_H
#define FILTER_IMG_PATCH_PARAM_PLUGIN__VISIBLESET_H




#include <common/ml_document/raster_model.h>
#include <common/ml_shared_data_context/ml_shared_data_context.h>
#include <wrap/glw/glw.h>


class VisibleSet
{
public:
    enum WeightMask
    {
        W_ORIENTATION   = 0x01,
        W_DISTANCE      = 0x02,
        W_IMG_BORDER    = 0x04,
        W_IMG_ALPHA     = 0x08,
    };


    class FaceVisInfo
    {
    private:
        float                       m_RefWeight;
        RasterModel                 *m_Ref;
        std::vector<RasterModel*>   m_Visible;

    public:
        inline  FaceVisInfo() : m_RefWeight(-std::numeric_limits<float>::max()), m_Ref(NULL) {}

        inline void         add( float weight, RasterModel *rm )
        {
            m_Visible.push_back( rm );
            if( weight > m_RefWeight )
            {
                m_RefWeight = weight;
                m_Ref = rm;
            }
        }
        inline bool         contains( RasterModel *rm ) const   { return std::find( m_Visible.begin(), m_Visible.end(), rm ) != m_Visible.end(); }
        inline RasterModel* ref() const                         { return m_Ref; }
        inline void         setRef( RasterModel *rm )           { m_Ref = rm; }
    };


private:
    const CMeshO        &m_Mesh;
    std::vector<FaceVisInfo>   m_FaceVis;
    int                 m_WeightMask;
    float               m_DepthMax;
    float               m_DepthRangeInv;

    inline int          id( const CFaceO& f ) const                         { return &f - &m_Mesh.face[0]; }

public:
    VisibleSet( glw::Context &ctx,MLPluginGLContext* plugctx,int meshid,
                CMeshO &mesh,
                QList<RasterModel*> &rasterList,
                int weightMask );

    float               getWeight( const RasterModel *rm, CFaceO &f );

    inline const FaceVisInfo&  operator[]( const int f ) const                     { return m_FaceVis[f]; }
    inline       FaceVisInfo&  operator[]( const int f )                           { return m_FaceVis[f]; }
    inline const FaceVisInfo&  operator[]( const CFaceO& f ) const                 { return m_FaceVis[id(f)]; }
    inline       FaceVisInfo&  operator[]( const CFaceO& f )                       { return m_FaceVis[id(f)]; }
    inline const FaceVisInfo&  operator[]( const CFaceO* f ) const                 { return m_FaceVis[id(*f)]; }
    inline       FaceVisInfo&  operator[]( const CFaceO* f )                       { return m_FaceVis[id(*f)]; }
    inline const FaceVisInfo&  operator[]( const CMeshO::FaceIterator& f ) const   { return m_FaceVis[id(*f)]; }
    inline       FaceVisInfo&  operator[]( const CMeshO::FaceIterator& f )         { return m_FaceVis[id(*f)]; }
};




#endif // FILTER_IMG_PATCH_PARAM_PLUGIN__VISIBLESET_H
