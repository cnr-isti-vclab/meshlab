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
  QCheckBox * allowScalingCB;
  AlignPairDialog (QWidget * parent = 0) : QDialog(parent)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
	
		aa=new AlignPairWidget(this);
		QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
		allowScalingCB = new QCheckBox("Allow Scaling");
		allowScalingCB->setChecked(false);
		
		QLabel *helpLabel = new QLabel("Choose at least 4 matching pair of points on the two meshes. <br>Double Click over each mesh to add new points. Choose points in consistent order");
		layout->addWidget(helpLabel);
		layout->addWidget(aa);
		layout->addWidget(allowScalingCB);
		layout->addWidget(buttonBox);
	
		setLayout(layout);
		adjustSize();
		connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	
//	public slots:	
};
#endif