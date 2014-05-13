#include "mainwindow.h"
#include <exception>

#include "../common/scriptinterface.h"
#include "../common/meshlabdocumentxml.h"
#include "../common/meshlabdocumentbundler.h"
#include "filterthread.h"

FilterThread* FilterThread::_cur = NULL;

FilterThread::FilterThread(const QString& fname,MeshLabXMLFilterContainer *mfc, MeshDocument& md,Env& env) 
:QThread(), _mfc(mfc), _fname(fname),_md(md),_envwrap(env),_glwid(NULL)
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
	QGLFormat defForm = QGLFormat::defaultFormat();
	_mfc->filterInterface->glContext = new QGLContext(defForm,_glwid->context()->device());
	_mfc->filterInterface->glContext->create(_glwid->context());
	_cur = this;
	_ret = _mfc->filterInterface->applyFilter(_fname, _md, _envwrap, &localCallBack);
	_cur = NULL;
}

FilterThread::~FilterThread()
{
	delete _glwid;
}
