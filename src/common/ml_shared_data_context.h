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

#ifndef __ML_SHARED_DATA_CONTEXT_H
#define __ML_SHARED_DATA_CONTEXT_H

#include <GL/glew.h>

#include "ml_mesh_type.h"
#include <wrap/qt/qt_thread_safe_mesh_attributes_multi_viewer_bo_manager.h>


#include <QGLWidget>
#include <QMap>

class MeshModel;
class MeshDocument;

struct MLBridgeStandAloneFunctions
{
    static void computeRequestedRenderingAttributesCompatibleWithMesh( MeshModel* meshmodel,const vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK& inputpm,const vcg::GLMeshAttributesInfo::RendAtts& inputatts, 
                                                                                         vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK& outputpm,     vcg::GLMeshAttributesInfo::RendAtts& outputatts );

    static void fromMeshModelMaskToMLRenderingAtts(int meshmodelmask,vcg::GLMeshAttributesInfo::RendAtts& atts);
    
    static void updatedRenderingAttsAddedToRenderingAttsAccordingToPriorities(const vcg::GLMeshAttributesInfo::RendAtts& updated,const vcg::GLMeshAttributesInfo::RendAtts& current,vcg::GLMeshAttributesInfo::RendAtts& result); 
};

struct MLPerViewGLOptions : public vcg::PerViewPerRenderingModalityGLOptions
{
    bool _visible;
    bool _lighting;
    bool _backfacecull;
    bool _doublesidelighting;
    bool _fancylighting;

     MLPerViewGLOptions()
        :vcg::PerViewPerRenderingModalityGLOptions(),_visible(true),_lighting(true),_backfacecull(false),_doublesidelighting(false),_fancylighting(false)
    {
    }

    ~ MLPerViewGLOptions()
    {
    }

     MLPerViewGLOptions(const  MLPerViewGLOptions& opts)
        :vcg::PerViewPerRenderingModalityGLOptions(opts)
    {
        copyData(opts);
    }
    
     MLPerViewGLOptions& operator=(const MLPerViewGLOptions& opts)
    {
        vcg::PerViewPerRenderingModalityGLOptions::operator=(opts);
        copyData(opts);
        return (*this);
    }
private:
    void copyData(const  MLPerViewGLOptions& opts)
    {
        _visible = opts._visible;
        _lighting = opts._lighting;
        _backfacecull = opts._backfacecull;
        _doublesidelighting = opts._doublesidelighting;
        _fancylighting = opts._fancylighting;
    }
};

struct MLRenderingData
{
    vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK _mask;
    vcg::GLMeshAttributesInfo::RendAtts _atts;
    MLPerViewGLOptions _opts;

    MLRenderingData()
        :_mask((unsigned int) vcg::GLMeshAttributesInfo::PR_NONE),_atts(),_opts()
    {
    }

    MLRenderingData(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK mask,const vcg::GLMeshAttributesInfo::RendAtts& atts,const MLPerViewGLOptions& opts)
        :_mask(mask),_atts(atts),_opts(opts)
    {
    }

    static void all(MLRenderingData& dt)
    {
        for(unsigned int ii = 0;ii < vcg::GLMeshAttributesInfo::ATT_NAMES::enumArity();++ii)
            dt._atts[ii] = true;
        dt._mask = vcg::GLMeshAttributesInfo::PR_BBOX | vcg::GLMeshAttributesInfo::PR_POINTS | vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES | vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES | vcg::GLMeshAttributesInfo::PR_SOLID;
    }
};

class MLSceneGLSharedDataContext : public QGLWidget
{
    Q_OBJECT
public:
    //parent is set to NULL in order to avoid QT bug on MAC (business as usual...).
    //The QGLWidget are destroyed by hand in the MainWindow destructor...
    MLSceneGLSharedDataContext(MeshDocument& md,vcg::QtThreadSafeMemoryInfo& gpumeminfo,bool highprecision,size_t perbatchtriangles);

    ~MLSceneGLSharedDataContext();

    vcg::QtThreadSafeMemoryInfo& memoryInfoManager() const
    {
        return _gpumeminfo;
    }

    inline bool highPrecisionRendering() const
    {
        return _highprecision;
    }
    

    typedef QMap<int,MLRenderingData> PerMeshRenderingDataMap; 

    void initializeGL();
    void deAllocateGPUSharedData();

    void draw(int mmid,QGLContext* viewid);
    void setSceneTransformationMatrix(const Matrix44m& m);
    void setMeshTransformationMatrix(int mmid,const Matrix44m& m);
    
    GLuint allocateTexturePerMesh(int mmid,const QImage& img,size_t maxdimtextmb);
    void deAllocateTexturesPerMesh(int mmid);

    void getRenderInfoPerMeshView(QGLContext* ctx,PerMeshRenderingDataMap& map);
    void getRenderInfoPerMeshView(int mmid,QGLContext* ctx,MLRenderingData& dt);
    void meshInserted(int mmid);
    void meshRemoved(int mmid);
    void manageBuffers(int mmid);
public slots:
    void meshDeallocated(int mmid);
    void setRequestedAttributesPerMeshViews(int mmid,const QList<QGLContext*>& viewerid,const MLRenderingData& perviewdata);
    void setRequestedAttributesPerMeshView(int mmid,QGLContext* viewerid,const MLRenderingData& perviewdata);
    void setGLOptions(int mmid,QGLContext* viewid,const MLPerViewGLOptions& opts);

    void addView(QGLContext* viewerid,const MLRenderingData& dt);
    void removeView(QGLContext* viewerid);
    void meshAttributesUpdated(int mmid,bool conntectivitychanged,const vcg::GLMeshAttributesInfo::RendAtts& dt);

private:
    typedef vcg::QtThreadSafeGLMeshAttributesMultiViewerBOManager<CMeshO,QGLContext*,MLPerViewGLOptions> PerMeshMultiViewManager; 
    PerMeshMultiViewManager* meshAttributesMultiViewerManager(int mmid ) const;

    MeshDocument& _md;	
    typedef QMap<int,PerMeshMultiViewManager*> MeshIDManMap;
    MeshIDManMap _meshboman;
    vcg::QtThreadSafeMemoryInfo& _gpumeminfo;
    size_t _perbatchtriangles;
    bool _highprecision;
}; 

#endif
