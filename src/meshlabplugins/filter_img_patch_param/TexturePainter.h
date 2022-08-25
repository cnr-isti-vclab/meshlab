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

#ifndef FILTER_IMG_PATCH_PARAM_PLUGIN__TEXTUREPAINTER_H
#define FILTER_IMG_PATCH_PARAM_PLUGIN__TEXTUREPAINTER_H




#include "Patch.h"
#include <wrap/glw/glw.h>


class TexturePainter
{
protected:
    glw::Context            &m_Context;
    bool                    m_IsInitialized;
    glw::Texture2DHandle    m_TexImg;
    glw::FramebufferHandle  m_TexFB;
    glw::ProgramHandle      m_PushPullShader_Init;
    glw::ProgramHandle      m_PushPullShader_Push;
    glw::ProgramHandle      m_PushPullShader_Pull;
    glw::ProgramHandle      m_PushPullShader_Combine;

    virtual bool    init( int texSize );

    void            pushPullInit( RasterPatchMap &patches,
                                  glw::Texture2DHandle &diffTex,
                                  int filterSize );
    void            push( glw::Texture2DHandle &higherLevel,
                          glw::Texture2DHandle &lowerLevel );
    void            pull( glw::Texture2DHandle &lowerLevel,
                          glw::Texture2DHandle &higherLevel );
    void            apply( glw::Texture2DHandle &color,
                           glw::Texture2DHandle &correction );

public:
    inline          TexturePainter( glw::Context &ctx,
                                    int texSize ) : m_Context(ctx)  { m_IsInitialized = init(texSize); }

    void            paint( RasterPatchMap &patches );
    void            rectifyColor( RasterPatchMap &patches,
                                  int filterSize );
    inline bool     isInitialized() const                           { return m_IsInitialized; }

    QImage          getTexture(glw::FramebufferHandle &fbh, glw::Texture2DHandle &txh);
    QImage          getTexture();
};




#endif // FILTER_IMG_PATCH_PARAM_PLUGIN__TEXTUREPAINTER_H
