#include "richparameterlistdialog.h"

#include <QDialogButtonBox>

RichParameterListDialog::RichParameterListDialog(QWidget *p, RichParameterList *_curParSet, QString title, MeshDocument *_meshDocument)
	: QDialog(p) {
		stdParFrame=NULL;
		curParSet=_curParSet;
		meshDocument = _meshDocument;
		createFrame();
		if(!title.isEmpty())
			setWindowTitle(title);
}


// update the values of the widgets with the values in the paramlist;
void RichParameterListDialog::resetValues()
{
	stdParFrame->resetValues(*curParSet);
}

void RichParameterListDialog::toggleHelp()
{
	stdParFrame->toggleHelp();
	this->updateGeometry();
	this->adjustSize();
}


void RichParameterListDialog::createFrame()
{
	QVBoxLayout *vboxLayout = new QVBoxLayout(this);
	setLayout(vboxLayout);

	stdParFrame = new RichParameterListFrame(this);
	stdParFrame->loadFrameContent(*curParSet, meshDocument);
	layout()->addWidget(stdParFrame);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
	//add the reset button so we can get its signals
	QPushButton *resetButton = buttonBox->addButton(QDialogButtonBox::Reset);
	layout()->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	this->setMinimumSize(stdParFrame->sizeHint());
	this->showNormal();
	this->adjustSize();
}


void RichParameterListDialog::getAccept()
{
	stdParFrame->readValues(*curParSet);
	accept();
}

RichParameterListDialog::~RichParameterListDialog()
{
	delete stdParFrame;
}


