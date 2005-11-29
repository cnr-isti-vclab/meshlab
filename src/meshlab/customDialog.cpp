#include "customDialog.h"
#include <QPalette>



CustomDialog::CustomDialog(QWidget * parent)
		:QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.ButtonBCB,SIGNAL(clicked()),this,SLOT(SetBackgroundBottomColor()));
	connect(ui.ButtonBCT,SIGNAL(clicked()),this,SLOT(SetBackgroundTopColor()));
	connect(ui.ButtonLC,SIGNAL(clicked()),this,SLOT(SetLogColor()));
	setFixedSize(200,120);
}


void CustomDialog::LoadCurrentSetting(const Color4b& bb,const Color4b& bt,const Color4b& l)
{
	cBackgroundBottom=bb;
	cBackgroundTop=bt;
 	cLog=l;
	QPalette pbb(QColor(bb.V(0),bb.V(1),bb.V(2)));
	QPalette pbt(QColor(bt.V(0),bt.V(1),bt.V(2)));
	QPalette pl(QColor(l.V(0),l.V(1),l.V(2)));
	ui.ButtonBCB->setPalette(pbb);
	ui.ButtonBCT->setPalette(pbt);
	ui.ButtonLC->setPalette(pl);
}

void CustomDialog::SetBackgroundBottomColor()
{
	QColor bb=QColorDialog::getColor(QColor(255,255,255,255),this);
	QPalette palette(bb);
	ui.ButtonBCB->setPalette(palette);
	cBackgroundBottom.V(0)=bb.red();
	cBackgroundBottom.V(1)=bb.green();
	cBackgroundBottom.V(2)=bb.blue();
	
}

void CustomDialog::SetBackgroundTopColor()
{
	QColor bt=QColorDialog::getColor(QColor(255,255,255,255),this);
	QPalette palette(bt);
	ui.ButtonBCT->setPalette(palette);
	cBackgroundTop.V(0)=bt.red();
	cBackgroundTop.V(1)=bt.green();
	cBackgroundTop.V(2)=bt.blue();
}

void CustomDialog::SetLogColor()
{
	QColor l=QColorDialog::getColor(QColor(255,255,255,255),this);
	QPalette palette(l);
	ui.ButtonLC->setPalette(palette);
	cLog.V(0)=l.red();
	cLog.V(1)=l.green();
	cLog.V(2)=l.blue();
}
