#include <GL/glew.h>
#include <QObject>
#include <QDialog>
#include <QtGui>
#include <wrap/gui/trackball.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "AlignPairWidget.h"

#ifndef __VCG_ALIGNPAIRDIALOG
#define __VCG_ALIGNPAIRDIALOG

class AlignPairDialog: public QDialog
{
	Q_OBJECT 
public:
	AlignPairWidget *aa;

  AlignPairDialog (QWidget * parent = 0) : QDialog(parent)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
	
		aa=new AlignPairWidget(this);
		QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
		
		layout->addWidget(aa);
		layout->addWidget(buttonBox);
	
		setLayout(layout);
		adjustSize();
		connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}
	
	
//	public slots:	
};
#endif