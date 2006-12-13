#ifndef MESHLAB_STDPARDIALOG_H
#define MESHLAB_STDPARDIALOG_H

#include <QtCore>
#include <QAction>
#include <QtGui>

#include "meshmodel.h"
#include "filterparameter.h"
#include "interfaces.h"

// frame for the standard plugin window
class MeshlabStdDialogFrame : public QFrame
{
public:
    MeshlabStdDialogFrame(QWidget *parent)
        : QFrame(parent)
    {
    }

};


// standard plugin window
class MeshlabStdDialog : public QDockWidget
{
	  Q_OBJECT

public:
  MeshlabStdDialog(QWidget *p):QDockWidget(QString("Plugin"),p)
  {
	qf = NULL;
	initValues();
	//resetMe();
  }

	void initValues();
	void resetMe();
	void loadPluginAction(MeshFilterInterface *mfi,MeshModel *mm,QAction *q,MainWindowInterface *mwi);




  private slots:
	 void applyClick();
 
protected:
	QFrame *qf;
	QAction *curaction;
	MainWindowInterface *curmwi;
	QVector<QWidget *> stdfieldwidgets;
	QWidget *curextra;
	StdParList parlist;
};




#endif