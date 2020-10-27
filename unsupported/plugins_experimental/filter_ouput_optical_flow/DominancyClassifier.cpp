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

#include "DominancyClassifier.h"
#include <wrap/gl/shot.h>
#include <common/pluginmanager.h>
#include "floatbuffer.h"
#include "utils.h"




DominancyClassifier::DominancyClassifier( CMeshO &mesh, QList<OOCRaster> &rasterList, int weightMask ) :
    m_Mesh(mesh),
    m_RasterList(rasterList),
    m_VertexDom(mesh.vn),
    m_WeightMask(weightMask)
{
    updateDepthRange();
    updateMeshVBO();
    initShaders();

    for( QList<OOCRaster>::iterator r=rasterList.begin(); r!=rasterList.end(); ++r )
    {
        projectiveTexMatrices( *r );
        setupShadowTexture( *r );
        generateWeightsAndShadowMap( *r );
        checkDominancy( *r );
    }

    releaseAll();
}


bool DominancyClassifier::initShaders()
{
    std::string logs;
    if( !loadShader(m_WeightShader  ,"weight_gen"      ,&logs) ||
        !loadShader(m_VisCheckShader,"visibility_check",&logs) )
    {
        qWarning( (__FUNCTION__": "+logs).c_str() );
        return false;
    }


    GLint useImageBorderMask   = m_WeightMask & W_IMG_BORDER;
    GLint useViewDirectionMask = m_WeightMask & W_ORIENTATION;
    GLint useDistanceMask      = m_WeightMask & W_DISTANCE;
    GLint useSilhouetteMask    = m_WeightMask & W_SILHOUETTE;

    m_WeightShader.SetUniform( "u_UseImageBorderMask"  , &useImageBorderMask );
    m_WeightShader.SetUniform( "u_UseViewDirectionMask", &useViewDirectionMask );
    m_WeightShader.SetUniform( "u_UseDistanceMask"     , &useDistanceMask );
    m_WeightShader.SetUniform( "u_DepthMin"            , &m_DepthMin );
    m_WeightShader.SetUniform( "u_DepthMax"            , &m_DepthMax );

    m_VisCheckShader.SetUniform( "u_UseSilhouetteMask" , &useSilhouetteMask );

    return true;
}


void DominancyClassifier::updateMeshVBO()
{
    #pragma pack(push,1)
    struct VBOData
    {
        GLfloat vertex[3];
        GLfloat normal[3];
        GLint   id;
    };
    #pragma pack(pop)


    // Gathers vertex position and normal data from the mesh, and store them on GPU into a VBO.
    VBOData *vboData = new VBOData [ m_Mesh.vn ];

    for( int v=0; v<m_Mesh.vn; ++v )
    {
        for( int i=0; i<3; ++i )
        {
            vboData[v].vertex[i] = m_Mesh.vert[v].P()[i];
            vboData[v].normal[i] = m_Mesh.vert[v].N()[i];
        }
        vboData[v].id = v;
    }

    m_MeshVBO.Create( GL_STATIC_DRAW, vboData, m_Mesh.vn );
    m_MeshVBO.Vertex.SetPointer( sizeof(VBOData), OffsetOf(VBOData,vertex) );
    m_MeshVBO.Normal.SetPointer( sizeof(VBOData), OffsetOf(VBOData,normal) );
    m_MeshVBO.TexCoord.SetPointer( sizeof(VBOData), OffsetOf(VBOData,id) );

    delete [] vboData;


    // Gathers mesh face indices and store them into the VBO.
    unsigned int *vboIndices = new unsigned int [ 3*m_Mesh.fn ];

    for( int f=0, n=0; f<m_Mesh.fn; ++f )
        for( int i=0; i<3; ++i, ++n )
            vboIndices[n] = m_Mesh.face[f].V(i) - &m_Mesh.vert[0];

    m_MeshVBO.LoadIndices( GL_STATIC_DRAW, vboIndices, 3*m_Mesh.fn );

    delete [] vboIndices;
}


void DominancyClassifier::updateDepthRange()
{
    m_DepthMin =  std::numeric_limits<float>::max();
    m_DepthMax = -std::numeric_limits<float>::max();

    for( QList<OOCRaster>::iterator r=m_RasterList.begin(); r!=m_RasterList.end(); ++r )
    {
        float zNear, zFar;
        GlShot< vcg::Shot<float> >::GetNearFarPlanes( r->shot, m_Mesh.bbox, zNear, zFar );

        if( zNear < m_DepthMin )
            m_DepthMin = zNear;
        if( zFar > m_DepthMax )
            m_DepthMax = zFar;
    }

    if( m_DepthMin < 0.0001f )
        m_DepthMin = 0.1f;
    if( m_DepthMax < m_DepthMin )
        m_DepthMax = m_DepthMin + 1000.0f;
}


void DominancyClassifier::projectiveTexMatrices( OOCRaster &rr )
{
    // Recover the view frustum of the current raster.
    float zNear, zFar;
    GlShot< vcg::Shot<float> >::GetNearFarPlanes( rr.shot, m_Mesh.bbox, zNear, zFar );
    if( zNear < 0.0001f )
        zNear = 0.1f;
    if( zFar < zNear )
        zFar = zNear + 1000.0f;

    float l, r, b, t, focal;
    rr.shot.Intrinsics.GetFrustum( l, r, b, t, focal );


    // Compute from the frustum values the camera projection matrix.
    m_Proj.SetZero();
    m_Proj[0][0] = 2.0f*focal / (r-l);
    m_Proj[2][0] = (r+l) / (r-l);
    m_Proj[1][1] = 2.0f*focal / (t-b);
    m_Proj[2][1] = (t+b) / (t-b);
    m_Proj[2][2] = (zNear+zFar) / (zNear-zFar);
    m_Proj[3][2] = 2.0f*zNear*zFar / (zNear-zFar);
    m_Proj[2][3] = -1.0f;


    // Extract the pose matrix from the current raster.
    m_Pose = vcg::Transpose( rr.shot.GetWorldToExtrinsicsMatrix() );


    // Define the bias matrix that will enable to go from clipping space to texture space.
    const float biasMatData[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 0.5f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.5f, 0.5f, 0.5f, 1.0f };

    m_TexProj = m_Pose * m_Proj * vcg::Matrix44f(biasMatData);
}


void DominancyClassifier::setupShadowTexture( OOCRaster &rr )
{
    const vcg::Point2i vp = rr.shot.Intrinsics.ViewportPx;

    if( !m_ShadowMap.IsInstantiated() ||
        m_ShadowMap.Width() !=vp.X()  ||
        m_ShadowMap.Height()!=vp.Y()  )
    {
        glPushAttrib( GL_TEXTURE_BIT );

        // Create and initialize the OpenGL texture object used to store the shadow map.
        m_ShadowMap.Create( GL_DEPTH_COMPONENT,
                            vp.X(),
                            vp.Y(),
                            GL_DEPTH_COMPONENT,
                            GL_INT,
                            NULL );

        m_ShadowMap.SetFiltering( GL_NEAREST );
        m_ShadowMap.SetParam( GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE );
        m_ShadowMap.SetParam( GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL );
        m_ShadowMap.SetParam( GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY );

        glPopAttrib();
    }
}


void DominancyClassifier::generateWeightsAndShadowMap( OOCRaster &rr )
{
    // Backup the previous OpenGL states.
    glPushAttrib( GL_VIEWPORT_BIT     |
                  GL_COLOR_BUFFER_BIT |
                  GL_TRANSFORM_BIT    |
                  GL_ENABLE_BIT       |
                  GL_POLYGON_BIT      );


    // Set the OpenGL matrices so as to place the viewpoint on the given raster.
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadMatrixf( m_Proj.V() );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadMatrixf( m_Pose.V() );


    // Create the off-screen rendering context, using the shadow map texture as depth buffer.
    //m_WeightMap.Create( GL_LUMINANCE32F_ARB, m_ShadowMap.Width(), m_ShadowMap.Height() );
m_WeightMap.Create( GL_LUMINANCE, m_ShadowMap.Width(), m_ShadowMap.Height() );
    m_WeightMap.SetFiltering( GL_NEAREST );

    GPU::FrameBuffer fbuffer( m_ShadowMap.Width(), m_ShadowMap.Height() );
    fbuffer.Attach( GL_COLOR_ATTACHMENT0, m_WeightMap );
    fbuffer.Attach( GL_DEPTH_ATTACHMENT, m_ShadowMap );
    fbuffer.Bind();


    // Perform the rendering pass that computes the the shadow map used afterward for visibility checks
    // as well as the first weight map containing the orientation mask, the image border mask and
    // the distance masks (the silhouette mask is computed hereafter, using the depth gather in the shadow map).
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 2.0f, 2.0f );

    glClearColor( 0.0f, 1.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_WeightShader.Bind();
    m_MeshVBO.TexCoord.Disable();
    m_MeshVBO.Bind();
    m_MeshVBO.DrawElements( GL_TRIANGLES, 0, 3*m_Mesh.fn );
    m_MeshVBO.Unbind();
    m_MeshVBO.TexCoord.Enable();
    m_WeightShader.Unbind();

    fbuffer.Unbind();


    // Restore the previous OpenGL states.
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();

    glPopAttrib();
#if 0
unsigned char *b = new unsigned char [ fbuffer.Width()*fbuffer.Height() ];
fbuffer.DumpTo( GL_COLOR_ATTACHMENT0, b, GL_LUMINANCE, GL_UNSIGNED_BYTE );
QImage img( b, fbuffer.Width(), fbuffer.Height(), QImage::Format_Indexed8 );
img = img.convertToFormat( QImage::Format_RGB888 );
img.save( "test1.png" );
delete [] b;
#endif


    // Recover the depth of the scene and create the texture containing the step discontinuity mask, if required.
    if( m_WeightMask & W_SILHOUETTE )
    {
        glPushAttrib( GL_TEXTURE_BIT );

        // Read the depth normalized between 0 and 1 from the framebuffer.
        floatbuffer buffer1, buffer2;
        buffer1.init( fbuffer.Width(), fbuffer.Height() );
        buffer2.init( fbuffer.Width(), fbuffer.Height() );
        fbuffer.DumpTo( GL_DEPTH_ATTACHMENT, buffer1.data, GL_DEPTH_COMPONENT, GL_FLOAT );  // WARNING: leads to a GL error.


        float zNear, zFar;
        GlShot< vcg::Shot<float> >::GetNearFarPlanes( rr.shot, m_Mesh.bbox, zNear, zFar );
        if( zNear < 0.0001f )
            zNear = 0.1f;
        if( zFar < zNear )
            zFar = zNear + 1000.0f;

        float range = zFar - zNear;
        for( unsigned int i=0; i<fbuffer.Width()*fbuffer.Height(); ++i )
            if( buffer1.data[i] < 1.0f )
    	        buffer1.data[i] = zNear*zFar / ((zFar - buffer1.data[i]*range)*range);


        // Detect step discontinuities and compute the distance of each pixel to the closest one.
        buffer2.applysobel( &buffer1 );
        buffer2.initborder( &buffer1 );
        buffer2.distancefield();

        // Create the OpenGL texture containing the weight values.
        //m_WeightMapSilh.Create( GL_LUMINANCE32F_ARB, fbuffer.Width(), fbuffer.Height(), GL_LUMINANCE, GL_FLOAT, buffer2.data );
m_WeightMapSilh.Create( GL_LUMINANCE, fbuffer.Width(), fbuffer.Height(), GL_LUMINANCE, GL_FLOAT, buffer2.data );
        m_WeightMapSilh.SetFiltering( GL_NEAREST );

        glPopAttrib();
#if 0
unsigned char *b = new unsigned char [ fbuffer.Width()*fbuffer.Height() ];
for( unsigned int i=0; i<fbuffer.Width()*fbuffer.Height(); ++i )
    b[i] = (unsigned char)(255.0f*buffer2.data[i]);
QImage img( b, fbuffer.Width(), fbuffer.Height(), QImage::Format_Indexed8 );
img = img.convertToFormat( QImage::Format_RGB888 );
img.save( "test2.png" );
delete [] b;
#endif
    }
}


// This function performs visibility check and gathers weights for all mesh vertices with respect to the given raster,
// and update their dominant images accordingly.
// The computation is done by a shader that stores weight values in the frame buffer, with a negative weight for
// vertices that are not visible from the current raster's viewpoint. For each mesh vertex, a pixel is written,
// and pixel ordering in the framebuffer corresponds to vertex ordering in the mesh.
void DominancyClassifier::checkDominancy( OOCRaster &rr )
{
    // Save the previous OpenGL state.
    glPushAttrib( GL_VIEWPORT_BIT |
                  GL_ENABLE_BIT   |
                  GL_TEXTURE_BIT  );


    // Create a frambuffer with a viewport big enough to define one pixel per mesh vertex.
    vcg::Point2i vp;
    vp[0] = 2048;
    vp[1] = (int) std::ceil( (float)m_Mesh.vn / vp[0] );

    GPU::FrameBuffer fbuffer( vp[0], vp[1] );
    fbuffer.Attach( GL_COLOR_ATTACHMENT0, GL_LUMINANCE32F_ARB );
    fbuffer.Bind();


    // Set up the shader that will compute visibility check and weight gathering.
    m_VisCheckShader.Bind();
    m_ShadowMap.Bind( 0 );
    m_VisCheckShader.SetSampler( "u_ShadowMap"    , 0 );
    m_WeightMap.Bind( 1 );
    m_VisCheckShader.SetSampler( "u_WeightMap"    , 1 );
    if( m_WeightMask & W_SILHOUETTE )
    {
        m_WeightMapSilh.Bind( 2 );
        m_VisCheckShader.SetSampler( "u_WeightMapSilh", 2 );
    }
    m_VisCheckShader.SetUniform( "u_Viewport", vp.V() );
    m_VisCheckShader.SetUniform( "u_Viewpoint", rr.shot.GetViewPoint().V() );
    m_VisCheckShader.SetUniform( "u_ShadowProj", m_TexProj.V() );


    // Perform the rendering pass.
    glDisable( GL_DEPTH_TEST );
    m_MeshVBO.Bind();
    m_MeshVBO.DrawArrays( GL_POINTS, 0, m_Mesh.vn );
    m_MeshVBO.Unbind();


    // Restore the previous OpenGL state.
    m_ShadowMap.Unbind();
    m_WeightMap.Unbind();
    if( m_WeightMask & W_SILHOUETTE )
        m_WeightMapSilh.Unbind();
    m_VisCheckShader.Unbind();

    fbuffer.Unbind();

    glPopAttrib();


    // Read the content of the framebuffer and update the vertex dominancy accordingly.
    float *weightBuffer = new float [ vp[0]*vp[1] ];
    fbuffer.DumpTo( GL_COLOR_ATTACHMENT0, weightBuffer, GL_LUMINANCE, GL_FLOAT );

    for( int v=0; v<m_Mesh.vn; ++v )
        if( weightBuffer[v] > m_VertexDom[v].weight1 )
        {
            m_VertexDom[v].weight2   = m_VertexDom[v].weight1;
            m_VertexDom[v].dominant2 = m_VertexDom[v].dominant1;

            m_VertexDom[v].weight1   = weightBuffer[v];
            m_VertexDom[v].dominant1 = &rr;
        }
        else if( weightBuffer[v] > m_VertexDom[v].weight2 )
        {
            m_VertexDom[v].weight2   = weightBuffer[v];
            m_VertexDom[v].dominant2 = &rr;
        }

    delete [] weightBuffer;
}


void DominancyClassifier::releaseAll()
{
    m_MeshVBO.Release();
    m_WeightShader.Release();
    m_WeightMap.Release();
    m_WeightMapSilh.Release();
    m_ShadowMap.Release();
}


void DominancyClassifier::dominancyCoverage( RasterFaceMap &rpatches ) const
{
    rpatches.clear();

    for( CMeshO::FaceIterator f=m_Mesh.face.begin(); f!=m_Mesh.face.end(); ++f )
    {
        const VDominancy &d0 = (*this)[f->V(0)];
        const VDominancy &d1 = (*this)[f->V(1)];
        const VDominancy &d2 = (*this)[f->V(2)];

        std::set<OOCRaster*> rastersFBelongsTo;

        if( d0.dominant1 )
            rastersFBelongsTo.insert( d0.dominant1 );
        if( d1.dominant1 )
            rastersFBelongsTo.insert( d1.dominant1 );
        if( d2.dominant1 )
            rastersFBelongsTo.insert( d2.dominant1 );

        if( d0.isOnBoundary() )
            rastersFBelongsTo.insert( d0.dominant2 );
        if( d1.isOnBoundary() )
            rastersFBelongsTo.insert( d1.dominant2 );
        if( d2.isOnBoundary() )
            rastersFBelongsTo.insert( d2.dominant2 );

        for( std::set<OOCRaster*>::iterator r=rastersFBelongsTo.begin(); r!=rastersFBelongsTo.end(); ++r )
            rpatches[*r].push_back( &*f );
    }
}
