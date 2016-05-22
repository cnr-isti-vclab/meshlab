#include "ml_shared_data_context.h"
#include "mlexception.h"
#include <vector>

#include "meshmodel.h"

MLSceneGLSharedDataContext::MLSceneGLSharedDataContext(MeshDocument& md,vcg::QtThreadSafeMemoryInfo& gpumeminfo,bool highprecision,size_t perbatchtriangles) 
    :QGLWidget(),_md(md),_gpumeminfo(gpumeminfo),_perbatchtriangles(perbatchtriangles),_highprecision(highprecision)
{
    if (md.size() != 0)
        throw MeshLabException(QString("MLSceneGLSharedDataContext: MeshDocument is not empty when MLSceneGLSharedDataContext is constructed."));
}

MLSceneGLSharedDataContext::~MLSceneGLSharedDataContext()
{
}

MLSceneGLSharedDataContext::PerMeshMultiViewManager* MLSceneGLSharedDataContext::meshAttributesMultiViewerManager( int mmid ) const
{
    MeshIDManMap::const_iterator it = _meshboman.find(mmid);
    if (it == _meshboman.end())
        return NULL;
    return it.value();
}

void MLSceneGLSharedDataContext::initializeGL()
{
    glewExperimental=GL_TRUE;
    makeCurrent();
    GLenum err = glewInit();
    doneCurrent();
    if (err != GLEW_OK ) {
        throw MeshLabException("MLSceneGLSharedDataContext: GLEW initialization failed\n");
    }
    
}

void MLSceneGLSharedDataContext::setRequestedAttributesPerMeshView( int mmid,QGLContext* viewerid,const MLRenderingData& perviewdata )
{
    MeshModel* mm = _md.getMesh(mmid);
    if (mm == NULL)
        return;
    MLSceneGLSharedDataContext::PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
    {
        MLRenderingData outdt;
        MLBridgeStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh(mm,perviewdata,outdt);

        man->setPerViewInfo(viewerid,outdt._mask,outdt._atts);
        man->setGLOptions(viewerid,outdt._opts);
        //makeCurrent();
        //man->manageBuffers();
        //doneCurrent();
    }
}

void MLSceneGLSharedDataContext::setRequestedAttributesPerMeshViews( int mmid,const QList<QGLContext*>& viewerid,const MLRenderingData& perviewdata )
{
    MeshModel* mm = _md.getMesh(mmid);
    if (mm == NULL)
        return;

    PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
    {
        MLRenderingData outdt;
        MLBridgeStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh(mm,perviewdata,outdt);
        foreach(QGLContext* vid,viewerid)
        {
            man->setPerViewInfo(vid,outdt._mask,outdt._atts);
            man->setGLOptions(vid,outdt._opts);
        }
        makeCurrent();
        man->manageBuffers();
        doneCurrent();
    }
}


void MLSceneGLSharedDataContext::deAllocateTexturesPerMesh( int mmid )
{
    PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
    {
        makeCurrent();
        for(size_t ii = 0;ii < man->textureIDContainer().size();++ii)
        {
            GLuint textid = man->textureIDContainer().remove(man->textureIDContainer()[ii]);
            glDeleteTextures(1,&textid);
        }
        doneCurrent();
    }
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

        makeCurrent();
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

        GLuint tmpid = txtcont[txtcont.size() - 1];
        glGenTextures( 1, (GLuint*)&(tmpid) );
        glBindTexture( GL_TEXTURE_2D, tmpid );
        //qDebug("      	will be loaded as GL texture id %i  ( %i x %i )",txtcont[txtcont.size() - 1] ,imgGL.width(), imgGL.height());
        glTexImage2D( GL_TEXTURE_2D, 0, 3, imggl.width(), imggl.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imggl.bits() );
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imggl.width(), imggl.height(), GL_RGBA, GL_UNSIGNED_BYTE, imggl.bits() );
        res = tmpid;
        doneCurrent();
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

    PerMeshMultiViewManager* man = it.value();
    if (man != NULL)
    {
        makeCurrent();
        man->removeAllViewsAndDeallocateBO();
        doneCurrent();
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

void MLSceneGLSharedDataContext::setSceneTransformationMatrix( const Matrix44m& m )
{

}

void MLSceneGLSharedDataContext::setGLOptions( int mmid,QGLContext* viewid,const MLPerViewGLOptions& opts )
{
    PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
        man->setGLOptions(viewid,opts);
}

void MLSceneGLSharedDataContext::draw( int mmid,QGLContext* viewid )
{
    PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
        man->draw(viewid);
}

void MLSceneGLSharedDataContext::removeView( QGLContext* viewerid )
{
    makeCurrent();
    for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
    {
        PerMeshMultiViewManager* man = it.value();
        if (man != NULL)
        {
            man->removeView(viewerid);
            man->manageBuffers();
        }
    }
    doneCurrent();
}

void MLSceneGLSharedDataContext::addView( QGLContext* viewerid,const MLRenderingData& dt)
{
    for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
    {
        MeshModel* mesh = _md.getMesh(it.key());
        if (mesh != NULL)
        {
            setRequestedAttributesPerMeshView(it.key(),viewerid,dt);
            manageBuffers(it.key());
        }
    }
}

void MLSceneGLSharedDataContext::deAllocateGPUSharedData()
{
    makeCurrent();
    for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
    {
        PerMeshMultiViewManager* man = it.value();
        deAllocateTexturesPerMesh(it.key());
        it.value()->removeAllViewsAndDeallocateBO();
    }
    doneCurrent();
}

void MLSceneGLSharedDataContext::meshAttributesUpdated(int mmid,bool conntectivitychanged,const vcg::GLMeshAttributesInfo::RendAtts& atts)
{
    MeshModel* mm = _md.getMesh(mmid);
    if (mm == NULL)
        return;
    PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
        man->meshAttributesUpdated(conntectivitychanged,atts);
}

void MLSceneGLSharedDataContext::meshDeallocated( int mmid )
{

}

void MLSceneGLSharedDataContext::getRenderInfoPerMeshView( int mmid,QGLContext* ctx,MLRenderingData& dt )
{
    PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
        man->getPerViewInfo(ctx,&dt._mask,&dt._atts,&dt._opts);
}

void MLSceneGLSharedDataContext::getRenderInfoPerMeshView( QGLContext* ctx,PerMeshRenderingDataMap& map )
{
    map.clear();
    for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
    {
        MLRenderingData dt;
        int meshid = it.key();
        PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(meshid);
        if (man != NULL)
            man->getPerViewInfo(ctx,&dt._mask,&dt._atts,&dt._opts);
        map.insert(meshid,dt);
    }
}

void MLSceneGLSharedDataContext::manageBuffers( int mmid )
{
    MeshModel* mm = _md.getMesh(mmid);
    if (mm == NULL)
        return;

    PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
    {
        makeCurrent();
        man->manageBuffers();
        doneCurrent();
    }
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

void MLSceneGLSharedDataContext::getLog( int mmid,vcg::GLMeshAttributesInfo::DebugInfo& info)
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

void MLBridgeStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh( MeshModel* meshmodel,const MLRenderingData& inputdt,MLRenderingData& outputdt)
                                                                                      
{
    //we use tmp value in order to avoid problems (resetting of the output values at the beginning of the function) if the (&inputpm == &outputpm) || (&inputatts == &outputatts)
    vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK tmpoutputpm = outputdt._mask;
    vcg::GLMeshAttributesInfo::RendAtts tmpoutputatts = outputdt._atts;

    tmpoutputpm = 0;
    tmpoutputatts.reset();
    if (meshmodel == NULL)
        return;
    CMeshO& mesh = meshmodel->cm;
    if (mesh.VN() == 0)
    {
        outputdt._mask = tmpoutputpm;
        outputdt._atts = tmpoutputatts;
        return;
    }

    tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = inputdt._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] && meshmodel->hasDataMask(MeshModel::MM_VERTCOORD);

    bool validfaces = (mesh.FN() > 0);
    if (!validfaces)
    {
        outputdt._mask = (unsigned int) ((inputdt._mask & vcg::GLMeshAttributesInfo::PR_POINTS) | (inputdt._mask & vcg::GLMeshAttributesInfo::PR_BBOX));
        outputdt._atts = tmpoutputatts;
        return;
    }
    
    tmpoutputpm = inputdt._mask;
    if ((inputdt._mask & vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES) && (!meshmodel->hasDataMask(MeshModel::MM_POLYGONAL)))
        tmpoutputpm = tmpoutputpm & (!vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES);

    tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = inputdt._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] && meshmodel->hasDataMask(MeshModel::MM_VERTNORMAL);
    tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = inputdt._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] && meshmodel->hasDataMask(MeshModel::MM_FACENORMAL);
    tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = inputdt._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] && meshmodel->hasDataMask(MeshModel::MM_VERTCOLOR);
    tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = inputdt._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] && meshmodel->hasDataMask(MeshModel::MM_FACECOLOR);
    tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR] = inputdt._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR] && meshmodel->hasDataMask(MeshModel::MM_COLOR);

    //horrible trick caused by MeshLab GUI. In MeshLab exists just a button turning on/off the texture visualization.
    //Unfortunately the RenderMode::textureMode member field is not just a boolean value but and enum one.
    //The enum-value depends from the enabled attributes of input mesh.
    bool wedgetexture = meshmodel->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
    tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = inputdt._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] && (meshmodel->hasDataMask(MeshModel::MM_VERTTEXCOORD) && (!wedgetexture));
    tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = inputdt._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] && wedgetexture;
    outputdt._atts = tmpoutputatts;
    outputdt._mask = tmpoutputpm;
    outputdt._opts = inputdt._opts;
}

void MLBridgeStandAloneFunctions::fromMeshModelMaskToMLRenderingAtts( int meshmodelmask,vcg::GLMeshAttributesInfo::RendAtts& atts )
{
    atts.reset();
    //connectivitychanged = bool(meshmodelmask | MeshModel::MM_FACEFACETOPO) || bool(meshmodelmask | MeshModel::MM_VERTFACETOPO) || bool(meshmodelmask | MeshModel::MM_VERTNUMBER) || bool(meshmodelmask | MeshModel::MM_FACENUMBER);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = bool(meshmodelmask & MeshModel::MM_VERTCOORD);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = bool(meshmodelmask & MeshModel::MM_VERTNORMAL);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = bool(meshmodelmask & MeshModel::MM_FACENORMAL);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = bool(meshmodelmask & MeshModel::MM_VERTCOLOR);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = bool(meshmodelmask & MeshModel::MM_FACECOLOR);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR] = bool(meshmodelmask & MeshModel::MM_COLOR);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = bool(meshmodelmask & MeshModel::MM_VERTTEXCOORD);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = bool(meshmodelmask & MeshModel::MM_WEDGTEXCOORD);
}

void MLBridgeStandAloneFunctions::updatedRenderingAttsAddedToRenderingAttsAccordingToPriorities(const vcg::GLMeshAttributesInfo::RendAtts& updated,const vcg::GLMeshAttributesInfo::RendAtts& current,vcg::GLMeshAttributesInfo::RendAtts& result)
{
    result = vcg::GLMeshAttributesInfo::RendAtts::unionSet(updated,current);
    result[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = result[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] && !(updated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL]);
    result[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = result[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] && !(updated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR]);
    result[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = result[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] && !(updated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE]);
}

//void MLBridgeStandAloneFunctions::bestPrimitiveModalityMaskAfterUpdate( MeshModel* meshmodel,int meshmodelmask,const vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK& inputpm,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK& outputpm )
//{
//    vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK tmpmask = 0;
//    if (meshmodel->cm.VN() == 0)
//    {
//        outputpm = vcg::GLMeshAttributesInfo::PR_NONE;
//        return;
//    }
//
//    tmpmask = (inputpm
//}
