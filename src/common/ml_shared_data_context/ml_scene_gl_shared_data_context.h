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

#include <QOpenGLWidget>
//#include <QOffscreenSurface>

//
// This is supposed to be the shared GL context used everywhere
//
// QOpenGLContext* is currently used as view id (it's the context of each GL view)
//
class MLSceneGLSharedDataContext : public QOpenGLWidget
{
	Q_OBJECT
public:
	// FIXME GL: the following must be checked
	//parent is set to NULL in order to avoid QT bug on MAC (business as usual...).
	//The QGLWidget are destroyed by hand in the MainWindow destructor...

	MLSceneGLSharedDataContext(MeshDocument& md, vcg::QtThreadSafeMemoryInfo& gpumeminfo, bool highprecision, size_t perbatchtriangles, size_t minfacespersmoothrendering = 0, QWidget * parent = nullptr);

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

	//Given a view id (QOpenGlContext*) the PerMeshRenderingDataMap contains the rendering data per each mesh contained in the MeshDocument (eg. flat/smooth shading? pervertex/perface/permesh color?)
	//and the 'meaningful' gl parameters used by the rendering system
	typedef QMap<int, MLRenderingData> PerMeshRenderingDataMap;

	MeshDocument& meshDoc() { return _md; }

	// FIXME GL: this function is called just once after the contructor to initialize the GLEW
	void initializeGL();
	void deAllocateGPUSharedData();

	void draw(int mmid, QOpenGLContext* viewid) const;
	void drawAllocatedAttributesSubset(int mmid, QOpenGLContext* viewid, const MLRenderingData& dt);
	void setSceneTransformationMatrix(const Matrix44m& m);
	void setMeshTransformationMatrix(int mmid, const Matrix44m& m);

	GLuint allocateTexturePerMesh(int mmid, const QImage& img, size_t maxdimtextmb);
	void deAllocateTexturesPerMesh(int mmid);
	GLuint getTextureId(int meshid, size_t position) const;


	void getRenderInfoPerMeshView(QOpenGLContext* ctx, PerMeshRenderingDataMap& map);
	void getRenderInfoPerMeshView(int mmid, QOpenGLContext* ctx, MLRenderingData& dt);
	void meshInserted(int mmid);
	void meshRemoved(int mmid);
	void manageBuffers(int mmid);
	void setDebugMode(int mmid, bool activatedebugmodality);
	void getLog(int mmid, MLRenderingData::DebugInfo& debug);
	bool isBORenderingAvailable(int mmid);


	/*functions intended for the plugins (they emit different signals according if the calling thread is different from the one where the MLSceneGLSharedDataContext object lives)*/
	void requestInitPerMeshView(QThread* callingthread, int meshid, QOpenGLContext* cont, const MLRenderingData& dt);
	void requestRemovePerMeshView(QThread* callingthread, QOpenGLContext* cont);
	void requestSetPerMeshViewRenderingData(QThread* callingthread, int meshid, QOpenGLContext* cont, const MLRenderingData& dt);
	void requestMeshAttributesUpdated(QThread* callingthread, int meshid, bool connectivitychanged, const MLRenderingData::RendAtts& dt);
	/***************************************/
	public slots:
	void meshDeallocated(int mmid);
	void setRenderingDataPerMeshView(int mmid, QOpenGLContext* viewerid, const MLRenderingData& perviewdata);
	void setRenderingDataPerAllMeshViews(int mmid, const MLRenderingData& perviewdata);
	void setGLOptions(int mmid, QOpenGLContext* viewid, const MLPerViewGLOptions& opts);

	void addView(QOpenGLContext* viewerid);
	void addView(QOpenGLContext* viewerid, MLRenderingData& dt);


	void removeView(QOpenGLContext* viewerid);
	void meshAttributesUpdated(int mmid, bool conntectivitychanged, const MLRenderingData::RendAtts& dt);
	void updateGPUMemInfo();
	//void updateRequested(int meshid,MLRenderingData::ATT_NAMES name);

	private slots:
	/*slots intended for the plugins living in another thread*/
	void initPerMeshViewRequested(int meshid, QOpenGLContext* cont, const MLRenderingData& dt);
	void removePerMeshViewRequested(QOpenGLContext* cont);
	void setPerMeshViewRenderingDataRequested(int meshid, QOpenGLContext* cont, const MLRenderingData& dt);
	void meshAttributesUpdatedRequested(int meshid, bool connectivitychanged, const MLRenderingData::RendAtts& dt);
	/***************************************/
private:
	typedef vcg::QtThreadSafeGLMeshAttributesMultiViewerBOManager<CMeshO, QOpenGLContext *, MLPerViewGLOptions> PerMeshMultiViewManager;
	PerMeshMultiViewManager* meshAttributesMultiViewerManager(int mmid) const;

	MeshDocument& _md;
	typedef std::map<int, PerMeshMultiViewManager*> MeshIDManMap;
	MeshIDManMap _meshboman;
	vcg::QtThreadSafeMemoryInfo& _gpumeminfo;
	size_t _perbatchtriangles;
	size_t _minfacessmoothrendering;
	bool _highprecision;
	QTimer _timer;

//	QOffscreenSurface _surface; // dummy offscreen surface to use makeCurrent() for this context

signals:

	void currentAllocatedGPUMem(int nv_all, int nv_current, int ati_tex, int ati_vbo);

	///*signals intended for the plugins living in the same thread*/
	//void initPerMeshViewRequestST(int,QGLContext*,const MLRenderingData&);
	//void removePerMeshViewRequestST(QGLContext*);
	//void setPerMeshViewRenderingDataRequestST(int,QGLContext*,const MLRenderingData&);
	///***************************************/

	/*signals intended for the plugins living in another thread*/
	void initPerMeshViewRequestMT(int, QOpenGLContext*, const MLRenderingData&);
	void removePerMeshViewRequestMT(QOpenGLContext*);
	void setPerMeshViewRenderingDataRequestMT(int, QOpenGLContext*, const MLRenderingData&);
	void meshAttributesUpdatedRequestMT(int, bool, const MLRenderingData::RendAtts&);
	/***************************************/
};

#endif // ML_SCENE_GL_SHARED_DATA_CONTEXT_H
