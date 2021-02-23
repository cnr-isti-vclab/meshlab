/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#ifndef ML_SCENE_GL_SHARED_DATA_CONTEXT_H
#define ML_SCENE_GL_SHARED_DATA_CONTEXT_H

#include "ml_shared_data_context.h"

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
	typedef std::map<int, PerMeshMultiViewManager*> MeshIDManMap;
	MeshIDManMap _meshboman;
	vcg::QtThreadSafeMemoryInfo& _gpumeminfo;
	size_t _perbatchtriangles;
	size_t _minfacessmoothrendering;
	bool _highprecision;
	QTimer _timer;

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

#endif // ML_SCENE_GL_SHARED_DATA_CONTEXT_H
