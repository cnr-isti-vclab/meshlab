#ifndef FILTERTHREAD_H
#define FILTERTHREAD_H

#include <QThread>
#include "mainwindow.h"

class FilterThread : public QThread
{
	Q_OBJECT
public:
	FilterThread(const QString& fname,MeshLabXMLFilterContainer *mfc, MeshDocument& md,Env& env);
	~FilterThread();
	MeshLabXMLFilterContainer *_mfc;
	QString _fname;
	MeshDocument& _md;
	EnvWrap _envwrap;
	bool _ret;
	static bool localCallBack(const int pos, const char * str);

protected:
	void run();

signals:
	void threadCB(const int pos,const QString& str);
private:
	static FilterThread* _cur;
	QGLWidget* _glwid;
};

#endif // FILTERTHREAD_H