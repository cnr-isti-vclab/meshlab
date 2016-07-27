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
#include <QTimer>

#include "ml_mesh_type.h"
#include <wrap/qt/qt_thread_safe_mesh_attributes_multi_viewer_bo_manager.h>


#include <QGLWidget>
#include <QMap>

class MeshModel;
class MeshDocument;

struct MLPerViewGLOptions : public vcg::RenderingModalityGLOptions
{
    /*IN THIS MOMENT IT'S IGNORED! for the rendering process we use the flag contained in the meshmodel class*/
    bool _visible;
    /*****************************/

    bool _perbbox_quoted_info_enabled;

    bool _peredge_extra_enabled;
    bool _peredge_edgeboundary_enabled;
    bool _peredge_faceboundary_enabled;
    bool _peredge_edgemanifold_enabled;
    bool _peredge_vertmanifold_enabled;
    bool _peredge_text_boundary_enabled;

    bool _back_face_cull;
    bool _double_side_lighting;
    bool _fancy_lighting;
    
    vcg::Color4b _base_light_ambient_color;
    vcg::Color4b _base_light_diffuse_color;
    vcg::Color4b _base_light_specular_color;

    bool _fancy_lighting_enabled;
    vcg::Color4b _fancy_f_light_diffuse_color;
    vcg::Color4b _fancy_b_light_diffuse_color;

    bool _sel_enabled;
    bool _vertex_sel;
    bool _face_sel;


     MLPerViewGLOptions()
         :vcg::RenderingModalityGLOptions()
     {
        _visible = true;
        _perbbox_quoted_info_enabled = false;
        _peredge_extra_enabled = false;
        _peredge_edgeboundary_enabled = false;
        _peredge_faceboundary_enabled = false;
        _peredge_edgemanifold_enabled = false;
        _peredge_vertmanifold_enabled  = false;
        _peredge_text_boundary_enabled = false;
        _back_face_cull = false;
        _double_side_lighting = false;
        _fancy_lighting = false;
        _sel_enabled = false;    
        _vertex_sel = true;
        _face_sel = true;
        
        _base_light_ambient_color = vcg::Color4b(32,32,32,255); 
        _base_light_diffuse_color = vcg::Color4b(204,204,204,255);
        _base_light_specular_color = vcg::Color4b(255,255,255,255);

        _fancy_lighting_enabled = false;
        _fancy_f_light_diffuse_color = vcg::Color4b(255,204,204,255);
        _fancy_b_light_diffuse_color = vcg::Color4b(204,204,255,255);
     }

     ~ MLPerViewGLOptions()
     {
     }

     MLPerViewGLOptions(const  MLPerViewGLOptions& opts)
        :vcg::RenderingModalityGLOptions(opts)
    {
        copyData(opts);
    }
    
     MLPerViewGLOptions& operator=(const MLPerViewGLOptions& opts)
    {
        vcg::RenderingModalityGLOptions::operator=(opts);
        copyData(opts);
        return (*this);
    }
private:
    void copyData(const  MLPerViewGLOptions& opts)
    {
        _visible = opts._visible;
        _perbbox_quoted_info_enabled = opts._perbbox_quoted_info_enabled;
        _peredge_extra_enabled = opts._peredge_extra_enabled;
        _peredge_edgeboundary_enabled = opts._peredge_edgeboundary_enabled;
        _peredge_faceboundary_enabled = opts._peredge_faceboundary_enabled;
        _peredge_edgemanifold_enabled = opts._peredge_edgemanifold_enabled;
        _peredge_vertmanifold_enabled = opts._peredge_vertmanifold_enabled;
        _peredge_text_boundary_enabled = opts._peredge_text_boundary_enabled;
        _back_face_cull = opts._back_face_cull;
        _double_side_lighting = opts._double_side_lighting;
        _fancy_lighting = opts._fancy_lighting;
        _sel_enabled = opts._sel_enabled;
        _face_sel = opts._face_sel;

        _base_light_ambient_color = opts._base_light_ambient_color;
        _base_light_diffuse_color = opts._base_light_diffuse_color;
        _base_light_specular_color = opts._base_light_specular_color;

        _fancy_lighting_enabled = opts._fancy_lighting_enabled;
        _fancy_f_light_diffuse_color = opts._fancy_f_light_diffuse_color;
        _fancy_b_light_diffuse_color = opts._fancy_b_light_diffuse_color;
    }
};

class MLRenderingData : public vcg::PerViewData<MLPerViewGLOptions>
{
public:
    MLRenderingData();
    MLRenderingData(const MLRenderingData& dt);

    bool set(MLRenderingData::PRIMITIVE_MODALITY pm,const MLRenderingData::RendAtts& atts);
    bool set(MLRenderingData::PRIMITIVE_MODALITY pm,MLRenderingData::ATT_NAMES att,bool onoff);
    bool set(MLRenderingData::PRIMITIVE_MODALITY pm,bool onoff);
    void set(const MLPerViewGLOptions& opts);

    typedef vcg::GLMeshAttributesInfo::RendAtts RendAtts;
    typedef vcg::GLMeshAttributesInfo::ATT_NAMES ATT_NAMES; 
    typedef vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY PRIMITIVE_MODALITY;
};
/*{
    MLRenderingData::PRIMITIVE_MODALITY_MASK _mask;
    MLRenderingData::RendAtts _atts;
    MLPerViewGLOptions _opts;

    MLRenderingData()
        :_mask((unsigned int) MLRenderingData::PR_NONE),_atts(),_opts()
    {
    }

    MLRenderingData(MLRenderingData::PRIMITIVE_MODALITY_MASK mask,const MLRenderingData::RendAtts& atts,const MLPerViewGLOptions& opts)
        :_mask(mask),_atts(atts),_opts(opts)
    {
    }

    static void all(MLRenderingData& dt)
    {
        for(unsigned int ii = 0;ii < MLRenderingData::ATT_NAMES::enumArity();++ii)
            dt._atts[ii] = true;
        dt._mask = MLRenderingData::PR_BBOX | MLRenderingData::PR_POINTS | MLRenderingData::PR_WIREFRAME_EDGES | MLRenderingData::PR_WIREFRAME_TRIANGLES | MLRenderingData::PR_SOLID;
   }
};*/

struct MLPoliciesStandAloneFunctions
{
    static void computeRequestedRenderingDataCompatibleWithMesh( MeshModel* meshmodel,const MLRenderingData& inputdt,MLRenderingData& outputdt);

    static void fromMeshModelMaskToMLRenderingAtts(int meshmodelmask,MLRenderingData::RendAtts& atts);

    static void updatedRendAttsAccordingToPriorities(const MLRenderingData::PRIMITIVE_MODALITY pm,const MLRenderingData::RendAtts& updated,const MLRenderingData::RendAtts& current,MLRenderingData::RendAtts& result); 

    static void maskMeaninglessAttributesPerPrimitiveModality( MLRenderingData::PRIMITIVE_MODALITY pm,MLRenderingData::RendAtts& atts );

    static void setAttributePriorities(MLRenderingData::RendAtts& atts);

    static void setPerViewGLOptionsPriorities(MeshModel* mm,MLRenderingData& dt );

    //static void setPerViewRenderingDataPriorities(MeshModel* mm,MLRenderingData& dt);

    static void suggestedDefaultPerViewGLOptions(MLPerViewGLOptions& tmp);

    static void suggestedDefaultPerViewRenderingData(MeshModel* meshmodel,MLRenderingData& dt);

    static bool isPrimitiveModalityCompatibleWithMesh(MeshModel* m,const MLRenderingData::PRIMITIVE_MODALITY pm);

    static bool isPrimitiveModalityCompatibleWithMeshInfo(bool validvert,bool validfaces,bool validedges,int meshmask,const MLRenderingData::PRIMITIVE_MODALITY pm);

    static bool isPrimitiveModalityWorthToBeActivated(MLRenderingData::PRIMITIVE_MODALITY pm,bool wasvisualized,bool wasmeanigful,bool ismeaningful);
    
    static MLRenderingData::PRIMITIVE_MODALITY bestPrimitiveModalityAccordingToMesh(MeshModel* m);

    static void filterUselessUdpateAccordingToMeshMask(MeshModel* m,MLRenderingData::RendAtts& atts);   
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
    
    //Given a QGLContext the PerMeshRenderingDataMap contains the rendering data per each mesh contained in the MeshDocument (eg. flat/smooth shading? pervertex/perface/permesh color?) 
    //and the 'meaningful' gl parameters used by the rendering system
    typedef QMap<int,MLRenderingData> PerMeshRenderingDataMap; 

    MeshDocument& meshDoc() {return _md;}

    void initializeGL();
    void deAllocateGPUSharedData();

    void draw(int mmid,QGLContext* viewid) const;
    void setSceneTransformationMatrix(const Matrix44m& m);
    void setMeshTransformationMatrix(int mmid,const Matrix44m& m);
    
    GLuint allocateTexturePerMesh(int mmid,const QImage& img,size_t maxdimtextmb);
    void deAllocateTexturesPerMesh(int mmid);

    void getRenderInfoPerMeshView(QGLContext* ctx,PerMeshRenderingDataMap& map);
    void getRenderInfoPerMeshView(int mmid,QGLContext* ctx,MLRenderingData& dt);
    void meshInserted(int mmid);
    void meshRemoved(int mmid);
    void manageBuffers(int mmid);
    void setDebugMode(int mmid,bool activatedebugmodality);
    void getLog(int mmid,MLRenderingData::DebugInfo& debug);
    bool isBORenderingAvailable(int mmid);

    /*functions intended for the plugins (they emit different signals according if the calling thread is different from the one where the MLSceneGLSharedDataContext object lives)*/
    void requestInitPerMeshView(QThread* callingthread,int meshid,QGLContext* cont,const MLRenderingData& dt);
    void requestRemovePerMeshView(QThread* callingthread,QGLContext* cont);
    void requestSetPerMeshViewRenderingData(QThread* callingthread,int meshid,QGLContext* cont,const MLRenderingData& dt);
    void requestMeshAttributesUpdated(QThread* callingthread,int meshid,bool connectivitychanged,const MLRenderingData::RendAtts& dt );
    /***************************************/
public slots:
    void meshDeallocated(int mmid);
    void setRenderingDataPerMeshView(int mmid,QGLContext* viewerid,const MLRenderingData& perviewdata);
    void setGLOptions(int mmid,QGLContext* viewid,const MLPerViewGLOptions& opts);

    void addView(QGLContext* viewerid);
    void addView(QGLContext* viewerid,MLRenderingData& dt);
	

    void removeView(QGLContext* viewerid);
    void meshAttributesUpdated(int mmid,bool conntectivitychanged,const MLRenderingData::RendAtts& dt);
    void updateGPUMemInfo();
    //void updateRequested(int meshid,MLRenderingData::ATT_NAMES name);

private slots:
    /*slots intended for the plugins living in another thread*/
    void initPerMeshViewRequested(int meshid,QGLContext* cont,const MLRenderingData& dt);
    void removePerMeshViewRequested(QGLContext* cont);
    void setPerMeshViewRenderingDataRequested(int meshid,QGLContext* cont,const MLRenderingData& dt);
    void meshAttributesUpdatedRequested(int meshid,bool connectivitychanged,const MLRenderingData::RendAtts& dt);
    /***************************************/
private:
    typedef vcg::QtThreadSafeGLMeshAttributesMultiViewerBOManager<CMeshO,QGLContext*,MLPerViewGLOptions> PerMeshMultiViewManager; 
    PerMeshMultiViewManager* meshAttributesMultiViewerManager(int mmid ) const;
    QGLContext* makeCurrentGLContext();
    void doneCurrentGLContext(QGLContext* oldone = NULL);

    MeshDocument& _md;	
    typedef QMap<int,PerMeshMultiViewManager*> MeshIDManMap;
    MeshIDManMap _meshboman;
    vcg::QtThreadSafeMemoryInfo& _gpumeminfo;
    size_t _perbatchtriangles;
    bool _highprecision;
    QTimer* _timer;

signals:
    
    void currentAllocatedGPUMem(int all,int current);

    ///*signals intended for the plugins living in the same thread*/
    //void initPerMeshViewRequestST(int,QGLContext*,const MLRenderingData&);
    //void removePerMeshViewRequestST(QGLContext*);
    //void setPerMeshViewRenderingDataRequestST(int,QGLContext*,const MLRenderingData&);
    ///***************************************/

    /*signals intended for the plugins living in another thread*/
    void initPerMeshViewRequestMT(int,QGLContext*,const MLRenderingData&);
    void removePerMeshViewRequestMT(QGLContext*);
    void setPerMeshViewRenderingDataRequestMT(int,QGLContext*,const MLRenderingData&);
    void meshAttributesUpdatedRequestMT(int,bool,const MLRenderingData::RendAtts&);
    /***************************************/
}; 

class MLPluginGLContext : public QGLContext
{
public:
    MLPluginGLContext(const QGLFormat& frmt,QPaintDevice* dvc,MLSceneGLSharedDataContext& shared);
    ~MLPluginGLContext();

    void initPerViewRenderingData(int meshid,MLRenderingData& dt);
    void removePerViewRenderindData();
    void setRenderingData(int meshid,MLRenderingData& dt);
    void drawMeshModel( int meshid) const;
    void meshAttributesUpdated(int meshid,bool conntectivitychanged,const MLRenderingData::RendAtts& dt);

    static void smoothModalitySuggestedRenderingData(MLRenderingData& dt);
    static void pointModalitySuggestedRenderingData(MLRenderingData& dt); 
private:
    MLSceneGLSharedDataContext& _shared;
};

#endif
