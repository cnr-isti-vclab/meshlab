#include "mainwindow.h"
#include <exception>

#include "../common/scriptinterface.h"
#include "../common/meshlabdocumentxml.h"
#include "../common/meshlabdocumentbundler.h"
#include "filterthread.h"

FilterThread* FilterThread::_cur = NULL;

FilterThread::FilterThread(const QString& fname,const QMap<QString,QString>& parexpval,PluginManager& pm, MeshDocument& md,MainWindow* mw)
:QThread(),_fname(fname),_parexpval(parexpval),_pm(pm),_md(md),_glwid(NULL),_mw(mw)
{
    if ((_mw != NULL) && (_mw->currentViewContainer() != NULL))
        _glwid = new QGLWidget(NULL,_mw->currentViewContainer()->sharedDataContext());
}

bool FilterThread::localCallBack(const int pos, const char * str)
{
    QString st(str);
    static QElapsedTimer currTime;
    if(currTime.isValid() && currTime.elapsed()< 100) return true;
    emit _cur->threadCB(pos,st);
    currTime.start();
    return true;
}


void FilterThread::run()
{
    try
    {
        Env env;
		QScriptValue val;
	
		val = env.loadMLScriptEnv(_md, _pm);
		if (val.isError())
			throw JavaScriptException("A Plugin-bridge-code generated a JavaScript Error: " + val.toString() + "\n");

        QMap<QString,MeshLabXMLFilterContainer>::iterator it =_pm.stringXMLFilterMap.find(_fname);
        if ((it == _pm.stringXMLFilterMap.end()) || (it->xmlInfo == NULL))
            throw MLException("Filter " + _fname + " has not been found.\n");
        if (it->filterInterface != NULL)
        {
            MLSceneGLSharedDataContext* cont = NULL;
            if ((_mw != NULL) && (_mw->currentViewContainer() != NULL))
            {    
                cont = _mw->currentViewContainer()->sharedDataContext();
                it->filterInterface->glContext = new MLPluginGLContext(QGLFormat::defaultFormat(),_glwid->context()->device(),(*cont));
                it->filterInterface->glContext->create(_glwid->context());
                MLRenderingData dt;
                MLRenderingData::RendAtts atts;
                atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
                atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;

                if (it->xmlInfo->filterAttribute(_fname,MLXMLElNames::filterArity) == MLXMLElNames::singleMeshArity)
                {
                    MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(_md.mm());
                    if ((pm != MLRenderingData::PR_ARITY) && (_md.mm() != NULL))
                    {
                        dt.set(pm,atts);
                        it->filterInterface->glContext->initPerViewRenderingData(_md.mm()->id(),dt);
                    }
                }
                else
                {
                    for(int ii = 0;ii < _md.meshList.size();++ii)
                    {
                        MeshModel* mm = _md.meshList[ii];
                        MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(mm);
                        if ((pm != MLRenderingData::PR_ARITY) && (mm != NULL))
                        {
                            dt.set(pm,atts);
                            it->filterInterface->glContext->initPerViewRenderingData(mm->id(),dt);
                        }
                    }
                }

            }
			for (QMap<QString, QString>::const_iterator itp = _parexpval.constBegin(); itp != _parexpval.constEnd(); ++itp)
				env.insertExpressionBinding(itp.key(), itp.value());
            EnvWrap envwrap(env);
            _cur = this;
            _success = it->filterInterface->applyFilter(_fname, _md, envwrap, &localCallBack);
			for (MeshModel* mm = _md.nextMesh(); mm != NULL; mm = _md.nextMesh(mm))
				vcg::tri::Allocator<CMeshO>::CompactEveryVector(mm->cm);
            _cur = NULL;
			if (it->filterInterface->glContext != NULL)
			{
				it->filterInterface->glContext->removePerViewRenderindData();
				delete it->filterInterface->glContext;
			}
        }
        else
            throw MLException("There is not yet support for not-C++ filters.");
    }
    catch (MLException& e)
    {
        _md.Log.Log(GLLogStream::SYSTEM,e.what());
    }
}

FilterThread::~FilterThread()
{
    delete _glwid;
}
