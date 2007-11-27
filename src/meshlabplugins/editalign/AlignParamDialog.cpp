#include "AlignParamDialog.h"
#include <meshlab/stdpardialog.h>


AlignParamDialog::AlignParamDialog(QWidget *p, FilterParameterSet *_curParSet) :QDialog(p)
{
		stdParFrame=NULL;
		curParSet=_curParSet;
		createFrame();
}

// update the values of the widgets with the values in the paramlist;
void AlignParamDialog::resetValues()
{
		stdParFrame->resetValues(*defaultParSet);
}

void AlignParamDialog::toggleHelp()
{	
	stdParFrame->toggleHelp();
	this->updateGeometry();
	this->adjustSize();
}


void AlignParamDialog::createFrame()
{
	setWindowTitle("Alignment Parameters");

	QVBoxLayout *vboxLayout = new QVBoxLayout(this);
	setLayout(vboxLayout);
	
	stdParFrame = new StdParFrame(this);
	stdParFrame->loadFrameContent(*curParSet);
  layout()->addWidget(stdParFrame);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help  |QDialogButtonBox::Reset  | QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
	layout()->addWidget(buttonBox);
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
	
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
		
	this->showNormal();
	this->adjustSize();		
}


void AlignParamDialog::getAccept()
{
	stdParFrame->readValues(*curParSet);
	accept();
}

