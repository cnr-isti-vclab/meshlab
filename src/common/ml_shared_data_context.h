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

	/*DIRTY THING IN ORDER TO AVOID GUI PROBLEMS WiTH FAUX EDGES RENDERING*/
	bool _peredge_wire_enabled;
	bool _peredge_fauxwire_enabled;

	bool _back_face_cull;
	bool _single_side_lighting;
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
		_peredge_vertmanifold_enabled = false;
		_peredge_text_boundary_enabled = false;
		_peredge_fauxwire_enabled = false;
		_peredge_wire_enabled = false;
		_back_face_cull = false;
		_single_side_lighting = true;
		_double_side_lighting = false;
		_fancy_lighting = false;
		_sel_enabled = false;
		_vertex_sel = true;
		_face_sel = true;

		_base_light_ambient_color = vcg::Color4b(32, 32, 32, 255);
		_base_light_diffuse_color = vcg::Color4b(204, 204, 204, 255);
		_base_light_specular_color = vcg::Color4b(255, 255, 255, 255);

		_fancy_lighting_enabled = false;
		_fancy_f_light_diffuse_color = vcg::Color4b(255, 204, 204, 255);
		_fancy_b_light_diffuse_color = vcg::Color4b(204, 204, 255, 255);
	}

	~MLPerViewGLOptions()
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


  size_t serialize(std::string& str) const
  {
    str.append(((_visible) ? "1" : "0"));
    str.append(((_perbbox_quoted_info_enabled) ? "1" : "0"));
    str.append(((_peredge_extra_enabled) ? "1" : "0"));
    str.append(((_peredge_edgeboundary_enabled) ? "1" : "0"));
    str.append(((_peredge_faceboundary_enabled) ? "1" : "0"));
    str.append(((_peredge_edgemanifold_enabled) ? "1" : "0"));
    str.append(((_peredge_vertmanifold_enabled) ? "1" : "0"));
    str.append(((_peredge_text_boundary_enabled) ? "1" : "0"));
    str.append(((_peredge_fauxwire_enabled) ? "1" : "0"));
    str.append(((_peredge_wire_enabled) ? "1" : "0"));
    str.append(((_back_face_cull) ? "1" : "0"));
    str.append(((_single_side_lighting) ? "1" : "0"));
    str.append(((_double_side_lighting) ? "1" : "0"));
    str.append(((_fancy_lighting) ? "1" : "0"));
    str.append(((_sel_enabled) ? "1" : "0"));
    str.append(((_vertex_sel) ? "1" : "0"));
    str.append(((_face_sel) ? "1" : "0"));

    str.append(((_perbbox_enabled) ? "1" : "0"));
    str.append(((_perbbox_fixed_color_enabled) ? "1" : "0"));
    str.append(((_perpoint_fixed_color_enabled) ? "1" : "0"));
    str.append(((_perwire_fixed_color_enabled) ? "1" : "0"));
    str.append(((_persolid_fixed_color_enabled) ? "1" : "0"));
    str.append(((_perbbox_mesh_color_enabled) ? "1" : "0"));
    str.append(((_perpoint_mesh_color_enabled) ? "1" : "0"));
    str.append(((_perwire_mesh_color_enabled) ? "1" : "0"));
    str.append(((_persolid_mesh_color_enabled) ? "1" : "0"));
    str.append(((_perpoint_dot_enabled) ? "1" : "0"));
    str.append(((_perpoint_noshading) ? "1" : "0"));
    str.append(((_perwire_noshading) ? "1" : "0"));
    str.append(((_persolid_noshading) ? "1" : "0"));
    str.append(((_perpoint_pointsmooth_enabled) ? "1" : "0"));
    str.append(((_perpoint_pointattenuation_enabled) ? "1" : "0"));

    return str.size();
  }

  void deserialize(std::string& str)
  {
    std::string s(str.rbegin(), str.rend());
    std::bitset<32> bset(s);
    _visible = bset[0];
    _perbbox_quoted_info_enabled = bset[1];
    _peredge_extra_enabled = bset[2];
    _peredge_edgeboundary_enabled = bset[3];
    _peredge_faceboundary_enabled = bset[4];
    _peredge_edgemanifold_enabled = bset[5];
    _peredge_vertmanifold_enabled = bset[6];
    _peredge_text_boundary_enabled = bset[7];
    _peredge_fauxwire_enabled = bset[8];
    _peredge_wire_enabled = bset[9];
    _back_face_cull = bset[10];
    _single_side_lighting = bset[11];
    _double_side_lighting = bset[12];
    _fancy_lighting = bset[13];
    _sel_enabled = bset[14];
    _vertex_sel = bset[15];
    _face_sel = bset[16];

    _perbbox_enabled = bset[17];
    _perbbox_fixed_color_enabled = bset[18];
    _perpoint_fixed_color_enabled = bset[19];
    _perwire_fixed_color_enabled = bset[20];
    _persolid_fixed_color_enabled = bset[21];
    _perbbox_mesh_color_enabled = bset[22];
    _perpoint_mesh_color_enabled = bset[23];
    _perwire_mesh_color_enabled = bset[24];
    _persolid_mesh_color_enabled = bset[25];
    _perpoint_dot_enabled = bset[26];
    _perpoint_noshading = bset[27];
    _perwire_noshading = bset[28];
    _persolid_noshading = bset[29];
    _perpoint_pointsmooth_enabled = bset[30];
    _perpoint_pointattenuation_enabled = bset[31];
  }

	static float minPointSize()
	{
		return 1.0f;
	}

	static float maxPointSize()
	{
		return 15.0f;
	}

	static float maxLineWidth()
	{
		return 5.0f;
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
		_peredge_fauxwire_enabled = opts._peredge_fauxwire_enabled;
		_peredge_wire_enabled = opts._peredge_wire_enabled;
		_back_face_cull = opts._back_face_cull;
		_single_side_lighting = opts._single_side_lighting;
		_double_side_lighting = opts._double_side_lighting;
		_fancy_lighting = opts._fancy_lighting;
		_sel_enabled = opts._sel_enabled;
		_face_sel = opts._face_sel;
		_vertex_sel = opts._vertex_sel;

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

	bool set(MLRenderingData::PRIMITIVE_MODALITY pm, const MLRenderingData::RendAtts& atts);
	bool set(MLRenderingData::PRIMITIVE_MODALITY pm, MLRenderingData::ATT_NAMES att, bool onoff);
	bool set(MLRenderingData::PRIMITIVE_MODALITY pm, bool onoff);
	void set(const MLPerViewGLOptions& opts);

	typedef vcg::GLMeshAttributesInfo::RendAtts RendAtts;
	typedef vcg::GLMeshAttributesInfo::ATT_NAMES ATT_NAMES;
	typedef vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY PRIMITIVE_MODALITY;
	typedef vcg::QtThreadSafeTextureNamesContainer MLTextureID;
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
	/*WARNING!!!!! TEMPORARY!!! TO BE DELETED*/
	static void computeRequestedRenderingDataCompatibleWithMeshSameGLOpts(MeshModel * meshmodel, const MLRenderingData & inputdt, MLRenderingData & outputdt);

	static void computeRequestedRenderingDataCompatibleWithMesh(MeshModel* meshmodel, const MLRenderingData& inputdt, MLRenderingData& outputdt);

	static void computeRequestedRenderingDataCompatibleWithMeshCommonCode(MeshModel * meshmodel, const MLRenderingData & inputdt, MLRenderingData & outputdt);

	static void fromMeshModelMaskToMLRenderingAtts(int meshmodelmask, MLRenderingData::RendAtts& atts);

	static void updatedRendAttsAccordingToPriorities(const MLRenderingData::PRIMITIVE_MODALITY pm, const MLRenderingData::RendAtts& updated, const MLRenderingData::RendAtts& current, MLRenderingData::RendAtts& result);

	static void maskMeaninglessAttributesPerPrimitiveModality(MLRenderingData::PRIMITIVE_MODALITY pm, MLRenderingData::RendAtts& atts);

	static void setAttributePriorities(MLRenderingData::RendAtts& atts);

	static void setPerViewGLOptionsPriorities(MLRenderingData& dt);

	//static void setPerViewRenderingDataPriorities(MeshModel* mm,MLRenderingData& dt);

	static void suggestedDefaultPerViewGLOptions(MLPerViewGLOptions& tmp);

	static void suggestedDefaultPerViewRenderingData(MeshModel* meshmodel, MLRenderingData& dt,size_t minpolnumpersmoothshading = 0);

	static void disableRedundatRenderingDataAccordingToPriorities(MLRenderingData& dt);

	static bool isPrimitiveModalityCompatibleWithMesh(MeshModel* m, const MLRenderingData::PRIMITIVE_MODALITY pm);

	static bool isPrimitiveModalityCompatibleWithMeshInfo(bool validvert, bool validfaces, bool validedges, int meshmask, const MLRenderingData::PRIMITIVE_MODALITY pm);

	static bool isPrimitiveModalityWorthToBeActivated(MLRenderingData::PRIMITIVE_MODALITY pm, bool wasvisualized, bool wasmeanigful, bool ismeaningful);

	static MLRenderingData::PRIMITIVE_MODALITY bestPrimitiveModalityAccordingToMesh(MeshModel* m);

	static void filterUselessUdpateAccordingToMeshMask(MeshModel* m, MLRenderingData::RendAtts& atts);

	static void setBestWireModality(MeshModel* mm, MLRenderingData& dt);

	static void setPerViewGLOptionsAccordindToWireModality(MeshModel* mm, MLRenderingData& dt);
};


class MLSceneGLSharedDataContext : public QGLWidget
{
	Q_OBJECT
public:
	//parent is set to NULL in order to avoid QT bug on MAC (business as usual...).
	//The QGLWidget are destroyed by hand in the MainWindow destructor...
	MLSceneGLSharedDataContext(MeshDocument& md, vcg::QtThreadSafeMemoryInfo& gpumeminfo, bool highprecision, size_t perbatchtriangles, size_t minfacespersmoothrendering = 0);

	~MLSceneGLSharedDataContext();

	void setMinFacesForSmoothRendering(size_t fcnum);

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
	typedef QMap<int, MLRenderingData> PerMeshRenderingDataMap;

	MeshDocument& meshDoc() { return _md; }

	void initializeGL();
	void deAllocateGPUSharedData();

	void draw(int mmid, QGLContext* viewid) const;
	void drawAllocatedAttributesSubset(int mmid, QGLContext* viewid, const MLRenderingData& dt);
	void setSceneTransformationMatrix(const Matrix44m& m);
	void setMeshTransformationMatrix(int mmid, const Matrix44m& m);

	GLuint allocateTexturePerMesh(int mmid, const QImage& img, size_t maxdimtextmb);
	void deAllocateTexturesPerMesh(int mmid);
	GLuint getTextureId(int meshid, size_t position) const;


	void getRenderInfoPerMeshView(QGLContext* ctx, PerMeshRenderingDataMap& map);
	void getRenderInfoPerMeshView(int mmid, QGLContext* ctx, MLRenderingData& dt);
	void meshInserted(int mmid);
	void meshRemoved(int mmid);
	bool manageBuffers(int mmid);
	void setDebugMode(int mmid, bool activatedebugmodality);
	void getLog(int mmid, MLRenderingData::DebugInfo& debug);
	bool isBORenderingAvailable(int mmid);


	/*functions intended for the plugins (they emit different signals according if the calling thread is different from the one where the MLSceneGLSharedDataContext object lives)*/
	void requestInitPerMeshView(QThread* callingthread, int meshid, QGLContext* cont, const MLRenderingData& dt);
	void requestRemovePerMeshView(QThread* callingthread, QGLContext* cont);
	void requestSetPerMeshViewRenderingData(QThread* callingthread, int meshid, QGLContext* cont, const MLRenderingData& dt);
	void requestMeshAttributesUpdated(QThread* callingthread, int meshid, bool connectivitychanged, const MLRenderingData::RendAtts& dt);
	/***************************************/
	public slots:
	void meshDeallocated(int mmid);
	void setRenderingDataPerMeshView(int mmid, QGLContext* viewerid, const MLRenderingData& perviewdata);
	void setRenderingDataPerAllMeshViews(int mmid, const MLRenderingData& perviewdata);
	void setGLOptions(int mmid, QGLContext* viewid, const MLPerViewGLOptions& opts);

	void addView(QGLContext* viewerid);
	void addView(QGLContext* viewerid, MLRenderingData& dt);


	void removeView(QGLContext* viewerid);
	void meshAttributesUpdated(int mmid, bool conntectivitychanged, const MLRenderingData::RendAtts& dt);
	void updateGPUMemInfo();
	//void updateRequested(int meshid,MLRenderingData::ATT_NAMES name);

	private slots:
	/*slots intended for the plugins living in another thread*/
	void initPerMeshViewRequested(int meshid, QGLContext* cont, const MLRenderingData& dt);
	void removePerMeshViewRequested(QGLContext* cont);
	void setPerMeshViewRenderingDataRequested(int meshid, QGLContext* cont, const MLRenderingData& dt);
	void meshAttributesUpdatedRequested(int meshid, bool connectivitychanged, const MLRenderingData::RendAtts& dt);
	/***************************************/
private:
	typedef vcg::QtThreadSafeGLMeshAttributesMultiViewerBOManager<CMeshO, QGLContext*, MLPerViewGLOptions> PerMeshMultiViewManager;
	PerMeshMultiViewManager* meshAttributesMultiViewerManager(int mmid) const;
	QGLContext* makeCurrentGLContext();
	void doneCurrentGLContext(QGLContext* oldone = NULL);

	MeshDocument& _md;
	typedef QMap<int, PerMeshMultiViewManager*> MeshIDManMap;
	MeshIDManMap _meshboman;
	vcg::QtThreadSafeMemoryInfo& _gpumeminfo;
	size_t _perbatchtriangles;
	size_t _minfacessmoothrendering;
	bool _highprecision;
	QTimer* _timer;

signals:

	void currentAllocatedGPUMem(int nv_all, int nv_current, int ati_tex, int ati_vbo);

	///*signals intended for the plugins living in the same thread*/
	//void initPerMeshViewRequestST(int,QGLContext*,const MLRenderingData&);
	//void removePerMeshViewRequestST(QGLContext*);
	//void setPerMeshViewRenderingDataRequestST(int,QGLContext*,const MLRenderingData&);
	///***************************************/

	/*signals intended for the plugins living in another thread*/
	void initPerMeshViewRequestMT(int, QGLContext*, const MLRenderingData&);
	void removePerMeshViewRequestMT(QGLContext*);
	void setPerMeshViewRenderingDataRequestMT(int, QGLContext*, const MLRenderingData&);
	void meshAttributesUpdatedRequestMT(int, bool, const MLRenderingData::RendAtts&);
	/***************************************/
};

class MLPluginGLContext : public QGLContext
{
public:
	MLPluginGLContext(const QGLFormat& frmt, QPaintDevice* dvc, MLSceneGLSharedDataContext& shared);
	~MLPluginGLContext();

	void initPerViewRenderingData(int meshid, MLRenderingData& dt);
	void removePerViewRenderindData();
	void setRenderingData(int meshid, MLRenderingData& dt);
	void drawMeshModel(int meshid) const;
	void meshAttributesUpdated(int meshid, bool conntectivitychanged, const MLRenderingData::RendAtts& dt);

	static void smoothModalitySuggestedRenderingData(MLRenderingData& dt);
	static void pointModalitySuggestedRenderingData(MLRenderingData& dt);
private:
	MLSceneGLSharedDataContext& _shared;
};

#endif
