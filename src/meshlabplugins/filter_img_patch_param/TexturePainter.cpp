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
    m_TexImg.Create( GL_RGB, texSize, texSize );
    m_TexImg.SetFiltering( GL_NEAREST );

    m_TexFB.Create( texSize, texSize );
    m_TexFB.Attach( GL_COLOR_ATTACHMENT0, m_TexImg );


    // Init shaders used for color correction.
    std::string basename = PluginManager::getBaseDirPath().append("/shaders/img_patch_param/").toStdString();
    std::string logs;

    GPU::Shader::VertPg vpg;
    GPU::Shader::FragPg fpg;

    if( !vpg.CompileSrcFile( basename+"color_correction_init.vert", &logs )     ||
        !fpg.CompileSrcFile( basename+"color_correction_init.frag", &logs )     ||
        !m_PushPullShader_Init.Attach( vpg )
                              .AttachAndLink( fpg, &logs ) ||

        !vpg.CompileSrcFile( basename+"color_correction.vert", &logs )     ||
        !fpg.CompileSrcFile( basename+"color_correction_push.frag", &logs )     ||
        !m_PushPullShader_Push.Attach( vpg )
                              .AttachAndLink( fpg, &logs ) ||

        !fpg.CompileSrcFile( basename+"color_correction_pull.frag", &logs )     ||
        !m_PushPullShader_Pull.Attach( vpg )
                              .AttachAndLink( fpg, &logs )                      ||

        !fpg.CompileSrcFile( basename+"color_correction_combine.frag", &logs )  ||
        !m_PushPullShader_Combine.Attach( vpg )
                                 .AttachAndLink( fpg, &logs ) )

    {
        qWarning( (std::string(__func__)+": "+logs).c_str() );
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
    m_TexFB.Bind();
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );


    // TEXTURE PAINTING.
    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
    {
        QImage &rmImg = rp.key()->currentPlane->image;


        // Loads the raster into the GPU as a texture image.
        GPU::Texture2D rasterTex;
        GLubyte *rasterData = new GLubyte [ 3*rmImg.width()*rmImg.height() ];
        for( int y=rmImg.height()-1, n=0; y>=0; --y )
            for( int x=0; x<rmImg.width(); ++x, n+=3 )
            {
                QRgb p = rmImg.pixel(x,y);
                rasterData[n+0] = qRed  (p);
                rasterData[n+1] = qGreen(p);
                rasterData[n+2] = qBlue (p);
            }
        rasterTex.Create( GL_RGB, rmImg.width(), rmImg.height(), GL_RGB, GL_UNSIGNED_BYTE, rasterData );
        delete [] rasterData;
        rasterTex.SetFiltering( GL_LINEAR );
        rasterTex.Bind();


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
            vcg::glLoadMatrix( p->img2tex);
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

        rasterTex.Unbind();
        glMatrixMode( GL_TEXTURE );
        glPopMatrix();
    }

    m_TexFB.Unbind();


    // Restore the previous OpenGL state.
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glPopAttrib();
}


void TexturePainter::pushPullInit( RasterPatchMap &patches,
                                   GPU::Texture2D &diffTex,
                                   int filterSize )
{
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();


    GPU::FrameBuffer fbuffer( diffTex.Width(), diffTex.Height() );
    fbuffer.Attach( GL_COLOR_ATTACHMENT0, diffTex );
    fbuffer.Bind();
    m_TexImg.Bind( 0 );
    m_PushPullShader_Init.Bind();
    m_PushPullShader_Init.SetSampler( "u_Tex", 0 );
    m_PushPullShader_Init.SetUniform( "u_Radius", &filterSize );

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

    m_PushPullShader_Init.Unbind();
    m_TexImg.Unbind();
    fbuffer.Unbind();


    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
}


void TexturePainter::push( GPU::Texture2D &higherLevel,
                           GPU::Texture2D &lowerLevel )
{
    GPU::FrameBuffer fbuffer( lowerLevel.Width(), lowerLevel.Height() );
    fbuffer.Attach( GL_COLOR_ATTACHMENT0, lowerLevel );
    fbuffer.Bind();
    higherLevel.Bind( 0 );
    m_PushPullShader_Push.Bind();
    m_PushPullShader_Push.SetSampler( "u_Tex", 0 );

    glBegin( GL_QUADS );
        glVertex2i( -1, -1 );
        glVertex2i(  1, -1 );
        glVertex2i(  1,  1 );
        glVertex2i( -1,  1 );
    glEnd();

    m_PushPullShader_Push.Unbind();
    higherLevel.Unbind();
    fbuffer.Unbind();
}


void TexturePainter::pull( GPU::Texture2D &lowerLevel,
                           GPU::Texture2D &higherLevel )
{
    GPU::Texture2D tmp;
    tmp.Create( GL_RGBA32F, higherLevel.Width(), higherLevel.Height() );
    tmp.SetFiltering( GL_LINEAR );
    tmp.SetParam( GL_TEXTURE_WRAP_S, GL_CLAMP );
    tmp.SetParam( GL_TEXTURE_WRAP_T, GL_CLAMP );

    GPU::FrameBuffer fbuffer( tmp.Width(), tmp.Height() );
    fbuffer.Attach( GL_COLOR_ATTACHMENT0, tmp );
    fbuffer.Bind();
    lowerLevel.Bind( 0 );
    higherLevel.Bind( 1 );
    m_PushPullShader_Pull.Bind();
    m_PushPullShader_Pull.SetSampler( "u_TexLower", 0 );
    m_PushPullShader_Pull.SetSampler( "u_TexUpper", 1 );

    glBegin( GL_QUADS );
        glVertex2i( -1, -1 );
        glVertex2i(  1, -1 );
        glVertex2i(  1,  1 );
        glVertex2i( -1,  1 );
    glEnd();

    m_PushPullShader_Pull.Unbind();
    lowerLevel.Unbind();
    higherLevel.Unbind();
    fbuffer.Unbind();

    higherLevel = tmp;
}


void TexturePainter::apply( GPU::Texture2D &color,
                            GPU::Texture2D &correction )
{
    GPU::Texture2D tmp;
    tmp.Create( GL_RGB, color.Width(), color.Height() );
    tmp.SetFiltering( GL_NEAREST );
    m_TexFB.Attach( GL_COLOR_ATTACHMENT0, tmp );
    m_TexFB.Bind();

    color.Bind( 0 );
    correction.Bind( 1 );

    m_PushPullShader_Combine.Bind();
    m_PushPullShader_Combine.SetSampler( "u_TexColor", 0 );
    m_PushPullShader_Combine.SetSampler( "u_TexCorrection", 1 );

    glBegin( GL_QUADS );
        glVertex2i( -1, -1 );
        glVertex2i(  1, -1 );
        glVertex2i(  1,  1 );
        glVertex2i( -1,  1 );
    glEnd();

    correction.Unbind();
    color.Unbind();
    m_PushPullShader_Combine.Unbind();
    m_TexFB.Unbind();

    color = tmp;
}


void TexturePainter::rectifyColor( RasterPatchMap &patches, int filterSize )
{
    if( !isInitialized() )
        return;

    glPushAttrib( GL_ALL_ATTRIB_BITS );


    int nbLevels = std::ceil( std::log((float)m_TexImg.Width()) / std::log(2.0f) );
    std::vector<GPU::Texture2D> pushPullStack;
    pushPullStack.reserve( nbLevels+1 );


    pushPullStack.resize( 1 );
    pushPullStack[0].Create( GL_RGBA32F, m_TexImg.Width(), m_TexImg.Height() );
    pushPullStack[0].SetFiltering( GL_LINEAR );
    pushPullStack[0].SetParam( GL_TEXTURE_WRAP_S, GL_CLAMP );
    pushPullStack[0].SetParam( GL_TEXTURE_WRAP_T, GL_CLAMP );
    pushPullInit( patches, pushPullStack[0], filterSize );


    while( pushPullStack.back().Width() > 1 )
    {
        unsigned int newDim = (pushPullStack.back().Width()/2) + (pushPullStack.back().Width()&1);
        GPU::Texture2D newLevel;
        newLevel.Create( GL_RGBA32F, newDim, newDim );
        newLevel.SetFiltering( GL_LINEAR );
        newLevel.SetParam( GL_TEXTURE_WRAP_S, GL_CLAMP );
        newLevel.SetParam( GL_TEXTURE_WRAP_T, GL_CLAMP );

        push( pushPullStack.back(), newLevel );
        pushPullStack.push_back( newLevel );
    }


    for( int i=(int)pushPullStack.size()-2; i>=0; --i )
    {
        pull( pushPullStack[i+1], pushPullStack[i] );
        pushPullStack[i+1].Release();
    }


    apply( m_TexImg, pushPullStack[0] );


    glPopAttrib();
}


QImage TexturePainter::getTexture()
{
    if( !isInitialized() )
        return QImage();


    // Recovers the content of the off-screen painting buffer and returns it as a QImage object.
    GLubyte *texData = new GLubyte [ 3*m_TexFB.Width()*m_TexFB.Height() ];
    m_TexFB.DumpTo( GL_COLOR_ATTACHMENT0, texData, GL_RGB, GL_UNSIGNED_BYTE );

    QImage tex( m_TexFB.Width(), m_TexFB.Height(), QImage::Format_ARGB32 );
    for( int y=(int)m_TexFB.Height()-1, n=0; y>=0; --y )
        for( int x= 0; x<(int)m_TexFB .Width(); ++x, n+=3 )
            tex.setPixel( x, y, qRgb(texData[n+0],texData[n+1],texData[n+2]) );

    delete [] texData;
    return tex;
}
