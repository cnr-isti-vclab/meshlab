/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2008                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/


#include "stdparframe.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QClipboard>
#include <QColorDialog>
#include <QToolBar>
#include "../mainwindow.h"


using namespace vcg;

StdParFrame::StdParFrame(QWidget *p, QWidget *curr_gla )
    :QFrame(p)
{
    gla=curr_gla;
}

void StdParFrame::resetValues(RichParameterList &curParSet)
{
	assert((unsigned int)stdfieldwidgets.size() == curParSet.size());
    unsigned int i  =0;
    for(RichParameter* fpi : curParSet)
    {
        if (fpi != NULL)
            stdfieldwidgets[i++]->resetValue();
    }
}

/* creates widgets for the standard parameters */
void StdParFrame::loadFrameContent(RichParameterList &curParSet,MeshDocument * /*_mdPt*/ )
{
    if(layout()) delete layout();
    QGridLayout* glay = new QGridLayout();
    // QGridLayout *vlayout = new QGridLayout(this);
    //    vLayout->setAlignment(Qt::AlignTop);
    int i = 0;
    for(RichParameter* fpi : curParSet)
    {
		MeshLabWidget* wd = createWidgetFromRichParameter(this, *fpi, *fpi);
        //vLayout->addWidget(wd,i,0,1,1,Qt::AlignTop);
        stdfieldwidgets.push_back(wd);
        helpList.push_back(wd->helpLab);
        //glay->addItem(wd->leftItem(),i,0);
        //glay->addItem(wd->centralItem(),i,1);
        //glay->addItem(wd->rightItem(),i,2);
        wd->addWidgetToGridLayout(glay,i++);

    } // end for each parameter
    setLayout(glay);
    this->setMinimumSize(glay->sizeHint());
	glay->setSizeConstraint(QLayout::SetMinimumSize);
    this->showNormal();
    this->adjustSize();
}

void StdParFrame::toggleHelp()
{
    for(int i = 0; i < helpList.count(); i++)
        helpList.at(i)->setVisible(!helpList.at(i)->isVisible()) ;
    updateGeometry();
    adjustSize();
}

//void StdParFrame::readValues(ParameterDeclarationSet &curParSet)
void StdParFrame::readValues(RichParameterList &curParSet)
{
	assert(curParSet.size() == stdfieldwidgets.size());
	QVector<MeshLabWidget*>::iterator it = stdfieldwidgets.begin();
	for(RichParameter* p : curParSet)
	{
		QString sname = p->name();
		curParSet.setValue(sname,(*it)->widgetValue());
		++it;
	}
}

StdParFrame::~StdParFrame()
{

}

MeshLabWidget* StdParFrame::createWidgetFromRichParameter(
		QWidget* parent,
		const RichParameter& pd,
		const RichParameter& def)
{
    if (pd.value().isAbsPerc()){
		return new AbsPercWidget(parent, (const RichAbsPerc&)pd, (const RichAbsPerc&)def);
    }
    else if (pd.value().isDynamicFloat()){
		return new DynamicFloatWidget(parent, (const RichDynamicFloat&)pd, (const RichDynamicFloat&)def);
    }
    else if (pd.value().isEnum()){
		return new EnumWidget(parent, (const RichEnum&)pd, (const RichEnum&)def);
    }
    else if (pd.value().isBool()){
		return new BoolWidget(parent, (const RichBool&)pd, (const RichBool&)def);
    }
    else if (pd.value().isInt()){
		return new IntWidget(parent, (const RichInt&)pd, (const RichInt&)def);
    }
    else if (pd.value().isFloat()){
		return new FloatWidget(parent, (const RichFloat&)pd, (const RichFloat&)def);
    }
    else if (pd.value().isString()){
		return new StringWidget(parent, (const RichString&)pd, (const RichString&)def);
    }
    else if (pd.value().isMatrix44f()){
		return new Matrix44fWidget(parent, (const RichMatrix44f&)pd, (const RichMatrix44f&)def, reinterpret_cast<StdParFrame*>(parent)->gla);
    }
    else if (pd.value().isPoint3f()){
		return new Point3fWidget(parent, (const RichPoint3f&)pd, (const RichPoint3f&)def, reinterpret_cast<StdParFrame*>(parent)->gla);
    }
    else if (pd.value().isShotf()){
		return new ShotfWidget(parent, (const RichShotf&)pd, (const RichShotf&)def, reinterpret_cast<StdParFrame*>(parent)->gla);
    }
    else if (pd.value().isColor()){
		return new ColorWidget(parent, (const RichColor&)pd, (const RichColor&)def);
    }
    else if (pd.value().isFileName() && pd.stringType() == "RichOpenFile"){
		return new OpenFileWidget(parent, (const RichOpenFile&)pd, (const RichOpenFile&)def);
    }
    else if (pd.value().isFileName() && pd.stringType() == "RichSaveFile"){
		return new SaveFileWidget(parent, (const RichSaveFile&)pd, (const RichSaveFile&)def);
    }
    else if (pd.value().isMesh()){
		return new MeshWidget(parent, (const RichMesh&)pd, (const RichMesh&)def);
    }
    else {
        std::cerr << "RichParameter type not supported for widget creation.\n";
        assert(0);
        return nullptr;
    }
}

GenericParamDialog::GenericParamDialog(QWidget *p, RichParameterList *_curParSet, QString title, MeshDocument *_meshDocument)
    : QDialog(p) {
        stdParFrame=NULL;
        curParSet=_curParSet;
        meshDocument = _meshDocument;
        createFrame();
        if(!title.isEmpty())
            setWindowTitle(title);
}


// update the values of the widgets with the values in the paramlist;
void GenericParamDialog::resetValues()
{
    stdParFrame->resetValues(*curParSet);
}

void GenericParamDialog::toggleHelp()
{
    stdParFrame->toggleHelp();
    this->updateGeometry();
    this->adjustSize();
}


void GenericParamDialog::createFrame()
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    setLayout(vboxLayout);

    stdParFrame = new StdParFrame(this);
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


void GenericParamDialog::getAccept()
{
    stdParFrame->readValues(*curParSet);
    accept();
}

GenericParamDialog::~GenericParamDialog()
{
    delete stdParFrame;
}
















