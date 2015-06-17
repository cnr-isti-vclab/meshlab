#ifndef FILTERTHREAD_H
#define FILTERTHREAD_H

#include <QThread>
#include "mainwindow.h"

class FilterThread : public QThread
{
    Q_OBJECT
public:
    //Ideally PluginManager parameter should be const
    //and MainWindow parameter should not exist!
	FilterThread(const QString& fname,const QMap<QString,QString>& parexpval,PluginManager& pm, MeshDocument& md,MainWindow* mw);
    ~FilterThread();
    inline bool succeed() const {return _success;}
    inline QString filterName() const {return _fname;}
    static bool localCallBack(const int pos, const char * str);

protected:
    void run();

signals:
    void threadCB(const int pos,const QString& str);
private:
    static FilterThread* _cur;
    QString _fname;
    QMap<QString,QString> _parexpval;
    //ideally this should be const
    PluginManager& _pm;
    MeshDocument& _md;
    QGLWidget* _glwid;
    bool _success;

	//WARNING!!!!!!!!!!!!!!!MainWindow parameter should not exist at all! it has been introduced only to give access to global parameters to the script environment
    MainWindow* _mw;
	/*****************************************************************************************************************************************************************/
};

#endif // FILTERTHREAD_H