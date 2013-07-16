#include "mainwindow.h"
#include <exception>

#include "../common/scriptinterface.h"
#include "../common/meshlabdocumentxml.h"
#include "../common/meshlabdocumentbundler.h"
#include "filterthread.h"

FilterThread::FilterThread(QString fname,MeshLabXMLFilterContainer *mfc, MeshDocument& md,EnvWrap& env,QObject *parent) 
:QThread(parent), _mfc(mfc), _fname(fname),_md(md),_env(env),_glwid(NULL)
{
	_glwid = new QGLWidget();
}

FilterThread *cur=0;

bool FilterThread::QCallBackLocal(const int pos, const char * str)
{
	QString st(str);
	static QTime currTime;
	if(currTime.elapsed()< 100) return true;
	emit cur->ThreadCB(pos,st);
	currTime.start();
	return true;
}

void FilterThread::run()
{
	/*assert(cur==0)*/
	QGLFormat defForm = QGLFormat::defaultFormat();
	_mfc->filterInterface->glContext = new QGLContext(defForm,_glwid->context()->device());
	_mfc->filterInterface->glContext->create(_glwid->context());
	cur=this;
	_ret = _mfc->filterInterface->applyFilter(_fname, _md, _env, QCallBackLocal);
	cur=0;

}

FilterThread::~FilterThread()
{
	delete _glwid;
}
