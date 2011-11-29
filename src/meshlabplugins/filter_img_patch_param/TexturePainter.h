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
#include "GPU/GPU.h"


class TexturePainter
{
protected:
    bool                m_IsInitialized;
    GPU::Texture2D      m_TexImg;
    GPU::FrameBuffer    m_TexFB;
    GPU::Shader         m_PushPullShader_Init;
    GPU::Shader         m_PushPullShader_Push;
    GPU::Shader         m_PushPullShader_Pull;
    GPU::Shader         m_PushPullShader_Combine;

    virtual bool    init( int texSize );

    void            pushPullInit( RasterPatchMap &patches,
                                  GPU::Texture2D &diffTex,
                                  int filterSize );
    void            push( GPU::Texture2D &higherLevel,
                          GPU::Texture2D &lowerLevel );
    void            pull( GPU::Texture2D &lowerLevel,
                          GPU::Texture2D &higherLevel );
    void            apply( GPU::Texture2D &color,
                           GPU::Texture2D &correction );

public:
    inline          TexturePainter( int texSize )   { m_IsInitialized = init(texSize); }

    void            paint( RasterPatchMap &patches );
    void            rectifyColor( RasterPatchMap &patches,
                                  int filterSize );
    inline bool     isInitialized() const           { return m_IsInitialized; }

    QImage          getTexture();
};




#endif // FILTER_IMG_PATCH_PARAM_PLUGIN__TEXTUREPAINTER_H
