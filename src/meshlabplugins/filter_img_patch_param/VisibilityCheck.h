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

#ifndef FILTER_IMG_PATCH_PARAM_PLUGIN__VISIBILITYCHECK_H
#define FILTER_IMG_PATCH_PARAM_PLUGIN__VISIBILITYCHECK_H




#include <common/meshmodel.h>
#include <wrap/glw/glw.h>

#define USE_VBO




class VisibilityCheck
{
protected:
    enum VMarker
    {
        V_UNDEFINED ,
        V_BACKFACE  ,
        V_VISIBLE   ,
    };

    glw::Context                &m_Context;
    CMeshO                      *m_Mesh;
    RasterModel                 *m_Raster;
    std::vector<unsigned char>  m_VertFlag;

    static VisibilityCheck      *s_Instance;

    inline                  VisibilityCheck( glw::Context &ctx ) : m_Context(ctx), m_Mesh(NULL), m_Raster(NULL) {}
    virtual                 ~VisibilityCheck()                                                                  {}

public:
    static VisibilityCheck* GetInstance( glw::Context &ctx );
    static void             ReleaseInstance();

    virtual void            setMesh( CMeshO *mesh )                                 = 0;
    virtual void            setRaster( RasterModel *mesh )                          = 0;
    virtual void            checkVisibility()                                       = 0;

    inline bool             isVertVisible( const unsigned int n ) const             { return m_VertFlag[n]==V_VISIBLE; }
    inline bool             isVertVisible( const CVertexO *v ) const                { return isVertVisible( v - &m_Mesh->vert[0] ); }
    inline bool             isVertVisible( const CMeshO::VertexIterator &v ) const  { return isVertVisible( &*v ); }

    inline bool             isFaceVisible( const unsigned int n ) const             { return isFaceVisible( &m_Mesh->face[n] ); }
    inline bool             isFaceVisible( const CFaceO *f ) const                  { return isVertVisible(f->V(0)) || isVertVisible(f->V(1)) || isVertVisible(f->V(2)); }
    inline bool             isFaceVisible( const CMeshO::FaceIterator &f ) const    { return isFaceVisible( &*f ); }
};


class VisibilityCheck_VMV2002 : public VisibilityCheck
{
    friend class VisibilityCheck;

private:
	glw::RenderbufferHandle m_ColorRB;
	glw::RenderbufferHandle m_DepthRB;
	glw::FramebufferHandle  m_FrameBuffer;

    vcg::Point2i            m_ViewportMin;
    vcg::Point2i            m_ViewportMax;

    void        init( std::vector<unsigned char> &visBuffer );
    bool        iteration( std::vector<unsigned char> &visBuffer );
    void        release();

    inline      VisibilityCheck_VMV2002( glw::Context &ctx ) : VisibilityCheck(ctx) {}
    inline      ~VisibilityCheck_VMV2002()                                          {}

public:
    static bool isSupported();

    void        setMesh( CMeshO *mesh )                                             { m_Mesh = mesh; }
    void        setRaster( RasterModel *rm )                                        { m_Raster = rm; }
    void        checkVisibility();
};


class VisibilityCheck_ShadowMap : public VisibilityCheck
{
    friend class VisibilityCheck;

private:
    vcg::Matrix44f          m_Pose;
    vcg::Matrix44f          m_Proj;
    vcg::Matrix44f          m_ShadowProj;
	glw::Texture2DHandle    m_ShadowMap;

    glw::Texture2DHandle    m_VertexMap;
    glw::Texture2DHandle    m_NormalMap;
    glw::RenderbufferHandle m_ColorBuffer;
    glw::FramebufferHandle  m_FBuffer;

    glw::ProgramHandle      m_VisDetectionShader;

    static bool             s_AreVBOSupported;
    glw::BufferHandle       m_ShadowVBOVertices;
    glw::BufferHandle       m_ShadowVBOIndices;

    void        shadowProjMatrices();
    void        setupShadowTexture();
    void        updateShadowTexture();
    void        initMeshTextures();
    bool        initShaders();

                VisibilityCheck_ShadowMap( glw::Context &ctx );
    inline      ~VisibilityCheck_ShadowMap()    {}

public:
    static bool isSupported();

    void        setMesh( CMeshO *mesh );
    void        setRaster( RasterModel *rm );
    void        checkVisibility();
};




#endif // FILTER_IMG_PATCH_PARAM_PLUGIN__VISIBILITYCHECK_H
