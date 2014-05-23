#ifndef FILTERTHREAD_H
#define FILTERTHREAD_H

#include <QThread>
#include "mainwindow.h"

class FilterThread : public QThread
{
    Q_OBJECT
public:
    //Ideally PluginManager parameter should be const
    FilterThread(const QString& fname,const QMap<QString,QString>& parexpval,PluginManager& pm, MeshDocument& md);
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
    

};

#endif // FILTERTHREAD_H