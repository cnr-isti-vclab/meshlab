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

vcg::GLFeederInfo::ReqAtts MLThreadSafeGLMeshAttributesFeeder::setupRequestedAttributes(const vcg::GLFeederInfo::ReqAtts& rq,bool& allocated )
{
	QWriteLocker locker(&_lock);
	return GLMeshAttributesFeeder<CMeshO>::setupRequestedAttributes(rq,allocated);
}


void MLThreadSafeGLMeshAttributesFeeder::drawWire(vcg::GLFeederInfo::ReqAtts& rq)
{
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawTriangles(rq);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopAttrib();
}

void MLThreadSafeGLMeshAttributesFeeder::drawFlatWire(vcg::GLFeederInfo::ReqAtts& rq)
{
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1);
	drawTriangles(rq);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

	ReqAtts tmp = rq;
	tmp[ATT_VERTCOLOR] = false;
	tmp[ATT_FACECOLOR] = false;
	tmp[ATT_MESHCOLOR] = false;

	glColor3f(.3f,.3f,.3f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	QReadLocker locker(&_lock);
	GLMeshAttributesFeeder<CMeshO>::drawTriangles(tmp,_textids.textId());
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPopAttrib();
}

void MLThreadSafeGLMeshAttributesFeeder::drawPoints(vcg::GLFeederInfo::ReqAtts& rq)
{
	QReadLocker locker(&_lock);
	GLMeshAttributesFeeder<CMeshO>::drawPoints(rq);
}

void MLThreadSafeGLMeshAttributesFeeder::drawTriangles(vcg::GLFeederInfo::ReqAtts& rq)
{
	QReadLocker locker(&_lock);
    GLMeshAttributesFeeder<CMeshO>::drawTriangles(rq,_textids.textId());
}

void MLThreadSafeGLMeshAttributesFeeder::drawBBox(vcg::GLFeederInfo::ReqAtts& rq)
{
	QReadLocker locker(&_lock);
	
	Box3m& b = _mesh.bbox;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	GLuint bbhandle;
	glGenBuffers(1,&bbhandle);
	std::vector<vcg::Point3f> bbox(12 * 2);

	//0
	bbox[0] = vcg::Point3f((float)b.min[0],(float)b.min[1],(float)b.min[2]);
	bbox[1] = vcg::Point3f((float)b.max[0],(float)b.min[1],(float)b.min[2]);

	//1
	bbox[2] = vcg::Point3f((float)b.max[0],(float)b.min[1],(float)b.min[2]);
	bbox[3] = vcg::Point3f((float)b.max[0],(float)b.max[1],(float)b.min[2]);

	//2
	bbox[4] = vcg::Point3f((float)b.max[0],(float)b.max[1],(float)b.min[2]);
	bbox[5] = vcg::Point3f((float)b.min[0],(float)b.max[1],(float)b.min[2]);

	//3
	bbox[6] = vcg::Point3f((float)b.min[0],(float)b.max[1],(float)b.min[2]);
	bbox[7] = vcg::Point3f((float)b.min[0],(float)b.min[1],(float)b.min[2]);

	//4
	bbox[8] = vcg::Point3f((float)b.min[0],(float)b.min[1],(float)b.min[2]);
	bbox[9] = vcg::Point3f((float)b.min[0],(float)b.min[1],(float)b.max[2]);

	//5
	bbox[10] = vcg::Point3f((float)b.min[0],(float)b.min[1],(float)b.max[2]);
	bbox[11] = vcg::Point3f((float)b.max[0],(float)b.min[1],(float)b.max[2]);

	//6
	bbox[12] = vcg::Point3f((float)b.max[0],(float)b.min[1],(float)b.max[2]);
	bbox[13] = vcg::Point3f((float)b.max[0],(float)b.min[1],(float)b.min[2]);

	//7
	bbox[14] = vcg::Point3f((float)b.max[0],(float)b.min[1],(float)b.max[2]);
	bbox[15] = vcg::Point3f((float)b.max[0],(float)b.max[1],(float)b.max[2]);

	//8
	bbox[16] = vcg::Point3f((float)b.max[0],(float)b.max[1],(float)b.max[2]);
	bbox[17] = vcg::Point3f((float)b.max[0],(float)b.max[1],(float)b.min[2]);
	
	//9
	bbox[18] = vcg::Point3f((float)b.max[0],(float)b.max[1],(float)b.max[2]);
	bbox[19] = vcg::Point3f((float)b.min[0],(float)b.max[1],(float)b.max[2]);

	//10
	bbox[20] = vcg::Point3f((float)b.min[0],(float)b.max[1],(float)b.max[2]);
	bbox[21] = vcg::Point3f((float)b.min[0],(float)b.min[1],(float)b.max[2]);

	//11
	bbox[22] = vcg::Point3f((float)b.min[0],(float)b.max[1],(float)b.max[2]);
	bbox[23] = vcg::Point3f((float)b.min[0],(float)b.max[1],(float)b.min[2]);

	glColor3f(1.0f,1.0f,1.0f);
	glBindBuffer(GL_ARRAY_BUFFER,bbhandle);
	glBufferData(GL_ARRAY_BUFFER, 12 * 2 * sizeof(vcg::Point3f), &(bbox[0]), GL_STATIC_DRAW);
	glVertexPointer(3,GL_FLOAT,0,0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINES,0,24);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDeleteBuffers(1,&bbhandle);
	glPopAttrib();
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
	connect(&_md,SIGNAL(meshAdded(int)),this,SLOT(meshInserted(int)));
	connect(&_md,SIGNAL(meshRemoved(int)),this,SLOT(meshRemoved(int)));
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
		makeCurrent();
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
	doneCurrent();
}

MLThreadSafeGLMeshAttributesFeeder* MLSceneGLSharedDataContext::meshAttributesFeeder( int meshid ) const
{
	QMap<int,MLThreadSafeGLMeshAttributesFeeder*>::const_iterator it = _scene.find(meshid);
	if (it == _scene.end())
		return NULL;
	return it.value();
}

vcg::GLFeederInfo::ReqAtts MLSceneGLSharedDataContext::setupRequestedAttributesPerMesh( int meshid,const vcg::GLFeederInfo::ReqAtts& req,bool& allocated)
{
	allocated = false;
	MLThreadSafeGLMeshAttributesFeeder* meshfeed = meshAttributesFeeder(meshid);		
	vcg::GLFeederInfo::ReqAtts rr = req;
	if (meshfeed != NULL)
	{
		makeCurrent();
		rr = meshfeed->setupRequestedAttributes(req,allocated);
		doneCurrent();
	}
	return rr;
}
//bool MLSceneGLSharedDataContext::setupRequestedAttributes( unsigned int meshid,unsigned int viewid,vcg::GLFeederInfo::ReqAtts& rq )
//{
//	QWriteLocker locker(&_lock);
//	makeCurrent();
//	bool res = false;
//	MLThreadSafeGLMeshAttributesFeeder* meshfeed = meshAttributesFeeder(meshid);
//	if (meshfeed != NULL)
//		res = meshfeed->setupRequestedAttributes((unsigned int)viewid,rq);
//	doneCurrent();
//	return res;
//}

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
	case (vcg::GLW::DMWire):
		{
			rq[vcg::GLFeederInfo::ATT_VERTPOSITION] = true;
			rq[vcg::GLFeederInfo::ATT_VERTNORMAL] = true;
			rq[vcg::GLFeederInfo::ATT_FACENORMAL] = false;
			rq[vcg::GLFeederInfo::ATT_VERTINDEX] = true;
			break;
		}	
	case (vcg::GLW::DMFlat):
	case (vcg::GLW::DMFlatWire):
		{
			rq[vcg::GLFeederInfo::ATT_VERTPOSITION] = true;
			rq[vcg::GLFeederInfo::ATT_VERTNORMAL] = false;
			rq[vcg::GLFeederInfo::ATT_FACENORMAL] = true;
			rq[vcg::GLFeederInfo::ATT_VERTINDEX] = false;
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

void MLSceneRenderModeAdapter::renderMesh(unsigned int meshid, GLArea &area)
{
    MLSceneGLSharedDataContext* cont = area.getSceneGLSharedContext();
    if (cont == NULL)
        return;

    MLThreadSafeGLMeshAttributesFeeder* feed = cont->meshAttributesFeeder(meshid);
    if (feed == NULL)
        return;

	QMap<int,RenderMode>::iterator itrm = area.rendermodemap.find(meshid);
    QMap<int,vcg::GLFeederInfo::ReqAtts>::iterator itrq = area.reqattsmap.find(meshid);
    if ((itrq == area.reqattsmap.end()) || (itrm == area.rendermodemap.end()))
        return;

	area.makeCurrent();

	glPushMatrix();
	glMultMatrix(feed->mesh().Tr);
    switch (itrm.value().drawMode)
    {
        case(vcg::GLFeederInfo::PR_POINTS):
        {
            feed->drawPoints(itrq.value());
            break;
        }

        case(vcg::GLW::DMWire):
        {
            feed->drawWire(itrq.value());
            break;
        }

        case(vcg::GLW::DMFlatWire):
        {
            feed->drawFlatWire(itrq.value());
            break;
        }
        case(vcg::GLW::DMFlat):
        case(vcg::GLW::DMSmooth):
        {
            feed->drawTriangles(itrq.value());
            break;
        }
		case(vcg::GLW::DMBox):
		{
			feed->drawBBox(itrq.value());
			break;
		}
        default:
            break;
    }
	glTranslate(cont->globalSceneCenter());
	glPopMatrix();
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

//bool MLSceneRenderModeAdapter::setupRequestedAttributesAccordingToRenderMode( unsigned int meshid,GLArea& area )
//{
//	bool res = false;
//	MLSceneGLSharedDataContext* shared = area.getSceneGLSharedContext();
//	if (shared != NULL)
//	{
//		shared->makeCurrent();
//		MLThreadSafeGLMeshAttributesFeeder* meshfeed = shared->meshAttributesFeeder(meshid);		
//		if (meshfeed != NULL)
//		{
//			QMap<int,RenderMode>::iterator itrm = area.rendermodemap.find(meshid);
//			if (itrm != area.rendermodemap.end())
//			{
//				vcg::GLFeederInfo::ReqAtts rq;
//				MLSceneRenderModeAdapter::renderModeToReqAtts(itrm.value(),rq);
//				res = meshfeed->setupRequestedAttributes(rq);
//			}
//		}
//		shared->doneCurrent();
//	}
//	return res;
//}
