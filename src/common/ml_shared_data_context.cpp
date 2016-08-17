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

    QGLContext* ctx = makeCurrentGLContext();
    GLenum err = glewInit();
    doneCurrentGLContext(ctx);
    
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
        man->setPerViewInfo(viewerid,perviewdata);
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
        for(size_t ii = 0;ii < man->textureIDContainer().size();++ii)
        {
            GLuint textid = man->textureIDContainer().remove(man->textureIDContainer()[ii]);
            glDeleteTextures(1,&textid);
        }
        doneCurrentGLContext(ctx);

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
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imggl.width(), imggl.height(), GL_RGBA, GL_UNSIGNED_BYTE, imggl.bits() );
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

    PerMeshMultiViewManager* man = it.value();
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
        PerMeshMultiViewManager* man = it.value();
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
    QGLContext* ctx = makeCurrentGLContext();
    for(MeshIDManMap::iterator it = _meshboman.begin();it != _meshboman.end();++it)
    {
        PerMeshMultiViewManager* man = it.value();
        deAllocateTexturesPerMesh(it.key());
        it.value()->removeAllViewsAndDeallocateBO();
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
        QGLContext* ctx = makeCurrentGLContext();
        man->manageBuffers();
        doneCurrentGLContext(ctx);
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

void MLSceneGLSharedDataContext::updateGPUMemInfo()
{   
    QGLContext* ctx = makeCurrentGLContext();
    GLint allmem = 0;
    glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &allmem);

    GLint currentallocated = 0;
    glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentallocated);
    doneCurrentGLContext(ctx);
    emit currentAllocatedGPUMem((int)allmem,(int)currentallocated);
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

    MLRenderingData::PRIMITIVE_MODALITY_MASK tmpoutputpm = inputdt.getPrimitiveModalityMask();
    for(size_t pmind = 0;pmind < size_t(MLRenderingData::PR_ARITY); ++pmind)
    {
        MLRenderingData::PRIMITIVE_MODALITY pmc = MLRenderingData::PRIMITIVE_MODALITY(pmind);

            
        MLRenderingData::RendAtts tmpoutputatts;
        if (inputdt.get(MLRenderingData::PRIMITIVE_MODALITY(pmind),tmpoutputatts))
        {
            tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] &= meshmodel->hasDataMask(MeshModel::MM_VERTCOORD);
            tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] &= meshmodel->hasDataMask(MeshModel::MM_VERTNORMAL);
            tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] &= validfaces && meshmodel->hasDataMask(MeshModel::MM_FACENORMAL);
            tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] &= meshmodel->hasDataMask(MeshModel::MM_VERTCOLOR);
            tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] &= validfaces && meshmodel->hasDataMask(MeshModel::MM_FACECOLOR);

            //horrible trick caused by MeshLab GUI. In MeshLab exists just a button turning on/off the texture visualization.
            //Unfortunately the RenderMode::textureMode member field is not just a boolean value but and enum one.
            //The enum-value depends from the enabled attributes of input mesh.
            bool wedgetexture = meshmodel->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
            tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] &= (meshmodel->hasDataMask(MeshModel::MM_VERTTEXCOORD) && (!wedgetexture));
            tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] &= validfaces && wedgetexture;
            if (MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMesh(meshmodel,pmc))
                outputdt.set(pmc,tmpoutputatts);
        }
        else
            throw MLException(QString("MLPoliciesStandAloneFunctions: trying to access to a non defined PRIMITIVE_MODALITY!"));  

        
    }
    MLPoliciesStandAloneFunctions::setPerViewGLOptionsPriorities(meshmodel,outputdt);
}

void MLPoliciesStandAloneFunctions::fromMeshModelMaskToMLRenderingAtts( int meshmodelmask,MLRenderingData::RendAtts& atts)
{
    atts.reset();
    //connectivitychanged = bool(meshmodelmask | MeshModel::MM_FACEFACETOPO) || bool(meshmodelmask | MeshModel::MM_VERTFACETOPO) || bool(meshmodelmask | MeshModel::MM_VERTNUMBER) || bool(meshmodelmask | MeshModel::MM_FACENUMBER);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = bool(meshmodelmask & MeshModel::MM_VERTCOORD);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = bool(meshmodelmask & MeshModel::MM_VERTNORMAL);
    atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = bool(meshmodelmask & MeshModel::MM_FACENORMAL);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = bool(meshmodelmask & MeshModel::MM_VERTCOLOR);
    atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = bool(meshmodelmask & MeshModel::MM_FACECOLOR);
    //atts[MLRenderingData::ATT_NAMES::ATT_FIXEDCOLOR] = bool(meshmodelmask & MeshModel::MM_COLOR);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = bool(meshmodelmask & MeshModel::MM_VERTTEXCOORD);
    atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = bool(meshmodelmask & MeshModel::MM_WEDGTEXCOORD);
}

void MLPoliciesStandAloneFunctions::maskMeaninglessAttributesPerPrimitiveModality( MLRenderingData::PRIMITIVE_MODALITY pm,MLRenderingData::RendAtts& atts )
{
    switch(pm)
    {
    case (MLRenderingData::PR_POINTS):
        {
            atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = false;
            return;
        }
    case (MLRenderingData::PR_WIREFRAME_EDGES):
    case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
        {
            atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = false;  
            return;
        }
    case (MLRenderingData::PR_SOLID):
        {
            return;
        }
    case (MLRenderingData::PR_ARITY):
        {
            throw MLException("PR_ARITY passed as parameter!");
        }
    }
}

void MLPoliciesStandAloneFunctions::updatedRendAttsAccordingToPriorities(const MLRenderingData::PRIMITIVE_MODALITY /*pm*/,const MLRenderingData::RendAtts& updated,const MLRenderingData::RendAtts& current,MLRenderingData::RendAtts& result)
{
    MLRenderingData::RendAtts filteredupdated = updated;
    MLRenderingData::RendAtts tmp = current;
    tmp[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] |= filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION];
    tmp[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] |= filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL];
    tmp[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = (tmp[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] || filteredupdated[MLRenderingData::ATT_NAMES::ATT_FACENORMAL]) && !(filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL]);
    tmp[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] |= filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR];
    tmp[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = (tmp[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] || filteredupdated[MLRenderingData::ATT_NAMES::ATT_FACECOLOR]) && !(filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR]);
    //tmp[MLRenderingData::ATT_NAMES::ATT_FIXEDCOLOR] = (tmp[MLRenderingData::ATT_NAMES::ATT_FIXEDCOLOR] || filteredupdated[MLRenderingData::ATT_NAMES::ATT_FIXEDCOLOR]) && !(filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR]);
    tmp[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] |= filteredupdated[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE];
    tmp[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = (tmp[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] || filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE]) && !(filteredupdated[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE]);
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
        MLRenderingData::RendAtts tmpatts;
        tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
        tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
        tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = true;
        tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = true;

        dt.set(MLRenderingData::PR_POINTS,tmpatts);
       
        if (meshmodel->cm.FN() > 0)
        {
            dt.set(MLRenderingData::PR_POINTS,false);
            tmpatts.reset();
            tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
            tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
            tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = true;
            tmpatts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = true;
            dt.set(MLRenderingData::PR_SOLID,tmpatts);
        }
        else
        {
            if ((meshmodel->cm.FN() == 0) && ((meshmodel->cm.EN() > 0)))
            {
                dt.reset();
                tmpatts.reset();
                tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
                tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
                tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = true;
                dt.set(MLRenderingData::PR_WIREFRAME_EDGES,tmpatts);
                return;
            }
        }
    }
    MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh(meshmodel,dt,dtout);
}


bool MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMesh(MeshModel* m,const MLRenderingData::PRIMITIVE_MODALITY pm)
{
    bool validvert = (m->cm.VN() > 0);
    bool validfaces = (m->cm.FN() > 0);
    bool validedges = (m->cm.EN() > 0);
    return MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMeshInfo(validvert,validfaces,validedges,m->dataMask(),pm);
}

bool MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMeshInfo(bool validvert,bool validfaces,bool validedges,int meshmask,const MLRenderingData::PRIMITIVE_MODALITY pm)
{
    switch(pm)
    {
    case(MLRenderingData::PR_POINTS):
        {
            return validvert;
        }
    case(MLRenderingData::PR_WIREFRAME_EDGES):
        {
            return (validvert) && ((meshmask & MeshModel::MM_POLYGONAL) || ((!validfaces) && (validedges)));
        }
    case(MLRenderingData::PR_WIREFRAME_TRIANGLES):
    case(MLRenderingData::PR_SOLID):
        {
            return validvert && validfaces;
        }
    case (MLRenderingData::PR_ARITY):
        {
            throw MLException("PR_ARITY passed as parameter!");
        }      
    }
    return false;
}

bool MLPoliciesStandAloneFunctions::isPrimitiveModalityWorthToBeActivated(MLRenderingData::PRIMITIVE_MODALITY pm,bool wasvisualized,bool wasmeanigful,bool ismeaningful)
{
    (void) pm;
    if ((!wasmeanigful) && (ismeaningful))
        return true;
    if (wasmeanigful && ismeaningful && wasvisualized)
        return true;
    return false;
}

void MLPoliciesStandAloneFunctions::setAttributePriorities(MLRenderingData::RendAtts& atts )
{
    atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] &= !(atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL]);
    atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] &= !(atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR]);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] &= !(atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE]);
}

//void MLPoliciesStandAloneFunctions::setPerViewRenderingDataPriorities( MeshModel* mm,MLRenderingData& dt )
//{
//    if (mm == NULL)
//        return;
//    
//}

void MLPoliciesStandAloneFunctions::setPerViewGLOptionsPriorities(MeshModel* mm,MLRenderingData& dt )
{
    if (mm == NULL)
        return;
    bool permeshcolor = mm->hasDataMask(MeshModel::MM_COLOR);
    MLPerViewGLOptions glopts;
    if (!dt.get(glopts))
        return;
    if (permeshcolor)
        glopts._perpoint_mesh_color_enabled = true;

    for(MLRenderingData::PRIMITIVE_MODALITY pm = MLRenderingData::PRIMITIVE_MODALITY(0);pm < MLRenderingData::PR_ARITY;pm = MLRenderingData::next(pm))
    {
        MLRenderingData::RendAtts atts;
        if (dt.get(pm,atts))
        {
            switch(pm)
            {
            case (MLRenderingData::PR_POINTS):
                {
                    glopts._perpoint_noshading = !atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL];
                    glopts._perpoint_mesh_color_enabled = !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && permeshcolor;
                    glopts._perpoint_fixed_color_enabled = !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && !permeshcolor;
                    break;
                }
            case (MLRenderingData::PR_WIREFRAME_EDGES):
            case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
                {
                    glopts._perwire_noshading = !atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL];
                    glopts._perwire_mesh_color_enabled = !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && permeshcolor;
                    glopts._perwire_fixed_color_enabled =  !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && !atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] && !permeshcolor;
                    break;
                }
            case (MLRenderingData::PR_SOLID):
                {
                    glopts._persolid_noshading = (!atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL]) && (!atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL]);
                    glopts._persolid_mesh_color_enabled = !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && !atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] && permeshcolor;
                    glopts._persolid_fixed_color_enabled = !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && !atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] && !permeshcolor;
                    break;
                }
            case (MLRenderingData::PR_ARITY):
                {
                    throw MLException("PR_ARITY passed as parameter!");
                }
              
            }
        }
    }
    suggestedDefaultPerViewGLOptions(glopts);
    dt.set(glopts);
}

void MLPoliciesStandAloneFunctions::filterUselessUdpateAccordingToMeshMask( MeshModel* m,MLRenderingData::RendAtts& atts )
{
    if (m == NULL)
        return;
    atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] &= m->hasDataMask(MeshModel::MM_VERTCOORD);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] &= m->hasDataMask(MeshModel::MM_VERTNORMAL);
    atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] &= m->hasDataMask(MeshModel::MM_FACENORMAL);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] &= m->hasDataMask(MeshModel::MM_VERTCOLOR);
    atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] &= m->hasDataMask(MeshModel::MM_FACECOLOR);
    //atts[MLRenderingData::ATT_NAMES::ATT_FIXEDCOLOR]
    atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] &= m->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] &= m->hasDataMask(MeshModel::MM_VERTTEXCOORD);
}

MLRenderingData::PRIMITIVE_MODALITY MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh( MeshModel* m )
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

void MLPoliciesStandAloneFunctions::suggestedDefaultPerViewGLOptions( MLPerViewGLOptions& tmp )
{
    tmp._perbbox_fixed_color = vcg::Color4b(vcg::Color4b::White);
    tmp._perpoint_fixed_color = vcg::Color4b(vcg::Color4b::Yellow);
    tmp._perwire_fixed_color = vcg::Color4b(vcg::Color4b::DarkGray);
    tmp._persolid_fixed_color = vcg::Color4b(vcg::Color4b::LightGray);
	tmp._sel_enabled = true;
	tmp._peredge_extra_enabled = true;
}

//void MLPoliciesStandAloneFunctions::bestPrimitiveModalityMaskAfterUpdate( MeshModel* meshmodel,int meshmodelmask,const MLRenderingData::PRIMITIVE_MODALITY_MASK& inputpm,MLRenderingData::PRIMITIVE_MODALITY_MASK& outputpm )
//{
//    MLRenderingData::PRIMITIVE_MODALITY_MASK tmpmask = 0;
//    if (meshmodel->cm.VN() == 0)
//    {
//        outputpm = MLRenderingData::PR_NONE;
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

bool MLRenderingData::set( MLRenderingData::PRIMITIVE_MODALITY pm,const MLRenderingData::RendAtts& atts )
{
    MLRenderingData::RendAtts tmp(atts);
    MLPoliciesStandAloneFunctions::maskMeaninglessAttributesPerPrimitiveModality(pm,tmp);
    //MLPoliciesStandAloneFunctions::setAttributePriorities(tmp);
    return PerViewData<MLPerViewGLOptions>::set(pm,tmp);
}

bool MLRenderingData::set( MLRenderingData::PRIMITIVE_MODALITY pm,MLRenderingData::ATT_NAMES att,bool onoff )
{
    MLRenderingData::RendAtts tmp;
    bool valid = this->get(pm,tmp);
    if (valid)
    {
        tmp[att] = onoff;
        return set(pm,tmp);
    }
    return false;
}

bool MLRenderingData::set( MLRenderingData::PRIMITIVE_MODALITY pm,bool onoff )
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
    _shared.requestSetPerMeshViewRenderingData(QThread::currentThread(),meshid,this,dt);
}

void MLPluginGLContext::initPerViewRenderingData(int meshid,MLRenderingData& dt)
{
    _shared.requestInitPerMeshView(QThread::currentThread(),meshid,this,dt);
}

void MLPluginGLContext::removePerViewRenderindData()
{
    _shared.requestRemovePerMeshView(QThread::currentThread(),this);
}


void MLPluginGLContext::meshAttributesUpdated( int meshid,bool conntectivitychanged,const MLRenderingData::RendAtts& dt )
{
    _shared.requestMeshAttributesUpdated(QThread::currentThread(),meshid,conntectivitychanged,dt);
}


void MLPluginGLContext::smoothModalitySuggestedRenderingData( MLRenderingData& dt )
{
    MLRenderingData::RendAtts att;
    att[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
    att[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
    dt.set(MLRenderingData::PRIMITIVE_MODALITY::PR_SOLID,att);
}

void MLPluginGLContext::pointModalitySuggestedRenderingData( MLRenderingData& dt )
{
    MLRenderingData::RendAtts att;
    att[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
    att[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
    dt.set(MLRenderingData::PRIMITIVE_MODALITY::PR_POINTS,att);
}



