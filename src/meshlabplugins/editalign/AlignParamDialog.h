#include <GL/glew.h>
#include <QObject>
#include <QDialog>
#include <QtGui>
#include <wrap/gui/trackball.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/stdpardialog.h>
#include "AlignPairWidget.h"

#ifndef __VCG_ALIGNPARAMDIALOG
#define __VCG_ALIGNPARAMDIALOG

class AlignParamDialog: public QDialog
{
	Q_OBJECT 
public:
  AlignParamDialog(QWidget *p, FilterParameterSet *_curParSet); 
	
	FilterParameterSet *curParSet;
	FilterParameterSet *defaultParSet;
	StdParFrame *stdParFrame;
	
	void createFrame();
  void resetValues();
	
	public slots:	
	void getAccept();
	void toggleHelp();
};
#endif