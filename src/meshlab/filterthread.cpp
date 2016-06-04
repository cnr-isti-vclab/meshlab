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
    _glwid = new QGLWidget();
}

bool FilterThread::localCallBack(const int pos, const char * str)
{
    QString st(str);
    static QTime currTime;
    if(currTime.elapsed()< 100) return true;
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
	
		if (_mw != NULL)
			val = env.loadMLScriptEnv(_md,_pm,_mw->currentGlobalPars());
		else
			val = env.loadMLScriptEnv(_md,_pm);
        if (val.isError())
            throw JavaScriptException("A Plugin-bridge-code generated a JavaScript Error: " + val.toString() + "\n");

        QMap<QString,MeshLabXMLFilterContainer>::iterator it =_pm.stringXMLFilterMap.find(_fname);
        if ((it == _pm.stringXMLFilterMap.end()) || (it->xmlInfo == NULL))
            throw MLException("Filter " + _fname + " has not been found.\n");
        if (it->filterInterface != NULL)
        {

            it->filterInterface->glContext = new QGLContext(QGLFormat::defaultFormat(),_glwid->context()->device());
            it->filterInterface->glContext->create(_glwid->context());
            for (QMap<QString,QString>::const_iterator itp = _parexpval.constBegin();itp != _parexpval.constEnd();++itp)
                env.insertExpressionBinding(itp.key(),itp.value());
            EnvWrap envwrap(env);
            _cur = this;
            _success = it->filterInterface->applyFilter(_fname, _md, envwrap, &localCallBack);
            _cur = NULL;

            delete it->filterInterface->glContext;
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
