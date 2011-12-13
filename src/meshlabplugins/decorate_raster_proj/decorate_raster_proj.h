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
#ifndef DECORATE_RASTER_PROJ_PLUGIN_H
#define DECORATE_RASTER_PROJ_PLUGIN_H




#include <QObject>
#include <common/interfaces.h>
#include "../filter_img_patch_param/GPU/GPU.h"




class DecorateRasterProjPlugin : public QObject, public MeshDecorateInterface
{
    Q_OBJECT
    Q_INTERFACES( MeshDecorateInterface )


    // Types.
    enum { DP_PROJECT_RASTER };

    typedef GPU::VBO< GPU::InterlacedBuffers,
                      GPU::Vertex3f         ,
                      GPU::Normal3f         ,
                      GPU::Indexui          >   MyVBO;

    class MeshDrawer
    {
        MyVBO               m_VBO;
        MeshModel           *m_Mesh;

    public:
        inline              MeshDrawer() : m_Mesh(NULL)                 {}
        inline              MeshDrawer( MeshModel *mm ) : m_Mesh(mm)    {}

        void                update( bool useVBO );
        void                drawShadow();
        void                draw();

        inline MeshModel*   mm()                                        { return m_Mesh; }
    };


    // Class variables.
    static bool             s_AreVBOSupported;


    // Member variables.
    bool                    m_ProjectOnAllMeshes;

    vcg::Box3f              m_SceneBox;
    QMap<int,MeshDrawer>    m_Scene;
    MeshDrawer              *m_CurrentMesh;

    RasterModel             *m_CurrentRaster;
    vcg::Matrix44f          m_RasterProj;
    vcg::Matrix44f          m_RasterPose;

    vcg::Matrix44f          m_ShadowProj;
    GPU::Texture2D          m_DepthTexture;
    GPU::Texture2D          m_ColorTexture;
    GPU::Shader             m_ShadowMapShader;


    // Constructors / destructor.
public:
    DecorateRasterProjPlugin();
    ~DecorateRasterProjPlugin();


    // Member functions.
private:
    void                    updateCurrentMesh( MeshDocument &m,
                                               RichParameterSet &par );
    void                    updateCurrentRaster( MeshDocument &m );

    void                    setPointParameters( MeshDrawer &md,
                                                RichParameterSet *par );
    void                    drawScene();
    void                    drawSceneShadow();
    void                    updateShadowProjectionMatrix();
    void                    updateColorTexture();
    void                    updateDepthTexture();

    bool                    initShaders( std::string &logs );


    virtual QString         decorationInfo( FilterIDType filter ) const;
    virtual QString         decorationName( FilterIDType filter ) const;

public:
	inline QList<QAction*>  actions() const                             { return actionList; }

    virtual bool            startDecorate( QAction          *act,
                                           MeshDocument     &m  ,
                                           RichParameterSet *par,
                                           GLArea           *gla );

    virtual void            decorate( QAction           *act,
                                      MeshDocument      &m  ,
                                      RichParameterSet  *par,
                                      GLArea            *gla,
                                      QPainter          *p  );

    virtual void            endDecorate( QAction          *act,
                                         MeshDocument     &m  ,
                                         RichParameterSet *par,
                                         GLArea           *gla );

    virtual void            initGlobalParameterSet( QAction *act, RichParameterSet &par );

    virtual int             getDecorationClass( QAction *act ) const;
};




#endif // DECORATE_RASTER_PROJ_PLUGIN_H
