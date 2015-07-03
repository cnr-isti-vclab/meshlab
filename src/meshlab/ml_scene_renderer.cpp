#include "ml_scene_renderer.h"
#include "ml_thread_safe_memory_info.h"
#include "glarea.h"
#include <common/mlexception.h>

#include <vector>

MLThreadSafeGLMeshAttributesFeeder::MLThreadSafeGLMeshAttributesFeeder(CMeshO& mesh,MLThreadSafeMemoryInfo& gpumeminfo,size_t perbatchtriangles)
	:GLMeshAttributesFeeder<CMeshO>(mesh,gpumeminfo,perbatchtriangles),_lock(QReadWriteLock::Recursive)
{
}

void MLThreadSafeGLMeshAttributesFeeder::setPerBatchTriangles( size_t perbatchtriangles )
{
	QWriteLocker locker(&_lock);
	GLMeshAttributesFeeder<CMeshO>::setPerBatchPrimitives(perbatchtriangles);
}

size_t MLThreadSafeGLMeshAttributesFeeder::perBatchTriangles() const
{
	QReadLocker locker(&_lock);
	return GLMeshAttributesFeeder<CMeshO>::perBatchPrimitives();

}

bool MLThreadSafeGLMeshAttributesFeeder::renderedWithBO() const
{
	QReadLocker locker(&_lock);
	return GLMeshAttributesFeeder<CMeshO>::isPossibleToUseBORendering();
}

void MLThreadSafeGLMeshAttributesFeeder::meshAttributesUpdated( int mask )
{
	QWriteLocker locker(&_lock);
	GLMeshAttributesFeeder<CMeshO>::meshAttributesUpdated(mask);
}

bool MLThreadSafeGLMeshAttributesFeeder::setupRequestedAttributes(unsigned int viewid,vcg::GLFeederInfo::ReqAtts& rq )
{
	QWriteLocker locker(&_lock);
	return GLMeshAttributesFeeder<CMeshO>::setupRequestedAttributes(viewid,rq);
}


void MLThreadSafeGLMeshAttributesFeeder::drawWire(unsigned int viewid)
{
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawTriangles(viewid);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopAttrib();
}

void MLThreadSafeGLMeshAttributesFeeder::drawFlatWire(unsigned int viewid)
{
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1);
	drawTriangles(viewid);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);


	/*WARNING!!! TERRIBLE TRICK IN ORDER TO BE ABLE TO MODIFY THE ReqAtts WITHOUT CHANGING THEM IN THE PerView map*/
	std::map<unsigned int,ReqAtts>::iterator it = _allreqattsmap.find(viewid);
	if (it == _allreqattsmap.end())
		return;

	ReqAtts tmp = it->second;
	tmp[ATT_VERTCOLOR] = false;
	tmp[ATT_FACECOLOR] = false;
	tmp[ATT_MESHCOLOR] = false;

	glColor3f(.3f,.3f,.3f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	QReadLocker locker(&_lock);
	GLMeshAttributesFeeder<CMeshO>::drawTriangles(tmp,_textids.textId());
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/************************************************************************************/
	glPopAttrib();
}

void MLThreadSafeGLMeshAttributesFeeder::drawPoints(unsigned int viewid)
{
	QReadLocker locker(&_lock);
	GLMeshAttributesFeeder<CMeshO>::draw(viewid);
}

void MLThreadSafeGLMeshAttributesFeeder::drawTriangles(unsigned int viewid)
{
	QReadLocker locker(&_lock);
	GLMeshAttributesFeeder<CMeshO>::draw(viewid,_textids.textId());
}

void MLThreadSafeGLMeshAttributesFeeder::drawBBox(unsigned int viewid)
{

}

void MLThreadSafeGLMeshAttributesFeeder::deAllocateBO()
{
	QWriteLocker locker(&_lock);
	GLMeshAttributesFeeder<CMeshO>::buffersDeAllocationRequested();
}

MLThreadSafeGLMeshAttributesFeeder::MLThreadSafeTextureNamesContainer::MLThreadSafeTextureNamesContainer()
	:_tmid(),_lock(QReadWriteLock::Recursive)
{

}

MLThreadSafeGLMeshAttributesFeeder::MLThreadSafeTextureNamesContainer::~MLThreadSafeTextureNamesContainer()
{
	clear();
}

void MLThreadSafeGLMeshAttributesFeeder::MLThreadSafeTextureNamesContainer::push_back( GLuint textid )
{
	QWriteLocker locker(&_lock);
	_tmid.push_back(textid);
}

size_t MLThreadSafeGLMeshAttributesFeeder::MLThreadSafeTextureNamesContainer::size() const
{
	QReadLocker locker(&_lock);
	return _tmid.size(); 
}

bool MLThreadSafeGLMeshAttributesFeeder::MLThreadSafeTextureNamesContainer::empty() const
{
	QReadLocker locker(&_lock);
	return _tmid.empty(); 
}

void MLThreadSafeGLMeshAttributesFeeder::MLThreadSafeTextureNamesContainer::clear()
{
	QWriteLocker locker(&_lock);
	_tmid.clear();
}

MLSceneGLSharedDataContext::MLSceneGLSharedDataContext(MeshDocument& md,MLThreadSafeMemoryInfo& gpumeminfo,bool highprecision,size_t perbatchtriangles,QWidget* parent) 
	:QGLWidget(parent),_scene(),_md(md),_gpumeminfo(gpumeminfo),_perbatchtriangles(perbatchtriangles),_globalscenecenter(0.0,0.0,0.0),_highprecision(highprecision)
{
	connect(&_md,SIGNAL(meshAdded(int meshid)),this,SLOT(meshInserted(int meshid)));
	connect(&_md,SIGNAL(meshRemoved(int meshid)),this,SLOT(meshRemoved(int meshid)));
}

MLSceneGLSharedDataContext::~MLSceneGLSharedDataContext()
{

	for(QMap< int ,MLThreadSafeGLMeshAttributesFeeder* >::iterator it = _scene.begin();it != _scene.end();++it)
		delete it.value();
	_scene.clear();
}

void MLSceneGLSharedDataContext::computeSceneGlobalCenter()
{
	vcg::Box3<Scalarm> scenebbox;
	for(QMap< int,MLThreadSafeGLMeshAttributesFeeder* >::const_iterator it = _scene.begin();it != _scene.end();++it)
	{
		MeshModel* mesh = _md.getMesh(it.key());
		if (mesh != NULL)
			scenebbox.Add(mesh->cm.Tr,mesh->cm.bbox);
	}
	_globalscenecenter = scenebbox.Center();
}

void MLSceneGLSharedDataContext::meshInserted( int meshid )
{
	MeshModel* mesh = _md.getMesh(meshid);
	if (mesh != NULL)
	{
		_scene[meshid] = new MLThreadSafeGLMeshAttributesFeeder(mesh->cm,_gpumeminfo,_perbatchtriangles);
		computeSceneGlobalCenter();
	}
}

void MLSceneGLSharedDataContext::meshRemoved(int meshid)
{
	QMap<int,MLThreadSafeGLMeshAttributesFeeder*>::iterator it = _scene.find(meshid);
	if (it == _scene.end())
		return;

	if (it.value() != NULL)
	{
		makeCurrent();
		it.value()->deAllocateBO();
		delete it.value();
	}
	_scene.erase(it);
	computeSceneGlobalCenter();
}

void MLSceneGLSharedDataContext::deAllocateGPUSharedData()
{
	std::vector<int> tmpvect;
	for(QMap< int,MLThreadSafeGLMeshAttributesFeeder* >::iterator it = _scene.begin();it != _scene.end();++it)
		tmpvect.push_back(it.key());
	makeCurrent();
	for(std::vector<int>::const_iterator itv = tmpvect.begin();itv != tmpvect.end();++itv)
		meshRemoved(*itv);
	tmpvect.clear();
}

void MLSceneGLSharedDataContext::initializeGL()
{
	makeCurrent();
	glewExperimental=GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK ) {
		MeshLabException("GLArea warning: glew initialization failed\n");
	}
}

MLThreadSafeGLMeshAttributesFeeder* MLSceneGLSharedDataContext::meshAttributeFeeder( int meshid ) const
{
	QMap<int,MLThreadSafeGLMeshAttributesFeeder*>::const_iterator it = _scene.find(meshid);
	if (it == _scene.end())
		return NULL;
	return it.value();
}

void MLSceneGLSharedDataContext::renderScene(GLArea& glarea)
{
	if (glarea.mvc() == NULL)
		return;

	glarea.makeCurrent();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	if (highPrecisionRendering())
		glTranslate(-_globalscenecenter);
	for(QMap<int,MLThreadSafeGLMeshAttributesFeeder*>::const_iterator it = _scene.begin();it != _scene.end();++it)
	{
		MLThreadSafeGLMeshAttributesFeeder* feed = (*it);
		if (feed == NULL)
			return;
		

		QMap<int,RenderMode>::const_iterator rmit = glarea.rendermodemap.find(it.key());
		if (rmit != glarea.rendermodemap.end())
		{
			unsigned int areaid(glarea.getId());
			const RenderMode& rendermode = rmit.value();
			vcg::GLFeederInfo::ReqAtts req;
			MLSceneRenderModeAdapter::renderModeToReqAtts(rendermode,req);

			switch (rendermode.drawMode)
			{
			case(vcg::GLW::DMPoints):
				{
					feed->drawPoints(areaid);
					break;
				}

			case(vcg::GLW::DMWire):
				{
					feed->drawWire(areaid);
					break;
				}

			case(vcg::GLW::DMFlatWire):
				{
					feed->drawFlatWire(areaid);
					break;
				}
			case(vcg::GLW::DMFlat):
			case(vcg::GLW::DMSmooth):
				{
					feed->drawTriangles(areaid);
					break;
				}
			}
		}
	}
	glPopMatrix();
	glPopAttrib();
}

void MLSceneRenderModeAdapter::renderModeToReqAtts( const RenderMode& rm,vcg::GLFeederInfo::ReqAtts& rq )
{
	rq.reset();
	switch(rm.drawMode)
	{
	case (vcg::GLW::DMPoints):
		{
			rq[vcg::GLFeederInfo::ATT_VERTPOSITION] = true;
			rq[vcg::GLFeederInfo::ATT_VERTNORMAL] = true;
			rq[vcg::GLFeederInfo::ATT_FACENORMAL] = false;
			rq[vcg::GLFeederInfo::ATT_VERTINDEX] = false;
			break;
		}
	case (vcg::GLW::DMSmooth):
		{
			rq[vcg::GLFeederInfo::ATT_VERTPOSITION] = true;
			rq[vcg::GLFeederInfo::ATT_VERTNORMAL] = true;
			rq[vcg::GLFeederInfo::ATT_FACENORMAL] = false;
			rq[vcg::GLFeederInfo::ATT_VERTINDEX] = false;
			break;
		}	
	case (vcg::GLW::DMFlat):
	case (vcg::GLW::DMFlatWire):
		{
			rq[vcg::GLFeederInfo::ATT_VERTPOSITION] = true;
			rq[vcg::GLFeederInfo::ATT_VERTNORMAL] = false;
			rq[vcg::GLFeederInfo::ATT_FACENORMAL] = true;
			rq[vcg::GLFeederInfo::ATT_VERTINDEX] = true;
			break;
		}	
	default:
		{
			break;
		}
	}
	renderModeColorToReqAtts(rm,rq);
	renderModeTextureToReqAtts(rm,rq);
	rq.primitiveModality() = renderModeToPrimitiveModality(rm);
}

void MLSceneRenderModeAdapter::renderModeColorToReqAtts( const RenderMode& rm,vcg::GLFeederInfo::ReqAtts& rq )
{
	switch(rm.colorMode)
	{
	case (vcg::GLW::CMNone):
		{
			rq[vcg::GLFeederInfo::ATT_VERTCOLOR] = false;
			rq[vcg::GLFeederInfo::ATT_FACECOLOR] = false;
			rq[vcg::GLFeederInfo::ATT_MESHCOLOR] = false;
			break;
		}

	case (vcg::GLW::CMPerVert):
		{
			rq[vcg::GLFeederInfo::ATT_VERTCOLOR] = true;
			rq[vcg::GLFeederInfo::ATT_FACECOLOR] = false;
			rq[vcg::GLFeederInfo::ATT_MESHCOLOR] = false;
			break;
		}

	case (vcg::GLW::CMPerFace):
		{
			rq[vcg::GLFeederInfo::ATT_VERTCOLOR] = false;
			rq[vcg::GLFeederInfo::ATT_FACECOLOR] = true;
			rq[vcg::GLFeederInfo::ATT_MESHCOLOR] = false;
			break;
		}

	case (vcg::GLW::CMPerMesh):
		{
			rq[vcg::GLFeederInfo::ATT_VERTCOLOR] = false;
			rq[vcg::GLFeederInfo::ATT_FACECOLOR] = false;
			rq[vcg::GLFeederInfo::ATT_MESHCOLOR] = true;
			break;
		}

	default:
		{
			break;
		}
	}
	rq[vcg::GLFeederInfo::ATT_VERTINDEX] = rq[vcg::GLFeederInfo::ATT_VERTINDEX] || rq[vcg::GLFeederInfo::ATT_FACECOLOR];
}

void MLSceneRenderModeAdapter::renderModeTextureToReqAtts( const RenderMode& rm,vcg::GLFeederInfo::ReqAtts& rq )
{
	switch(rm.textureMode)
	{
	case (vcg::GLW::TMNone):
		{
			rq[vcg::GLFeederInfo::ATT_VERTTEXTURE] = false;
			rq[vcg::GLFeederInfo::ATT_WEDGETEXTURE] = false;
			break;
		}

	case (vcg::GLW::TMPerVert):
		{
			rq[vcg::GLFeederInfo::ATT_VERTTEXTURE] = true;
			rq[vcg::GLFeederInfo::ATT_WEDGETEXTURE] = false;
			break;
		}

	case (vcg::GLW::TMPerWedge):
	case (vcg::GLW::TMPerWedgeMulti):
		{
			rq[vcg::GLFeederInfo::ATT_VERTTEXTURE] = false;
			rq[vcg::GLFeederInfo::ATT_WEDGETEXTURE] = true;
			break;
		}

	default:
		{
			break;
		}
	}
	rq[vcg::GLFeederInfo::ATT_VERTINDEX] = rq[vcg::GLFeederInfo::ATT_VERTINDEX] || rq[vcg::GLFeederInfo::ATT_WEDGETEXTURE];
}

vcg::GLFeederInfo::PRIMITIVE_MODALITY MLSceneRenderModeAdapter::renderModeToPrimitiveModality( const RenderMode& rm )
{
	switch(rm.drawMode)
	{

	case (vcg::GLW::DMPoints):
			return vcg::GLFeederInfo::PR_POINTS;
	case (vcg::GLW::DMSmooth):
	case (vcg::GLW::DMFlat):
	case (vcg::GLW::DMFlatWire):
			return vcg::GLFeederInfo::PR_TRIANGLES;	
	default:
			return vcg::GLFeederInfo::PR_NONE;
	}
	return vcg::GLFeederInfo::PR_NONE;
}
