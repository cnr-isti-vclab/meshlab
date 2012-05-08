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
#include <cmath>
#include "VisibilityCheck.h"
#include <wrap/gl/shot.h>
#include <common/pluginmanager.h>




VisibilityCheck* VisibilityCheck::s_Instance = NULL;


VisibilityCheck* VisibilityCheck::GetInstance( glw::Context &ctx )
{
    if( !s_Instance )
    {
        if( VisibilityCheck_ShadowMap::isSupported() )
            s_Instance = new VisibilityCheck_ShadowMap( ctx );
        else if( VisibilityCheck_VMV2002::isSupported() )
            s_Instance = new VisibilityCheck_VMV2002( ctx );
    }

    return s_Instance;
}


void VisibilityCheck::ReleaseInstance()
{
    delete s_Instance;
    s_Instance = NULL;
}






bool VisibilityCheck_VMV2002::isSupported()
{
    std::string ext( (char*) glGetString(GL_EXTENSIONS) );
    return ext.find("ARB_framebuffer_object") != std::string::npos;
}


void VisibilityCheck_VMV2002::init( std::vector<unsigned char> &visBuffer )
{
    // Save OpenGL current state.
    glPushAttrib( GL_ENABLE_BIT       |
                  GL_POLYGON_BIT      |
                  GL_VIEWPORT_BIT     |
                  GL_DEPTH_BUFFER_BIT );


    // Initialize the visibility check context.
    vcg::Point2i &vp = m_Raster->shot.Intrinsics.ViewportPx;

    if( m_FrameBuffer.isNull()        ||
        m_ColorRB->width()  != vp.X() ||
        m_ColorRB->height() != vp.Y() )
    {
        m_ColorRB     = glw::createRenderbuffer( m_Context, GL_RGBA, vp.X(), vp.Y() );
        m_DepthRB     = glw::createRenderbuffer( m_Context, GL_DEPTH_COMPONENT, vp.X(), vp.Y() );
		m_FrameBuffer = glw::createFramebuffer ( m_Context, glw::renderbufferTarget(m_DepthRB), glw::renderbufferTarget(m_ColorRB) );
    }

    m_Context.bindReadDrawFramebuffer( m_FrameBuffer );

    m_ViewportMin = vcg::Point2i( 0, 0 );
    m_ViewportMax = vcg::Point2i( vp.X()-1, vp.Y()-1 );

    visBuffer.resize( 4*vp.X()*vp.Y() );

    m_VertFlag.resize( m_Mesh->vn );
    for( int v=0; v<m_Mesh->vn; ++v )
        m_VertFlag[v] = (m_Raster->shot.GetViewPoint()-m_Mesh->vert[v].P()) * m_Mesh->vert[v].N() > 0.0f? V_UNDEFINED : V_BACKFACE;


    // Initialize the OpenGL transform so as to place the camera on the current raster.
    float zNear, zFar;
    GlShot< vcg::Shot<float> >::GetNearFarPlanes( m_Raster->shot, m_Mesh->bbox, zNear, zFar );
    if( zNear < 0.0001f )
        zNear = 0.1f;
    if( zFar < zNear )
        zFar = zNear + 1000.0f;

    GlShot< vcg::Shot<float> >::SetView( m_Raster->shot, zNear, zFar );


    // Perform the first rendering pass that initializes the depth buffer.
    glViewport( 0, 0, vp.X(), vp.Y() );

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 2.0f, 2.0f );

    glClear( GL_DEPTH_BUFFER_BIT );

    glBegin( GL_TRIANGLES );
        for( CMeshO::FaceIterator fi=m_Mesh->face.begin(); fi!=m_Mesh->face.end(); ++fi )
        {
            glVertex3fv( fi->V(0)->P().V() );
            glVertex3fv( fi->V(1)->P().V() );
            glVertex3fv( fi->V(2)->P().V() );
        }
    glEnd();


    // Prepare the OpenGL states for the rendering during the subsequent iterative steps of vibility check.
    glEnable( GL_BLEND );
    glBlendFunc( GL_ONE, GL_ZERO );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glDisable( GL_POLYGON_OFFSET_FILL );
    glDepthMask( GL_FALSE );
    glPointSize( 1.0f );
}


bool VisibilityCheck_VMV2002::iteration( std::vector<unsigned char> &visBuffer )
{
    // Render all mesh vertices so as to confront them to the pre-rendered depth buffer.
    // The color encode the vertex ID in the mesh.
    glClear( GL_COLOR_BUFFER_BIT );

    glBegin( GL_POINTS );
        for( int v=0; v<m_Mesh->vn; ++v )
            if( m_VertFlag[v] == V_UNDEFINED )
            {
                int vertId = v + 1;
                glColor4ub( (vertId    ) & 0xFF,
                            (vertId>> 8) & 0xFF,
                            (vertId>>16) & 0xFF,
                            (vertId>>24) & 0xFF );
                glVertex3fv( m_Mesh->vert[v].P().V() );
            }
    glEnd();


    // Read the content of the framebuffer so as to recover the IDs of visible vertices.
    // If a pixel contains a valid ID, the corresponding vertex in the mesh is marked
    // as visible from the current raster.
    // The viewport is progressively restricted to the screen area covered by the last
    // rendering so as to speed up computations.
    glReadPixels( m_ViewportMin.X(),
                  m_ViewportMin.Y(),
                  m_ViewportMax.X()-m_ViewportMin.X()+1,
                  m_ViewportMax.Y()-m_ViewportMin.Y()+1,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  &visBuffer[0] );

    int xMin = m_ViewportMax.X(), xMax = m_ViewportMin.X()-1;
    int yMin = m_ViewportMax.Y(), yMax = m_ViewportMin.X()-1;

    for( int y=m_ViewportMin.Y(), n=0; y<=m_ViewportMax.Y(); ++y )
        for( int x=m_ViewportMin.X(); x<=m_ViewportMax.X(); ++x, n+=4 )
        {
            int vertId =  (int)visBuffer[n+0]        +
                         ((int)visBuffer[n+1] <<  8) +
                         ((int)visBuffer[n+2] << 16) +
                         ((int)visBuffer[n+3] << 24) ;

            if( vertId > 0 )
            {
                if( x < xMin )
                    xMin = x;
                if( x > xMax )
                    xMax = x;

                if( y < yMin )
                    yMin = y;
                if( y > yMax )
                    yMax = y;

                m_VertFlag[vertId-1] = V_VISIBLE;
            }
        }

    m_ViewportMin = vcg::Point2i( xMin, yMin );
    m_ViewportMax = vcg::Point2i( xMax, yMax );


    return xMin < xMax;
}


void VisibilityCheck_VMV2002::release()
{
    GlShot< vcg::Shot<float> >::UnsetView();
    m_Context.unbindReadDrawFramebuffer();
    glPopAttrib();
}


void VisibilityCheck_VMV2002::checkVisibility()
{
    std::vector<unsigned char> visBuffer;
    init(visBuffer);
    while( iteration(visBuffer) );
    release();
}






bool VisibilityCheck_ShadowMap::s_AreVBOSupported = false;


VisibilityCheck_ShadowMap::VisibilityCheck_ShadowMap( glw::Context &ctx ) : VisibilityCheck(ctx)
{
    std::string ext( (char*) glGetString(GL_EXTENSIONS) );
    s_AreVBOSupported = ext.find( "ARB_vertex_buffer_object" ) != std::string::npos;

    initShaders();
}


void VisibilityCheck_ShadowMap::shadowProjMatrices()
{
    // Recover the near and far clipping planes by considering the bounding box of the current mesh
    // in the camera space of the current raster.
    float zNear, zFar;
    GlShot< vcg::Shot<float> >::GetNearFarPlanes( m_Raster->shot, m_Mesh->bbox, zNear, zFar );
    if( zNear < 0.0001f )
        zNear = 0.1f;
    if( zFar < zNear )
        zFar = zNear + 1000.0f;


    // Recover the view frustum of the current raster.
    float l, r, b, t, focal;
    m_Raster->shot.Intrinsics.GetFrustum( l, r, b, t, focal );


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
    m_Pose = m_Raster->shot.GetWorldToExtrinsicsMatrix().transpose();


    // Define the bias matrix that will enable to go from clipping space to texture space.
    const float biasMatData[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 0.5f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.5f, 0.5f, 0.5f, 1.0f };

    m_ShadowProj = m_Pose * m_Proj * vcg::Matrix44f(biasMatData);
}


void VisibilityCheck_ShadowMap::setupShadowTexture()
{
    glPushAttrib( GL_TEXTURE_BIT );

    // Create and initialize the OpenGL texture object used to store the shadow map.
    m_ShadowMap = glw::createTexture2D( m_Context,
                                        GL_DEPTH_COMPONENT,
                                        m_Raster->shot.Intrinsics.ViewportPx.X(),
                                        m_Raster->shot.Intrinsics.ViewportPx.Y(),
                                        GL_DEPTH_COMPONENT,
                                        GL_INT );

    glw::BoundTexture2DHandle boundShadowMap = m_Context.bindTexture2D(m_ShadowMap, 0);
        boundShadowMap->setSampleMode( glw::TextureSampleMode(GL_NEAREST,GL_NEAREST,GL_CLAMP,GL_CLAMP,GL_CLAMP) );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL               );
        glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB  , GL_INTENSITY            );
    m_Context.unbindTexture2D( 0 );

    glPopAttrib();
}


void VisibilityCheck_ShadowMap::updateShadowTexture()
{
    glPushAttrib( GL_ENABLE_BIT    |
                  GL_POLYGON_BIT   |
                  GL_TRANSFORM_BIT |
                  GL_VIEWPORT_BIT  );


    // Set up the OpenGL transformation matrices so as to place the viewpoint on the raster's camera.
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadMatrixf( m_Proj.V() );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadMatrixf( m_Pose.V() );


    // Perform an off-screen rendering pass so as to generate the a depth map of the model
    // from the viewpoint of the current raster's camera.
    glw::FramebufferHandle fbuffer = glw::createFramebuffer( m_Context, glw::texture2DTarget(m_ShadowMap) );
    m_Context.bindReadDrawFramebuffer( fbuffer );

    glViewport( 0, 0, m_ShadowMap->width(), m_ShadowMap->height() );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 2.0f, 2.0f );

    glClear( GL_DEPTH_BUFFER_BIT );
    if( s_AreVBOSupported )
    {
        glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
		glEnableClientState( GL_VERTEX_ARRAY );

        m_Context.bindVertexBuffer( m_ShadowVBOVertices );
		glVertexPointer( 3, GL_FLOAT, 0, 0 );
		m_Context.unbindVertexBuffer();

        m_Context.bindIndexBuffer( m_ShadowVBOIndices );
		glDrawElements( GL_TRIANGLES, 3*m_Mesh->fn, GL_UNSIGNED_INT, 0 );
		m_Context.unbindIndexBuffer();

        glPopClientAttrib();
    }
    else
    {
        glBegin( GL_TRIANGLES );
            for( CMeshO::FaceIterator fi=m_Mesh->face.begin(); fi!=m_Mesh->face.end(); ++fi )
            {
                glVertex3fv( fi->V(0)->P().V() );
                glVertex3fv( fi->V(1)->P().V() );
                glVertex3fv( fi->V(2)->P().V() );
            }
        glEnd();
    }

    m_Context.unbindReadDrawFramebuffer();


    // Restore the previous OpenGL state.
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    glPopAttrib();
}


bool VisibilityCheck_ShadowMap::initShaders()
{
    const std::string vertSrc = GLW_STRINGIFY
    (
        void main()
        {
            gl_Position = gl_Vertex;
        }
    );

    const std::string fragSrc = GLW_STRINGIFY
    (
        uniform sampler2D       u_VertexMap;
        uniform sampler2D       u_NormalMap;
        uniform sampler2DShadow u_SadowMap;
        uniform mat4            u_ShadowProj;
        uniform vec3            u_Viewpoint;

        const float             V_UNDEFINED = 0.0;
        const float             V_BACKFACE  = 1.0 / 255.0;
        const float             V_VISIBLE   = 2.0 / 255.0;


        void main()
        {
            vec3 pos = texelFetch( u_VertexMap, ivec2(gl_FragCoord.xy), 0 ).xyz;
            vec3 nor = texelFetch( u_NormalMap, ivec2(gl_FragCoord.xy), 0 ).xyz;

            if( dot(u_Viewpoint-pos,nor) < 0.0 )
                gl_FragColor = vec4( V_BACKFACE );
            else
            {
                vec4 projVert = u_ShadowProj * vec4(pos,1.0);
                vec2 clipCoord = projVert.xy / projVert.w;

                if( clipCoord.x>=0.0 && clipCoord.x<=1.0 &&
                    clipCoord.y>=0.0 && clipCoord.y<=1.0 &&
                    shadow2DProj( u_SadowMap, projVert ).r > 0.5 )
                    gl_FragColor = vec4( V_VISIBLE );
                else
                    gl_FragColor = vec4( V_UNDEFINED );
            }
        }
    );


	m_VisDetectionShader = glw::createProgram( m_Context, "", vertSrc, fragSrc );
    return m_VisDetectionShader->isLinked();
}


bool VisibilityCheck_ShadowMap::isSupported()
{
    std::string ext( (char*) glGetString(GL_EXTENSIONS) );
    return ext.find("ARB_framebuffer_object") != std::string::npos &&
           ext.find("ARB_shader_objects")     != std::string::npos &&
           ext.find("ARB_texture_float")      != std::string::npos;
}


void VisibilityCheck_ShadowMap::initMeshTextures()
{
    // Creates a first OpenGL texture into which normal vectors at each mesh vertex are stored.
    int mapH = (int) std::ceil( m_Mesh->vn / 2048.0f );
    vcg::Point3f *mapData = new vcg::Point3f [ 2048*mapH ];

    for( int i=0; i<m_Mesh->vn; ++i )
        mapData[i] = m_Mesh->vert[i].N();

    m_NormalMap = glw::createTexture2D( m_Context,
                                        GL_RGB32F,
                                        2048,
                                        mapH,
                                        GL_RGB,
                                        GL_FLOAT,
                                        mapData );

    glw::BoundTexture2DHandle boundTex = m_Context.bindTexture2D( m_NormalMap, 0 );
        boundTex->setSampleMode( glw::TextureSampleMode(GL_NEAREST,GL_NEAREST,GL_CLAMP,GL_CLAMP,GL_CLAMP) );
    m_Context.unbindTexture2D( 0 );


    // Does the same with a second texture to store the mesh vertices.
    for( int i=0; i<m_Mesh->vn; ++i )
        mapData[i] = m_Mesh->vert[i].P();

    m_VertexMap = glw::createTexture2D( m_Context,
                                        GL_RGB32F,
                                        2048,
                                        mapH,
                                        GL_RGB,
                                        GL_FLOAT,
                                        mapData );

    boundTex = m_Context.bindTexture2D( m_VertexMap, 0 );
        boundTex->setSampleMode( glw::TextureSampleMode(GL_NEAREST,GL_NEAREST,GL_CLAMP,GL_CLAMP,GL_CLAMP) );
    m_Context.unbindTexture2D( 0 );


    // Creates the VBO that will be used for the generation of the shadow map.
    if( s_AreVBOSupported )
    {
        m_ShadowVBOVertices = glw::createBuffer( m_Context, m_Mesh->vn*sizeof(vcg::Point3f), mapData );
        delete [] mapData;

        unsigned int *indices = new unsigned int [ 3*m_Mesh->fn ];
        for( int f=0, n=0; f<m_Mesh->fn; ++f )
            for( int v=0; v<3; ++v, ++n )
                indices[n] = m_Mesh->face[f].V(v) - &m_Mesh->vert[0];

        m_ShadowVBOIndices = glw::createBuffer( m_Context, 3*m_Mesh->fn*sizeof(unsigned int), indices );
        delete [] indices;
    }
    else
        delete [] mapData;
}


void VisibilityCheck_ShadowMap::setMesh( CMeshO *mesh )
{
    if( mesh && mesh!=m_Mesh )
    {
        m_Mesh = mesh;

        initMeshTextures();

        // Create the framebuffer into which the result of the visibility computation will be stored.
        m_ColorBuffer = glw::createRenderbuffer( m_Context, GL_RED, m_VertexMap->width(), m_VertexMap->height() );
        m_FBuffer     = glw::createFramebuffer ( m_Context, glw::RenderTarget(), glw::renderbufferTarget(m_ColorBuffer) );
    }
}


void VisibilityCheck_ShadowMap::setRaster( RasterModel *rm )
{
    if( rm && rm!=m_Raster )
    {
        m_Raster = rm;

        shadowProjMatrices();
        setupShadowTexture();
    }
}


void VisibilityCheck_ShadowMap::checkVisibility()
{
    updateShadowTexture();


    m_Context.bindReadDrawFramebuffer( m_FBuffer );
    glViewport( 0, 0, m_ColorBuffer->width(), m_ColorBuffer->height() );

    m_Context.bindTexture2D( m_VertexMap, 0 );
    m_Context.bindTexture2D( m_NormalMap, 1 );
    m_Context.bindTexture2D( m_ShadowMap, 2 );

    glw::BoundProgramHandle boundShader = m_Context.bindProgram( m_VisDetectionShader );
    boundShader->setUniform( "u_VertexMap", 0 );
    boundShader->setUniform( "u_NormalMap", 1 );
    boundShader->setUniform( "u_SadowMap" , 2 );
    boundShader->setUniform4x4( "u_ShadowProj", m_ShadowProj.V(), false );
    boundShader->setUniform3( "u_Viewpoint", m_Raster->shot.GetViewPoint().V() );

    glBegin( GL_QUADS );
        glVertex2i( -1, -1 );
        glVertex2i(  1, -1 );
        glVertex2i(  1,  1 );
        glVertex2i( -1,  1 );
    glEnd();

    m_Context.unbindProgram();
    m_Context.unbindTexture2D( 0 );
    m_Context.unbindTexture2D( 1 );
    m_Context.unbindTexture2D( 2 );

    m_VertFlag.resize( m_ColorBuffer->width() * m_ColorBuffer->height() );
    glReadPixels( 0,
                  0,
                  m_ColorBuffer->width(),
                  m_ColorBuffer->height(),
                  GL_RED,
                  GL_UNSIGNED_BYTE,
                  &m_VertFlag[0] );

    m_Context.unbindReadDrawFramebuffer();
}
