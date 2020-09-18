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
#include <common/interfaces/decorate_plugin_interface.h>
#include <common/ml_shared_data_context.h>
#include <common/meshmodel.h>
#include <wrap/glw/glw.h>




class DecorateRasterProjPlugin : public QObject, public DecoratePluginInterface
{
    Q_OBJECT
    MESHLAB_PLUGIN_IID_EXPORTER(DECORATE_PLUGIN_INTERFACE_IID)
    Q_INTERFACES( DecoratePluginInterface )


    // Types.
    enum { DP_PROJECT_RASTER };

    class MeshDrawer
    {
        MeshModel           *m_Mesh;

    public:
        inline              MeshDrawer() : m_Mesh(NULL)                 {}
        inline              MeshDrawer( MeshModel *mm ) : m_Mesh(mm)    {}

        //void                update(MLSceneGLSharedDataContext* ctx);
        void                drawShadow(QGLContext* glctx, MLSceneGLSharedDataContext* ctx);
        void                draw(QGLContext* glctx, MLSceneGLSharedDataContext* ctx);

        inline MeshModel*   mm()                                        { return m_Mesh; }
    };


    // Class variables.
    //static bool             s_AreVBOSupported;
	

    // Member variables.
    glw::Context            m_Context;
    bool                    m_ProjectOnAllMeshes;

    Box3m              m_SceneBox;
    QMap<int,MeshDrawer>    m_Scene;
    MeshDrawer              *m_CurrentMesh;

    RasterModel             *m_CurrentRaster;
    Matrix44m          m_RasterProj;
    Matrix44m          m_RasterPose;

    Matrix44m          m_ShadowProj;
    glw::Texture2DHandle    m_DepthTexture;
    glw::Texture2DHandle    m_ColorTexture;
    glw::ProgramHandle      m_ShadowMapShader;



    // Constructors / destructor.
public:
    DecorateRasterProjPlugin();
    ~DecorateRasterProjPlugin();
    QString pluginName() const;


    // Member functions.
private:
    void                    updateCurrentMesh( MeshDocument &m,
											   const RichParameterList &par );
    void                    updateCurrentRaster( MeshDocument &m, QGLContext* glctx, MLSceneGLSharedDataContext* ctx);

	void                    setPointParameters(MeshDrawer &md,
												const RichParameterList* par );
    void                    drawScene();
    void                    drawSceneShadow();
    void                    updateShadowProjectionMatrix();
    void                    updateColorTexture();
    void                    updateDepthTexture(QGLContext* glctx, MLSceneGLSharedDataContext* ctx);

    bool                    initShaders(std::string &logs);


    virtual QString         decorationInfo( FilterIDType filter ) const;
    virtual QString         decorationName( FilterIDType filter ) const;

public:
    inline QList<QAction*>  actions() const                             { return actionList; }

	bool  startDecorate(const QAction  *act, MeshDocument &m, const RichParameterList *par, GLArea *gla );
	void  decorateMesh(const QAction  *   , MeshModel    & , const RichParameterList *   , GLArea *   , QPainter * , GLLogStream &  ) {}
	void  decorateDoc(const QAction  *act, MeshDocument &m, const RichParameterList* par, GLArea *gla, QPainter *p, GLLogStream &  );
	void  endDecorate(const QAction* act, MeshDocument &m, const RichParameterList *par, GLArea *gla );
    void  initGlobalParameterSet( QAction *act, RichParameterList &par );
    int   getDecorationClass(const QAction* act ) const;
};




#endif // DECORATE_RASTER_PROJ_PLUGIN_H
