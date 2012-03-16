#include "mainwindow.h"
#include <exception>

#include "../common/scriptinterface.h"
#include "../common/meshlabdocumentxml.h"
#include "../common/meshlabdocumentbundler.h"
#include "filterthread.h"

FilterThread::FilterThread(QString fname,MeshLabXMLFilterContainer *mfc, MeshDocument& md,EnvWrap& env, QObject *parent) 
:QThread(parent), _fname(fname), _mfc(mfc),_md(md),_env(env)
{
}

FilterThread *cur=0;

bool FilterThread::QCallBackLocal(const int pos, const char * str)
{
	/*emit cur->ThreadCB(pos,QString(str));*/
	return true;
}

void FilterThread::run()
{
	/*assert(cur==0);
	cur=this;*/
	_ret = _mfc->filterInterface->applyFilter(_fname, _md, _env, QCallBackLocal);
	/*cur=0;*/
}