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

#include "ml_scene_gl_shared_data_context.h"
#include "../ml_document/mesh_document.h"
#include "../GLExtensionsManager.h"


MLSceneGLSharedDataContext::MLSceneGLSharedDataContext(MeshDocument& md,vcg::QtThreadSafeMemoryInfo& gpumeminfo,bool highprecision,size_t perbatchtriangles, size_t minfacespersmoothrendering)
	:QGLWidget(),_md(md),_gpumeminfo(gpumeminfo),_perbatchtriangles(perbatchtriangles), _minfacessmoothrendering(minfacespersmoothrendering),_highprecision(highprecision),_timer(this)
{
	//if (md.size() != 0)
	//    throw MLException(QString("MLSceneGLSharedDataContext: MeshDocument is not empty when MLSceneGLSharedDataContext is constructed."));

	connect(&_timer,SIGNAL(timeout()),this,SLOT(updateGPUMemInfo()));

	/*connection intended for the plugins living in another thread*/
	connect(this,SIGNAL(initPerMeshViewRequestMT(int,QGLContext*,const MLRenderingData&)),this,SLOT(initPerMeshViewRequested(int,QGLContext*,const MLRenderingData&)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(removePerMeshViewRequestMT(QGLContext*)),this,SLOT(removePerMeshViewRequested(QGLContext*)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(setPerMeshViewRenderingDataRequestMT(int,QGLContext*,const MLRenderingData&)),this,SLOT(setPerMeshViewRenderingDataRequested(int,QGLContext*,const MLRenderingData&)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(meshAttributesUpdatedRequestMT(int,bool,const MLRenderingData::RendAtts&)),this,SLOT(meshAttributesUpdatedRequested(int,bool,const MLRenderingData::RendAtts&)),Qt::BlockingQueuedConnection);
	/****************************************************************/

	///*connection intended for the plugins living in the same thread*/
	//connect(this,SIGNAL(initPerMeshViewRequestST(int,QGLContext*,const MLRenderingData&)),this,SLOT(initPerMeshViewRequested(int,QGLContext*,const MLRenderingData&)),Qt::DirectConnection);
	//connect(this,SIGNAL(removePerMeshViewRequestST(QGLContext*)),this,SLOT(removePerMeshViewRequested(QGLContext*)),Qt::DirectConnection);
	//connect(this,SIGNAL(setPerMeshViewRenderingDataRequestST(int,QGLContext*,const MLRenderingData&)),this,SLOT(setPerMeshViewRenderingDataRequested(int,QGLContext*,const MLRenderingData&)),Qt::DirectConnection);
	///****************************************************************/

	_timer.start(1000);
	updateGPUMemInfo();
	
	//if in the document there are already some meshes, we insert them here....
	MeshModel* mm = _md.nextMesh();
	do {
		if (mm != nullptr)
			meshInserted(mm->id());
		mm = _md.nextMesh(mm);
	} while (mm != nullptr);
}

MLSceneGLSharedDataContext::~MLSceneGLSharedDataContext()
{
	MeshModel* mm = _md.nextMesh();
	do {
		if (mm != nullptr)
			meshRemoved(mm->id());
		mm = _md.nextMesh(mm);
	} while (mm != nullptr);
}

void MLSceneGLSharedDataContext::setMinFacesForSmoothRendering(size_t fcnum)
{
	_minfacessmoothrendering = fcnum;
}

MLSceneGLSharedDataContext::PerMeshMultiViewManager* MLSceneGLSharedDataContext::meshAttributesMultiViewerManager( int mmid ) const
{
	MeshIDManMap::const_iterator it = _meshboman.find(mmid);
	if (it == _meshboman.end())
		return NULL;
	return it->second;
}

void MLSceneGLSharedDataContext::initializeGL()
{
	QGLContext *ctx = makeCurrentGLContext();
	GLExtensionsManager::initializeGLextensions();
	doneCurrentGLContext(ctx);
}

void MLSceneGLSharedDataContext::setRenderingDataPerMeshView( int mmid,QGLContext* viewerid,const MLRenderingData& perviewdata )
{
	MeshModel* mm = _md.getMesh(mmid);
	if (mm == NULL)
		return;
	MLSceneGLSharedDataContext::PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->setPerViewInfo(viewerid,perviewdata);
}

void MLSceneGLSharedDataContext::setRenderingDataPerAllMeshViews(int mmid, const MLRenderingData& perviewdata)
{
	MeshModel* mm = _md.getMesh(mmid);
	if (mm == NULL)
		return;
	MLSceneGLSharedDataContext::PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->setPerAllViewsInfo(perviewdata);
}

//void MLSceneGLSharedDataContext::setRequestedAttributesPerMeshViews( int mmid,const QList<QGLContext*>& viewerid,const MLRenderingData& perviewdata )
//{
//    MeshModel* mm = _md.getMesh(mmid);
//    if (mm == NULL)
//        return;
//
//    PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
//    if (man != NULL)
//    {
//        MLRenderingData outdt;
//        MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh(mm,perviewdata,outdt);
//        foreach(QGLContext* vid,viewerid)
//        {
//            setRequestedAttributesPerMeshView(mmid,vid,outdt);
//        }
//    }
//}


void MLSceneGLSharedDataContext::deAllocateTexturesPerMesh( int mmid )
{
	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
	{
		QGLContext* ctx = makeCurrentGLContext();
		std::vector<GLuint> texids;
		for(size_t ii = 0;ii < man->textureIDContainer().size();++ii)
		{
			texids.push_back(man->textureIDContainer()[ii]);
		}

		for (auto tex : texids)
			man->textureIDContainer().remove(tex);

		glDeleteTextures(texids.size(), texids.data());
		doneCurrentGLContext(ctx);
	}
}

GLuint MLSceneGLSharedDataContext::getTextureId(int meshid, size_t position) const
{
	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(meshid);
	if ((man != NULL) && (position < man->textureIDContainer().size()))
	{
		return man->textureIDContainer()[position];
	}
	return 0;
}

int roundUpToTheNextHighestPowerOf2(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

GLuint MLSceneGLSharedDataContext::allocateTexturePerMesh( int meshid,const QImage& img,size_t maxdimtextmpx)
{
	GLuint res = 0;
	PerMeshMultiViewManager* meshfeed = meshAttributesMultiViewerManager(meshid);
	if (meshfeed != NULL)
	{
		int singlemaxtexturesize = roundUpToTheNextHighestPowerOf2(int(sqrt(float(maxdimtextmpx))*1024.0))/2;
		QImage imgscaled;
		QImage imggl;

		QGLContext* ctx = makeCurrentGLContext();
		GLint maxtexturesize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxtexturesize);

		if(maxtexturesize  < singlemaxtexturesize)
			maxtexturesize = singlemaxtexturesize;

		int bestW=roundUpToTheNextHighestPowerOf2(img.width() );
		int bestH=roundUpToTheNextHighestPowerOf2(img.height());
		while(bestW>maxtexturesize) bestW /=2;
		while(bestH>maxtexturesize) bestH /=2;
		//Logf(GLLogStream::SYSTEM,"	Texture[ %3i ] =  '%s' ( %6i x %6i ) -> ( %6i x %6i )",	i,mp->cm.textures[i].c_str(), img.width(), img.height(),bestW,bestH);
		imgscaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
		imggl=convertToGLFormat(imgscaled);

		vcg::QtThreadSafeTextureNamesContainer& txtcont = meshfeed->textureIDContainer();


		GLuint tmpid;
		glGenTextures( 1, (GLuint*)&(tmpid) );
		glBindTexture( GL_TEXTURE_2D, tmpid );
		//qDebug("      	will be loaded as GL texture id %i  ( %i x %i )",txtcont[txtcont.size() - 1] ,imgGL.width(), imgGL.height());
		glTexImage2D( GL_TEXTURE_2D, 0, 3, imggl.width(), imggl.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imggl.bits() );
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imggl.width(), imggl.height(), GL_RGBA, GL_UNSIGNED_BYTE, imggl.bits() );
		glGenerateMipmap(GL_TEXTURE_2D);
		txtcont.push_back(tmpid);
		res = tmpid;

		doneCurrentGLContext(ctx);
	}
	return res;
}


void MLSceneGLSharedDataContext::meshInserted( int mmid )
{
	MeshModel* mesh = _md.getMesh(mmid);
	if (mesh != NULL)
	{
		_meshboman[mmid] = new PerMeshMultiViewManager(mesh->cm,_gpumeminfo,_perbatchtriangles);
		_meshboman[mmid]->setDebugMode(true);
	}
}

void MLSceneGLSharedDataContext::meshRemoved(int mmid)
{
	MeshIDManMap::iterator it = _meshboman.find(mmid);
	if (it == _meshboman.end())
		return;

	PerMeshMultiViewManager* man = it->second;
	if (man != NULL)
	{
		QGLContext* ctx = makeCurrentGLContext();
		man->removeAllViewsAndDeallocateBO();
		doneCurrentGLContext(ctx);
		delete man;
	}
	_meshboman.erase(it);
}

void MLSceneGLSharedDataContext::setMeshTransformationMatrix( int mmid,const Matrix44m& m )
{
	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->setTrMatrix(m);
}

void MLSceneGLSharedDataContext::setSceneTransformationMatrix( const Matrix44m& /*m*/ )
{

}

void MLSceneGLSharedDataContext::setGLOptions( int mmid,QGLContext* viewid,const MLPerViewGLOptions& opts )
{
	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->setGLOptions(viewid,opts);
}

void MLSceneGLSharedDataContext::draw( int mmid,QGLContext* viewid ) const
{
	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->draw(viewid);
}

void MLSceneGLSharedDataContext::drawAllocatedAttributesSubset(int mmid, QGLContext * viewid, const MLRenderingData & dt)
{
	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->drawAllocatedAttributesSubset(viewid,dt);
}

void MLSceneGLSharedDataContext::removeView( QGLContext* viewerid )
{
	QGLContext* ctx = makeCurrentGLContext();
	for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
	{
		PerMeshMultiViewManager* man = it->second;
		if (man != NULL)
		{
			man->removeView(viewerid);
			man->manageBuffers();
		}
	}
	doneCurrentGLContext(ctx);
}

void MLSceneGLSharedDataContext::addView( QGLContext* viewerid,MLRenderingData& dt)
{
	for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
	{
		MeshModel* mesh = _md.getMesh(it->first);
		if (mesh != NULL)
		{
			MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(mesh,dt, _minfacessmoothrendering);
			setRenderingDataPerMeshView(it->first,viewerid,dt);
			manageBuffers(it->first);
		}
	}
}

void MLSceneGLSharedDataContext::addView(QGLContext* viewerid)
{
	for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
	{
		MeshModel* mesh = _md.getMesh(it->first);
		if (mesh != NULL)
		{
			MLRenderingData dt;
			setRenderingDataPerMeshView(it->first,viewerid,dt);
			//manageBuffers(it.key());
		}
	}
}

void MLSceneGLSharedDataContext::deAllocateGPUSharedData()
{
	QGLContext* ctx = makeCurrentGLContext();
	for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
	{
		PerMeshMultiViewManager* man = it->second;
		deAllocateTexturesPerMesh(it->first);
		man->removeAllViewsAndDeallocateBO();
	}
	doneCurrentGLContext(ctx);
}

void MLSceneGLSharedDataContext::meshAttributesUpdated(int mmid,bool conntectivitychanged,const MLRenderingData::RendAtts& atts)
{
	MeshModel* mm = _md.getMesh(mmid);
	if (mm == NULL)
		return;
	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->meshAttributesUpdated(conntectivitychanged,atts);
}

void MLSceneGLSharedDataContext::meshDeallocated( int /*mmid*/ )
{

}

void MLSceneGLSharedDataContext::getRenderInfoPerMeshView( int mmid,QGLContext* ctx,MLRenderingData& dt )
{
	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->getPerViewInfo(ctx,dt);
}

void MLSceneGLSharedDataContext::getRenderInfoPerMeshView( QGLContext* ctx,PerMeshRenderingDataMap& map )
{
	map.clear();
	for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
	{
		MLRenderingData dt;
		int meshid = it->first;
		PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(meshid);
		if (man != NULL)
			man->getPerViewInfo(ctx,dt);
		map.insert(meshid,dt);
	}
}

bool MLSceneGLSharedDataContext::manageBuffers( int mmid )
{
	bool didsomething = false;
	MeshModel* mm = _md.getMesh(mmid);

	if (mm == NULL)
		return didsomething;

	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);

	if (man != NULL)
	{
		QGLContext* ctx = makeCurrentGLContext();
		man->manageBuffers();
		doneCurrentGLContext(ctx);
	}
	return didsomething;
}

void MLSceneGLSharedDataContext::setDebugMode(int mmid,bool activatedebugmodality )
{
	MeshModel* mm = _md.getMesh(mmid);
	if (mm == NULL)
		return;

	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->setDebugMode(activatedebugmodality);
}

void MLSceneGLSharedDataContext::getLog( int mmid,MLRenderingData::DebugInfo& info)
{
	MeshModel* mm = _md.getMesh(mmid);
	if (mm == NULL)
		return;

	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		man->getLog(info);
	return;
}

bool MLSceneGLSharedDataContext::isBORenderingAvailable( int mmid )
{
	MeshModel* mm = _md.getMesh(mmid);
	if (mm == NULL)
		return false;

	PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
	if (man != NULL)
		return man->isBORenderingAvailable();
	return false;
}

#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX   0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
#define VBO_FREE_MEMORY_ATI 0x87FB
#define TEXTURE_FREE_MEMORY_ATI  0x87FC
#define RENDERBUFFER_FREE_MEMORY_ATI 0x87FD

void MLSceneGLSharedDataContext::updateGPUMemInfo()
{
	initializeGL();
	QGLContext* ctx = makeCurrentGLContext();
	GLint allmem = 0;
	glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &allmem);
	GLint currentallocated = 0;
	glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentallocated);
	/*GLenum errorNV =*/ glGetError(); // purge errors

	GLint ATI_vbo[4] = { 0, 0, 0, 0 };
	glGetIntegerv(VBO_FREE_MEMORY_ATI, ATI_vbo);
	GLint ATI_tex[4] = { 0, 0, 0, 0 };
	glGetIntegerv(TEXTURE_FREE_MEMORY_ATI, ATI_tex);
	/*GLenum errorATI =*/ glGetError(); // purge errors

	doneCurrentGLContext(ctx);
	emit currentAllocatedGPUMem((int)allmem, (int)currentallocated, (int)ATI_tex[0], (int)ATI_vbo[0]);
}

//void MLSceneGLSharedDataContext::updateRequested( int meshid,MLRenderingData::ATT_NAMES name )
//{
//    MLRenderingData::RendAtts att;
//    att[name] = true;
//    meshAttributesUpdated(meshid,false,att);
//    manageBuffers(meshid);
//
//}

void MLSceneGLSharedDataContext::initPerMeshViewRequested( int meshid,QGLContext* cont,const MLRenderingData& dt)
{
	addView(cont);
	setRenderingDataPerMeshView(meshid,cont,dt);
	manageBuffers(meshid);
}

void MLSceneGLSharedDataContext::setPerMeshViewRenderingDataRequested( int meshid,QGLContext* cont,const MLRenderingData& dt )
{
	setRenderingDataPerMeshView(meshid,cont,dt);
	manageBuffers(meshid);
}

void MLSceneGLSharedDataContext::removePerMeshViewRequested(QGLContext* cont )
{
	removeView(cont);
}

void MLSceneGLSharedDataContext::meshAttributesUpdatedRequested( int meshid,bool connectivitychanged,const MLRenderingData::RendAtts& dt )
{
	meshAttributesUpdated(meshid,connectivitychanged,dt);
	manageBuffers(meshid);
}

void MLSceneGLSharedDataContext::requestInitPerMeshView(QThread* callingthread,int meshid,QGLContext* cont,const MLRenderingData& dt )
{
	QThread* tt = thread();
	if (callingthread != tt)
		emit initPerMeshViewRequestMT(meshid,cont,dt);
	else
		//emit initPerMeshViewRequestST(meshid,cont,dt);
		initPerMeshViewRequested(meshid,cont,dt);
}

void MLSceneGLSharedDataContext::requestRemovePerMeshView(QThread* callingthread,QGLContext* cont )
{
	QThread* tt = thread();
	if (callingthread != tt)
		emit removePerMeshViewRequestMT(cont);
	else
		removeView(cont);
}

void MLSceneGLSharedDataContext::requestSetPerMeshViewRenderingData(QThread* callingthread,int meshid,QGLContext* cont,const MLRenderingData& dt )
{
	QThread* tt = thread();
	if (callingthread != tt)
		emit setPerMeshViewRenderingDataRequestMT(meshid,cont,dt);
	else
		//emit setPerMeshViewRenderingDataRequestST(meshid,cont,dt);
		setPerMeshViewRenderingDataRequested(meshid,cont,dt);
}

void MLSceneGLSharedDataContext::requestMeshAttributesUpdated(QThread* callingthread,int meshid,bool connectivitychanged,const MLRenderingData::RendAtts& dt )
{
	QThread* tt = thread();
	if (callingthread != tt)
		emit meshAttributesUpdatedRequestMT(meshid,connectivitychanged,dt);
	else
		//emit setPerMeshViewRenderingDataRequestST(meshid,cont,dt);
		meshAttributesUpdated(meshid,connectivitychanged,dt);
}




QGLContext* MLSceneGLSharedDataContext::makeCurrentGLContext()
{
	QGLContext* ctx = const_cast<QGLContext*>(QGLContext::currentContext());
	makeCurrent();
	return ctx;
}

void MLSceneGLSharedDataContext::doneCurrentGLContext( QGLContext* oldone /*= NULL*/ )
{
	doneCurrent();
	if (oldone != NULL)
		oldone->makeCurrent();
}

