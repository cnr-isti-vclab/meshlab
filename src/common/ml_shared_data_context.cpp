#include "ml_shared_data_context.h"
#include "mlexception.h"
#include <vector>

#include "meshmodel.h"

MLSceneGLSharedDataContext::MLSceneGLSharedDataContext(MeshDocument& md,vcg::QtThreadSafeMemoryInfo& gpumeminfo,bool highprecision,size_t perbatchtriangles) 
    :QGLWidget(),_md(md),_gpumeminfo(gpumeminfo),_perbatchtriangles(perbatchtriangles),_highprecision(highprecision)
{
    if (md.size() != 0)
        throw MLException(QString("MLSceneGLSharedDataContext: MeshDocument is not empty when MLSceneGLSharedDataContext is constructed."));
    
    _timer = new QTimer(this);
    connect(_timer,SIGNAL(timeout()),this,SLOT(updateGPUMemInfo()));
    
    /*connection intended for the plugins living in another thread*/
    connect(this,SIGNAL(initPerMeshViewRequest(int,QGLContext*,const MLRenderingData&)),this,SLOT(initPerMeshViewRequested(int,QGLContext*,const MLRenderingData&)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(removePerMeshViewRequest(QGLContext*)),this,SLOT(removePerMeshViewRequested(QGLContext*)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(setPerMeshViewRenderingDataRequest(QGLContext*)),this,SLOT(setPerMeshViewRenderingDataRequested(QGLContext*)),Qt::BlockingQueuedConnection);
    /****************************************************************/

    _timer->start(1000);
    updateGPUMemInfo();
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
        throw MLException("MLSceneGLSharedDataContext: GLEW initialization failed\n");
    }
    
}

void MLSceneGLSharedDataContext::setRenderingDataPerMeshView( int mmid,QGLContext* viewerid,const MLRenderingData& perviewdata )
{
    MeshModel* mm = _md.getMesh(mmid);
    if (mm == NULL)
        return;
    MLSceneGLSharedDataContext::PerMeshMultiViewManager* man = meshAttributesMultiViewerManager(mmid);
    if (man != NULL)
    {
        man->setPerViewInfo(viewerid,perviewdata);
    }
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


        GLuint tmpid;
        glGenTextures( 1, (GLuint*)&(tmpid) );
        glBindTexture( GL_TEXTURE_2D, tmpid );
        //qDebug("      	will be loaded as GL texture id %i  ( %i x %i )",txtcont[txtcont.size() - 1] ,imgGL.width(), imgGL.height());
        glTexImage2D( GL_TEXTURE_2D, 0, 3, imggl.width(), imggl.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imggl.bits() );
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imggl.width(), imggl.height(), GL_RGBA, GL_UNSIGNED_BYTE, imggl.bits() );
        txtcont.push_back(tmpid);
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

void MLSceneGLSharedDataContext::draw( int mmid,QGLContext* viewid ) const
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

void MLSceneGLSharedDataContext::addView( QGLContext* viewerid,MLRenderingData& dt)
{
    for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
    {
        MeshModel* mesh = _md.getMesh(it.key());
        if (mesh != NULL)
        {
            MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(mesh,dt);
            setRenderingDataPerMeshView(it.key(),viewerid,dt);
            manageBuffers(it.key());
        }
    }
}

void MLSceneGLSharedDataContext::addView(QGLContext* viewerid)
{
    for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
    {
        MeshModel* mesh = _md.getMesh(it.key());
        if (mesh != NULL)
        {
            MLRenderingData dt;
            setRenderingDataPerMeshView(it.key(),viewerid,dt);
            //manageBuffers(it.key());
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
        man->getPerViewInfo(ctx,dt);
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
            man->getPerViewInfo(ctx,dt);
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

#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX   0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049

void MLSceneGLSharedDataContext::updateGPUMemInfo()
{   
    makeCurrent();
    GLint allmem = 0;
    glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &allmem);

    GLint currentallocated = 0;
    glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentallocated);
    doneCurrent();
    emit currentAllocatedGPUMem((int)allmem,(int)currentallocated);
}

void MLSceneGLSharedDataContext::updateRequested( int meshid,vcg::GLMeshAttributesInfo::ATT_NAMES name )
{
    vcg::GLMeshAttributesInfo::RendAtts att;
    att[name] = true;
    meshAttributesUpdated(meshid,false,att);
    manageBuffers(meshid);
    
}

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

void MLSceneGLSharedDataContext::requestInitPerMeshView( int meshid,QGLContext* cont,const MLRenderingData& dt )
{
    emit initPerMeshViewRequest(meshid,cont,dt);
}

void MLSceneGLSharedDataContext::requestRemovePerMeshView(QGLContext* cont )
{
    emit removePerMeshViewRequest(cont);
}

void MLSceneGLSharedDataContext::requestSetPerMeshViewRenderingData( int meshid,QGLContext* cont,const MLRenderingData& dt )
{
    emit setPerMeshViewRenderingDataRequest(meshid,cont,dt);
}

void MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh( MeshModel* meshmodel,const MLRenderingData& inputdt,MLRenderingData& outputdt)                                                                                    
{
    if (meshmodel == NULL)
        return;
    CMeshO& mesh = meshmodel->cm;
    if (mesh.VN() == 0)
    {
        outputdt.reset(false);
        return;
    }
    bool validfaces = (mesh.FN() > 0);

    vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK tmpoutputpm = inputdt.getPrimitiveModalityMask();
    for(size_t pmind = 0;pmind < size_t(vcg::GLMeshAttributesInfo::PR_ARITY); ++pmind)
    {
        vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pmc = vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY(pmind);

            
        vcg::GLMeshAttributesInfo::RendAtts tmpoutputatts;
        if (inputdt.get(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY(pmind),tmpoutputatts))
        {
            tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] &= meshmodel->hasDataMask(MeshModel::MM_VERTCOORD);
            tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] &= meshmodel->hasDataMask(MeshModel::MM_VERTNORMAL);
            tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] &= validfaces && meshmodel->hasDataMask(MeshModel::MM_FACENORMAL);
            tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] &= meshmodel->hasDataMask(MeshModel::MM_VERTCOLOR);
            tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] &= validfaces && meshmodel->hasDataMask(MeshModel::MM_FACECOLOR);

            //horrible trick caused by MeshLab GUI. In MeshLab exists just a button turning on/off the texture visualization.
            //Unfortunately the RenderMode::textureMode member field is not just a boolean value but and enum one.
            //The enum-value depends from the enabled attributes of input mesh.
            bool wedgetexture = meshmodel->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
            tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] &= (meshmodel->hasDataMask(MeshModel::MM_VERTTEXCOORD) && (!wedgetexture));
            tmpoutputatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] &= validfaces && wedgetexture;
            if (MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMesh(meshmodel,pmc))
                outputdt.set(pmc,tmpoutputatts);
        }
        else
            throw MLException(QString("MLPoliciesStandAloneFunctions: trying to access to a non defined PRIMITIVE_MODALITY!"));  

        
    }
    MLPoliciesStandAloneFunctions::setPerViewGLOptionsPriorities(meshmodel,outputdt);
}

void MLPoliciesStandAloneFunctions::fromMeshModelMaskToMLRenderingAtts( int meshmodelmask,vcg::GLMeshAttributesInfo::RendAtts& atts)
{
    atts.reset();
    //connectivitychanged = bool(meshmodelmask | MeshModel::MM_FACEFACETOPO) || bool(meshmodelmask | MeshModel::MM_VERTFACETOPO) || bool(meshmodelmask | MeshModel::MM_VERTNUMBER) || bool(meshmodelmask | MeshModel::MM_FACENUMBER);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = bool(meshmodelmask & MeshModel::MM_VERTCOORD);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = bool(meshmodelmask & MeshModel::MM_VERTNORMAL);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = bool(meshmodelmask & MeshModel::MM_FACENORMAL);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = bool(meshmodelmask & MeshModel::MM_VERTCOLOR);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = bool(meshmodelmask & MeshModel::MM_FACECOLOR);
    //atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FIXEDCOLOR] = bool(meshmodelmask & MeshModel::MM_COLOR);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = bool(meshmodelmask & MeshModel::MM_VERTTEXCOORD);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = bool(meshmodelmask & MeshModel::MM_WEDGTEXCOORD);
}

void MLPoliciesStandAloneFunctions::maskMeaninglessAttributesPerPrimitiveModality( vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,vcg::GLMeshAttributesInfo::RendAtts& atts )
{
    switch(pm)
    {
    case (vcg::GLMeshAttributesInfo::PR_POINTS):
        {
            atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = false;
            atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = false;
            atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = false;
            return;
        }
    case (vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES):
    case (vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES):
        {
            atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = false;
            atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = false;
            atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = false;
            atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = false;  
            return;
        }
    case (vcg::GLMeshAttributesInfo::PR_SOLID):
        {
            return;
        }
    }
}

void MLPoliciesStandAloneFunctions::updatedRendAttsAccordingToPriorities(const vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,const vcg::GLMeshAttributesInfo::RendAtts& updated,const vcg::GLMeshAttributesInfo::RendAtts& current,vcg::GLMeshAttributesInfo::RendAtts& result)
{
    vcg::GLMeshAttributesInfo::RendAtts filteredupdated = updated;
    vcg::GLMeshAttributesInfo::RendAtts tmp = current;
    tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] |= filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION];
    tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] |= filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL];
    tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = (tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] || filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL]) && !(filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL]);
    tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] |= filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR];
    tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = (tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] || filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR]) && !(filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR]);
    //tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FIXEDCOLOR] = (tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FIXEDCOLOR] || filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FIXEDCOLOR]) && !(filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR]);
    tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] |= filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE];
    tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = (tmp[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] || filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE]) && !(filteredupdated[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE]);
    result = tmp;
}

void MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(MeshModel* meshmodel,MLRenderingData& dtout)
{
    MLRenderingData dt; 
    if (meshmodel == NULL)
        return;
    if (meshmodel->cm.VN() == 0)
    {
        dt.reset();
    }
    else
    {
        vcg::GLMeshAttributesInfo::RendAtts tmpatts;
        tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
        tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = true;
        tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = true;
        tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = true;

        dt.set(vcg::GLMeshAttributesInfo::PR_POINTS,tmpatts);
       
        if (meshmodel->cm.FN() > 0)
        {
            dt.set(vcg::GLMeshAttributesInfo::PR_POINTS,false);
            tmpatts.reset();
            tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
            tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = true;
            tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = true;
            tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = true;
            dt.set(vcg::GLMeshAttributesInfo::PR_SOLID,tmpatts);
        }
        else
        {
            if ((meshmodel->cm.FN() == 0) && ((meshmodel->cm.EN() > 0)))
            {
                dt.reset();
                tmpatts.reset();
                tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
                tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = true;
                tmpatts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = true;
                dt.set(vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES,tmpatts);
                return;
            }
        }
    }
    MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh(meshmodel,dt,dtout);
}


bool MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMesh(MeshModel* m,const vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm)
{
    bool validvert = (m->cm.VN() > 0);
    bool validfaces = (m->cm.FN() > 0);
    bool validedges = (m->cm.EN() > 0);
    return MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMeshInfo(validvert,validfaces,validedges,m->dataMask(),pm);
}

bool MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMeshInfo(bool validvert,bool validfaces,bool validedges,int meshmask,const vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm)
{
    switch(pm)
    {
    case(vcg::GLMeshAttributesInfo::PR_POINTS):
        {
            return validvert;
        }
    case(vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES):
        {
            return (validvert) && ((meshmask & MeshModel::MM_POLYGONAL) || ((!validfaces) && (validedges)));
        }
    case(vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES):
    case(vcg::GLMeshAttributesInfo::PR_SOLID):
        {
            return validvert && validfaces;
        }
    }
    return false;
}

bool MLPoliciesStandAloneFunctions::isPrimitiveModalityWorthToBeActivated(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,bool wasvisualized,bool wasmeanigful,bool ismeaningful)
{
    (void) pm;
    if ((!wasmeanigful) && (ismeaningful))
        return true;
    if (wasmeanigful && ismeaningful && wasvisualized)
        return true;
    return false;
}

void MLPoliciesStandAloneFunctions::setAttributePriorities(vcg::GLMeshAttributesInfo::RendAtts& atts )
{
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] &= !(atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL]);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] &= !(atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR]);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] &= !(atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE]);
}

void MLPoliciesStandAloneFunctions::setPerViewGLOptionsPriorities(MeshModel* mm,MLRenderingData& dt )
{
    if (mm == NULL)
        return;
    bool permeshcolor = mm->hasDataMask(MeshModel::MM_COLOR);
    MLPerViewGLOptions glopts;
    if (!dt.get(glopts))
        return;
    if (permeshcolor)
    {
        vcg::Color4b def = mm->cm.C();
        glopts._permesh_color = def;
    }
    for(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm = vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY(0);pm < vcg::GLMeshAttributesInfo::PR_ARITY;pm = vcg::GLMeshAttributesInfo::next(pm))
    {
        vcg::GLMeshAttributesInfo::RendAtts atts;
        if (dt.get(pm,atts))
        {
            switch(pm)
            {
            case (vcg::GLMeshAttributesInfo::PR_POINTS):
                {
                    glopts._perpoint_noshading = !atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL];
                    glopts._perpoint_mesh_color_enabled = !atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] && permeshcolor;
                    glopts._perpoint_fixed_color_enabled = !(glopts._perpoint_mesh_color_enabled);
                    break;
                }
            case (vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES):
            case (vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES):
                {
                    glopts._perwire_noshading = !atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL];
                    glopts._perwire_mesh_color_enabled = !atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] && permeshcolor;
                    glopts._perwire_fixed_color_enabled = !(glopts._perpoint_mesh_color_enabled);
                    break;
                }
            case (vcg::GLMeshAttributesInfo::PR_SOLID):
                {
                    glopts._persolid_noshading = (!atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL]) && (!atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL]);
                    glopts._persolid_mesh_color_enabled = !atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] && !atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] && permeshcolor;
                    glopts._persolid_fixed_color_enabled = !atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] && !atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] && !permeshcolor;
                    break;
                }
            }
        }
    }
    dt.set(glopts);
}

void MLPoliciesStandAloneFunctions::filterFauxUdpateAccordingToMeshMask( MeshModel* m,vcg::GLMeshAttributesInfo::RendAtts& atts )
{
    if (m == NULL)
        return;
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] &= m->hasDataMask(MeshModel::MM_VERTCOORD);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] &= m->hasDataMask(MeshModel::MM_VERTNORMAL);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] &= m->hasDataMask(MeshModel::MM_FACENORMAL);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] &= m->hasDataMask(MeshModel::MM_VERTCOLOR);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] &= m->hasDataMask(MeshModel::MM_FACECOLOR);
    //atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FIXEDCOLOR]
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] &= m->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
    atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] &= m->hasDataMask(MeshModel::MM_VERTTEXCOORD);
}

vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh( MeshModel* m )
{
    if (m != NULL)
    {
        if (m->cm.FN() > 0)
            return MLRenderingData::PR_SOLID;
        else
            if ((m->cm.VN() > 0) && (m->cm.EN() > 0))
                return MLRenderingData::PR_WIREFRAME_EDGES;
            else
                if (m->cm.VN() > 0)
                    return MLRenderingData::PR_POINTS;
    }
    return MLRenderingData::PR_ARITY;
}

//void MLPoliciesStandAloneFunctions::bestPrimitiveModalityMaskAfterUpdate( MeshModel* meshmodel,int meshmodelmask,const vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK& inputpm,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK& outputpm )
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

MLRenderingData::MLRenderingData()
    :PerViewData<MLPerViewGLOptions>()
{
    _glopts = new MLPerViewGLOptions();
}

MLRenderingData::MLRenderingData( const MLRenderingData& dt )
    :PerViewData<MLPerViewGLOptions>(dt)
{
}

bool MLRenderingData::set( vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,const vcg::GLMeshAttributesInfo::RendAtts& atts )
{
    vcg::GLMeshAttributesInfo::RendAtts tmp(atts);
    MLPoliciesStandAloneFunctions::maskMeaninglessAttributesPerPrimitiveModality(pm,tmp);
    //MLPoliciesStandAloneFunctions::setAttributePriorities(tmp);
    return PerViewData<MLPerViewGLOptions>::set(pm,tmp);
}

bool MLRenderingData::set( vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,vcg::GLMeshAttributesInfo::ATT_NAMES att,bool onoff )
{
    vcg::GLMeshAttributesInfo::RendAtts tmp;
    bool valid = this->get(pm,tmp);
    if (valid)
    {
        tmp[att] = onoff;
        return set(pm,tmp);
    }
    return false;
}

bool MLRenderingData::set( vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,bool onoff )
{
    return vcg::PerViewData<MLPerViewGLOptions>::set(pm,onoff);
}

void MLRenderingData::set( const MLPerViewGLOptions& opts )
{
    vcg::PerViewData<MLPerViewGLOptions>::set(opts);
}


MLPluginGLContext::MLPluginGLContext(const QGLFormat& frmt,QPaintDevice* dvc,MLSceneGLSharedDataContext& cont )
    :QGLContext(frmt,dvc),_shared(cont)
{

}

MLPluginGLContext::~MLPluginGLContext()
{
}

void MLPluginGLContext::drawMeshModel( int meshid) const
{
    MLPluginGLContext* id = const_cast<MLPluginGLContext*>(this);
    _shared.draw(meshid,id);
}

void MLPluginGLContext::setRenderingData( int meshid,MLRenderingData& dt )
{
    /*_shared.setRenderingDataPerMeshView(meshid,this,dt);
    _shared.manageBuffers(meshid);*/
}

void MLPluginGLContext::initPerViewRenderingData(int meshid,MLRenderingData& dt)
{
    _shared.requestInitPerMeshView(meshid,this,dt);
}

void MLPluginGLContext::removePerViewRenderindData()
{
    _shared.requestRemovePerMeshView(this);
}

void MLPluginGLContext::smoothModalitySuggestedRenderingData( MLRenderingData& dt )
{
    vcg::GLMeshAttributesInfo::RendAtts att;
    att[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
    att[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = true;
    dt.set(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY::PR_SOLID,att);
}

void MLPluginGLContext::pointModalitySuggestedRenderingData( MLRenderingData& dt )
{
    vcg::GLMeshAttributesInfo::RendAtts att;
    att[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
    att[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = true;
    dt.set(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY::PR_POINTS,att);
}



