#ifndef FILTERTHREAD_H
#define FILTERTHREAD_H

#include <QThread>
#include "mainwindow.h"

class FilterThread : public QThread
{
	Q_OBJECT
public:
	explicit FilterThread(QString fname,MeshLabXMLFilterContainer *mfc, MeshDocument& md,EnvWrap& env, QObject *parent = 0);
	~FilterThread();
	MeshLabXMLFilterContainer *_mfc;
	QString _fname;
	MeshDocument& _md;
	EnvWrap & _env;
	bool _ret;
	static bool QCallBackLocal(const int pos, const char * str);

protected:
	void run();

signals:
	void ThreadCB(const int pos,const QString& str);
private:
	QGLWidget* _glwid;
};

#endif // FILTERTHREAD_H