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
#include "TexturePainter.h"
#include <common/pluginmanager.h>




bool TexturePainter::init( int texSize )
{
    // Init the off-screen rendering buffer.
    m_TexImg = glw::createTexture2D( m_Context, GL_RGB, texSize, texSize, GL_RGB, GL_UNSIGNED_BYTE );
    glw::BoundTexture2DHandle boundTex = m_Context.bindTexture2D( m_TexImg, 0 );
    boundTex->setSampleMode( glw::TextureSampleMode(GL_NEAREST,GL_NEAREST,GL_CLAMP,GL_CLAMP,GL_CLAMP) );
    m_Context.unbindTexture2D( 0 );

    m_TexFB = glw::createFramebuffer( m_Context, glw::RenderTarget(), glw::texture2DTarget(m_TexImg) );


    // Init shaders used for color correction.
    const std::string initVertSrc = GLW_STRINGIFY
    (
        void main()
        {
            gl_Position = ftransform();
            gl_TexCoord[0] = gl_Vertex;
            gl_TexCoord[1] = gl_MultiTexCoord0;
        }
    );

    const std::string initFragSrc = GLW_STRINGIFY
    (
          uniform sampler2D   u_Tex;
        uniform int         u_Radius;
        uniform vec2        u_PixelSize;

        vec4 fetch( int texUnit, int x, int y )
        {
          if(texUnit==0) return texture2D( u_Tex, gl_TexCoord[0].xy + u_PixelSize * vec2(x,y) );
          return texture2D( u_Tex, gl_TexCoord[1].xy + u_PixelSize * vec2(x,y) );
//            return texture2D( u_Tex, gl_TexCoord[texUnit].xy + u_PixelSize * vec2(x,y) ).xyz;
        }

        void main()
        {
            vec3 finalColor0 = vec3(0.0);
            vec3 finalColor1 = vec3(0.0);
            float cnt0 = 0.0;
            float cnt1 = 0.0;
            for( int y=-u_Radius; y<=u_Radius; ++y )
                for( int x=-u_Radius; x<=u_Radius; ++x )
                {
                    vec4 c0 = fetch( 0, x, y );
                    finalColor0+=c0.a*c0.xyz;
                    cnt0+=c0.a;

                    vec4 c1 = fetch( 1, x, y );
                    finalColor1+=c1.a*c1.xyz;
                    cnt1+=c1.a;
                }
            vec3 finalColor = 0.5*(finalColor1/cnt1 - finalColor0/cnt0);
            if(cnt0<=0.1 || cnt1<=0.1) finalColor = vec3(0);
            //int diameter = 2*u_Radius + 1;
//            finalColor /= float(cnt);

            gl_FragColor = vec4( finalColor, 1.0 );
        }
    );

    const std::string correctionVertSrc = GLW_STRINGIFY
    (
        void main()
        {
            gl_Position = gl_Vertex;
        }
    );

    const std::string pushFragSrc = GLW_STRINGIFY
    (
        uniform sampler2D   u_TexHigher;
        uniform vec2        u_PixelSize;

        void main()
        {
            vec2 pos = vec2(ivec2( gl_FragCoord.xy )*2);

            vec4 avg = vec4( 0.0 );
            for( int y=0; y<2; ++y )
                for( int x=0; x<2; ++x )
                    avg += texture2D( u_TexHigher, (pos+vec2(x,y))*u_PixelSize );

            if( avg.w < 0.5 )
                gl_FragColor = vec4( 0.0 );
            else
                gl_FragColor = vec4( avg.xyz/avg.w, 1.0 );
        }
    );

    const std::string pullFragSrc = GLW_STRINGIFY
    (
        uniform sampler2D   u_TexLower;
        uniform sampler2D   u_TexHigher;
        uniform vec2        u_PixelSize;

        void main()
        {
            vec2 texCoord = u_PixelSize * gl_FragCoord.xy;

            vec4 color = texture2D( u_TexHigher, texCoord );
            if( color.w < 0.5 )
                color = texture2D( u_TexLower, texCoord );

            gl_FragColor = color;
        }
    );

    const std::string combineFragSrc = GLW_STRINGIFY
    (
        uniform sampler2D   u_TexColor;
        uniform sampler2D   u_TexCorrection;
        uniform vec2        u_PixelSize;

        void main()
        {
            vec2 texCoord = gl_FragCoord.xy * u_PixelSize;

            vec4 color = texture2D( u_TexColor,      texCoord ) +
                         texture2D( u_TexCorrection, texCoord );

            gl_FragColor = vec4( clamp(color.xyz,0.0,1.0), 1.0 );
        }
    );

    m_PushPullShader_Init    = glw::createProgram( m_Context, "", initVertSrc      , initFragSrc    ); qDebug("1");
    m_PushPullShader_Push    = glw::createProgram( m_Context, "", correctionVertSrc, pushFragSrc    );qDebug("2");
    m_PushPullShader_Pull    = glw::createProgram( m_Context, "", correctionVertSrc, pullFragSrc    );qDebug("3");
    m_PushPullShader_Combine = glw::createProgram( m_Context, "", correctionVertSrc, combineFragSrc );qDebug("4");

    if( !m_PushPullShader_Init   ->isLinked() ||
        !m_PushPullShader_Push   ->isLinked() ||
        !m_PushPullShader_Pull   ->isLinked() ||
        !m_PushPullShader_Combine->isLinked() )
    {
    //    qWarning( (std::string(__func__)+": "+logs).c_str() );
        return false;
    }


    return true;
}


void TexturePainter::paint( RasterPatchMap &patches )
{
    if( !isInitialized() )
        return;


    // Initializes the OpenGL state and transformations for the texture painting rendering pass.
    glPushAttrib( GL_VIEWPORT_BIT     |
                  GL_TRANSFORM_BIT    |
                  GL_ENABLE_BIT       |
                  GL_COLOR_BUFFER_BIT |
                  GL_CURRENT_BIT      );

    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
    glColor3ub( 255, 255, 255 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();


    // Initializes the off-screen rendering context.
    m_Context.bindReadDrawFramebuffer( m_TexFB );
    glViewport( 0, 0, m_TexImg->width(), m_TexImg->height() );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    glActiveTexture( GL_TEXTURE0_ARB );
    glEnable( GL_TEXTURE_2D );


    // TEXTURE PAINTING.
    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
    {
        QImage &rmImg = rp.key()->currentPlane->image;


        // Loads the raster into the GPU as a texture image.
        GLubyte *rasterData = new GLubyte [ 3*rmImg.width()*rmImg.height() ];
        for( int y=rmImg.height()-1, n=0; y>=0; --y )
            for( int x=0; x<rmImg.width(); ++x, n+=3 )
            {
                QRgb p = rmImg.pixel(x,y);
                rasterData[n+0] = qRed  (p);
                rasterData[n+1] = qGreen(p);
                rasterData[n+2] = qBlue (p);
            }

        glw::Texture2DHandle rasterTex = glw::createTexture2D( m_Context, GL_RGB, rmImg.width(), rmImg.height(), GL_RGB, GL_UNSIGNED_BYTE, rasterData );
        delete [] rasterData;

        glw::BoundTexture2DHandle t = m_Context.bindTexture2D( rasterTex, 0 );
        t->setSampleMode( glw::TextureSampleMode(GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP,GL_CLAMP) );


        // Set the texture matrix up so as to rescale UV coordinates from [0,w]x[0,h] to [0,1]x[0,1].
        glMatrixMode( GL_TEXTURE );
        glPushMatrix();
        glLoadIdentity();
        glScalef( 1.0f/rmImg.width(), 1.0f/rmImg.height(), 1.0f );


        // Paints all patches by copying the rectangular area corresponding to its bounding box from
        // the raster texture to the final texture.
        glMatrixMode( GL_MODELVIEW );

        for( PatchVec::const_iterator p=rp->begin(); p!=rp->end(); ++p )
        {
            vcg::glLoadMatrix( p->img2tex );
            glBegin( GL_QUADS );
                vcg::Point2f boxCorners[4];
                boxCorners[0] = p->bbox.min;
                boxCorners[1] = vcg::Point2f( p->bbox.max.X(), p->bbox.min.Y() );
                boxCorners[2] = p->bbox.max;
                boxCorners[3] = vcg::Point2f( p->bbox.min.X(), p->bbox.max.Y() );

                for( int i=0; i<4; ++i )
                {
                    glTexCoord2fv( boxCorners[i].V() );
                    glVertex2fv( boxCorners[i].V() );
                }
            glEnd();
        }

        m_Context.unbindTexture2D( 0 );
        glMatrixMode( GL_TEXTURE );
        glPopMatrix();
    }

    m_Context.unbindReadDrawFramebuffer();


    // Restore the previous OpenGL state.
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glPopAttrib();
}


void TexturePainter::pushPullInit( RasterPatchMap &patches,
                                   glw::Texture2DHandle &diffTex,
                                   int filterSize )
{
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glw::FramebufferHandle fbuffer = glw::createFramebuffer( m_Context, glw::RenderTarget(), glw::texture2DTarget(diffTex) );

    m_Context.bindReadDrawFramebuffer( fbuffer );
    m_Context.bindTexture2D( m_TexImg, 0 );
    glw::BoundProgramHandle p = m_Context.bindProgram( m_PushPullShader_Init );
    p->setUniform( "u_Tex", 0 );
    p->setUniform1( "u_Radius", &filterSize );
    p->setUniform( "u_PixelSize", 1.0f/m_TexImg->width(), 1.0f/m_TexImg->height() );

    glViewport( 0, 0, diffTex->width(), diffTex->height() );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    //glBegin( GL_TRIANGLES );
    //    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
    //        for( PatchVec::iterator p=rp->begin(); p!=rp->end(); ++p )
    //            for( unsigned int n=0; n<p->boundary.size(); ++n )
    //                for( int i=0; i<3; ++i )
    //                {
    //                    glTexCoord2fv( p->boundary[n]->WT(i).P().V() );
    //                    glVertex2fv( p->boundaryUV[n].v[i].P().V() );
    //                }
    //glEnd();

    glBegin( GL_POINTS );
        for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
            for( PatchVec::iterator p=rp->begin(); p!=rp->end(); ++p )
                for( unsigned int n=0; n<p->boundary.size(); ++n )
                    for( int i=0; i<3; ++i )
                    {
                        glTexCoord2fv( p->boundary[n]->WT(i).P().V() );
                        glVertex2fv( p->boundaryUV[n].v[i].P().V() );
                    }
    glEnd();

    m_Context.unbindProgram();
    m_Context.unbindTexture2D( 0 );
    m_Context.unbindReadDrawFramebuffer();

    QImage ttt = getTexture();
    ttt.save("dump.png");

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
}


void TexturePainter::push( glw::Texture2DHandle &higherLevel,
                           glw::Texture2DHandle &lowerLevel )
{
    glw::FramebufferHandle fbuffer = glw::createFramebuffer( m_Context, glw::RenderTarget(), glw::texture2DTarget(lowerLevel) );
    glViewport( 0, 0, lowerLevel->width(), lowerLevel->height() );

    m_Context.bindReadDrawFramebuffer( fbuffer );
    m_Context.bindTexture2D( higherLevel, 0 );
    glw::BoundProgramHandle p = m_Context.bindProgram( m_PushPullShader_Push );
    p->setUniform( "u_TexHigher", 0 );
    p->setUniform( "u_PixelSize", 1.0f/higherLevel->width(), 1.0f/higherLevel->height() );

    glBegin( GL_QUADS );
        glVertex2i( -1, -1 );
        glVertex2i(  1, -1 );
        glVertex2i(  1,  1 );
        glVertex2i( -1,  1 );
    glEnd();

    m_Context.unbindProgram();
    m_Context.unbindTexture2D( 0 );
    m_Context.unbindReadDrawFramebuffer();
}


void TexturePainter::pull( glw::Texture2DHandle &lowerLevel,
                           glw::Texture2DHandle &higherLevel )
{
    glw::Texture2DHandle tmp = glw::createTexture2D( m_Context, GL_RGBA32F, higherLevel->width(), higherLevel->height(), GL_RGBA, GL_FLOAT );
    glw::BoundTexture2DHandle boundTmp = m_Context.bindTexture2D( tmp, 0 );
        boundTmp->setSampleMode( glw::TextureSampleMode(GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP,GL_CLAMP) );
    m_Context.unbindTexture2D( 0 );

    glw::FramebufferHandle fbuffer = glw::createFramebuffer( m_Context, glw::RenderTarget(), glw::texture2DTarget(tmp) );
    glViewport( 0, 0, tmp->width(), tmp->height() );

    m_Context.bindReadDrawFramebuffer( fbuffer );
    m_Context.bindTexture2D( lowerLevel,  0 );
    m_Context.bindTexture2D( higherLevel, 1 );
    glw::BoundProgramHandle p = m_Context.bindProgram( m_PushPullShader_Pull );
    p->setUniform( "u_TexLower", 0 );
    p->setUniform( "u_TexHigher", 1 );
    p->setUniform( "u_PixelSize", 1.0f/tmp->width(), 1.0f/tmp->height() );

    glBegin( GL_QUADS );
        glVertex2i( -1, -1 );
        glVertex2i(  1, -1 );
        glVertex2i(  1,  1 );
        glVertex2i( -1,  1 );
    glEnd();

    m_Context.unbindProgram();
    m_Context.unbindTexture2D( 0 );
    m_Context.unbindTexture2D( 1 );
    m_Context.unbindReadDrawFramebuffer();

    higherLevel = tmp;
}


void TexturePainter::apply( glw::Texture2DHandle &color,
                            glw::Texture2DHandle &correction )
{
    glw::Texture2DHandle tmp = glw::createTexture2D( m_Context, GL_RGB, color->width(), color->height(), GL_RGB, GL_UNSIGNED_BYTE );
    glw::BoundTexture2DHandle t = m_Context.bindTexture2D( tmp, 0 );
        t->setSampleMode( glw::TextureSampleMode(GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP,GL_CLAMP) );
    m_Context.unbindTexture2D( 0 );

    m_TexFB = glw::createFramebuffer( m_Context, glw::RenderTarget(), glw::texture2DTarget(tmp) );
    glViewport( 0, 0, tmp->width(), tmp->height() );

    m_Context.bindReadDrawFramebuffer( m_TexFB );
    m_Context.bindTexture2D( color,      0 );
    m_Context.bindTexture2D( correction, 1 );
    glw::BoundProgramHandle p = m_Context.bindProgram( m_PushPullShader_Combine );
    p->setUniform( "u_TexColor", 0 );
    p->setUniform( "u_TexCorrection", 1 );
    p->setUniform( "u_PixelSize", 1.0f/color->width(), 1.0f/color->height() );

    glBegin( GL_QUADS );
        glVertex2i( -1, -1 );
        glVertex2i(  1, -1 );
        glVertex2i(  1,  1 );
        glVertex2i( -1,  1 );
    glEnd();

    m_Context.unbindProgram();
    m_Context.unbindTexture2D( 0 );
    m_Context.unbindTexture2D( 1 );
    m_Context.unbindReadDrawFramebuffer();

    color = tmp;
}


void TexturePainter::rectifyColor( RasterPatchMap &patches, int filterSize )
{
    if( !isInitialized() )
        return;

    glPushAttrib( GL_ALL_ATTRIB_BITS );


    int nbLevels = std::ceil( std::log((float)m_TexImg->width()) / std::log(2.0f) );

    std::vector<glw::Texture2DHandle> pushPullStack;
    pushPullStack.reserve( nbLevels+1 );

    pushPullStack.resize( 1 );
    pushPullStack[0] = glw::createTexture2D( m_Context, GL_RGBA32F, m_TexImg->width(), m_TexImg->height(), GL_RGB, GL_UNSIGNED_BYTE );
    glw::BoundTexture2DHandle t = m_Context.bindTexture2D( pushPullStack[0], 0 );
        t->setSampleMode( glw::TextureSampleMode(GL_NEAREST,GL_NEAREST,GL_CLAMP,GL_CLAMP,GL_CLAMP) );
    m_Context.unbindTexture2D( 0 );

    pushPullInit( patches, pushPullStack[0], filterSize );


    while( pushPullStack.back()->width() > 1 )
    {
        unsigned int newDim = (pushPullStack.back()->width()/2) + (pushPullStack.back()->width()&1);

        glw::Texture2DHandle newLevel = glw::createTexture2D( m_Context, GL_RGBA32F, newDim, newDim, GL_RGB, GL_UNSIGNED_BYTE );
        glw::BoundTexture2DHandle t = m_Context.bindTexture2D( newLevel, 0 );
            t->setSampleMode( glw::TextureSampleMode(GL_NEAREST,GL_NEAREST,GL_CLAMP,GL_CLAMP,GL_CLAMP) );
        m_Context.unbindTexture2D( 0 );

        push( pushPullStack.back(), newLevel );
        pushPullStack.push_back( newLevel );
    }


    for( int i=(int)pushPullStack.size()-2; i>=0; --i )
        pull( pushPullStack[i+1], pushPullStack[i] );


    apply( m_TexImg, pushPullStack[0] );


    glPopAttrib();
}


QImage TexturePainter::getTexture()
{
    if( !isInitialized() )
        return QImage();


    // Recovers the content of the off-screen painting buffer and returns it as a QImage object.
    m_Context.bindReadDrawFramebuffer( m_TexFB );
    glReadBuffer( GL_COLOR_ATTACHMENT0 );

    GLubyte *texData = new GLubyte [ 3*m_TexImg->width()*m_TexImg->height() ];
    glReadPixels( 0, 0, m_TexImg->width(), m_TexImg->height(), GL_RGB, GL_UNSIGNED_BYTE, texData );

    m_Context.unbindReadDrawFramebuffer();

    QImage tex( m_TexImg->width(), m_TexImg->height(), QImage::Format_ARGB32 );
    for( int y=(int)m_TexImg->height()-1, n=0; y>=0; --y )
        for( int x= 0; x<(int)m_TexImg->width(); ++x, n+=3 ){
            tex.setPixel( x, y, qRgb(texData[n+0],texData[n+1],texData[n+2]) );
//            if((x%100)==0 && (y%100)==0) qDebug("img %i %i",x,y);
}
    delete [] texData;
    return tex;
}
