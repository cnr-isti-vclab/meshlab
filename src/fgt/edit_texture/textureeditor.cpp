#include <QTabBar>
#include "textureeditor.h"

static int countPage = 1;	// Number of Tab in the texture's TabWidgets

TextureEditor::TextureEditor(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	oldX = 0; oldY = 0;
	oldU = -1; oldV = -1;
	lock = false;
}

TextureEditor::~TextureEditor()
{
	Reset();
}

void TextureEditor::Reset()
{
	for (int i = 1; i < countPage; i++) ui.tabWidget->removeTab(i);
	countPage = 1;
	first = 0;
}

void TextureEditor::on_faceRB_toggled(bool t)
{
	// Rado button Remap -> Change border managment: Repeat
	if (t)
	{
		ui.vertRB->setChecked(false);
		ui.remapRB->setChecked(false);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RemapRepeat();
	}
}

void TextureEditor::on_vertRB_toggled(bool t)
{
	// Rado button Clamp -> Change border managment: Clamp
	if (t)
	{
		ui.faceRB->setChecked(false);
		ui.remapRB->setChecked(false);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RemapClamp();
	}
}

void TextureEditor::on_remapRB_toggled(bool t)
{
	// Rado button Modulus -> Change border managment: Modulus
	if (t)
	{
		ui.faceRB->setChecked(false);
		ui.vertRB->setChecked(false);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RemapMod();
	}
}

void TextureEditor::AddRenderArea(QString texture, vector<Container> map, bool or)
{
	// Add a RenderArea widget to the TabWidget
	QString name = QString(texture);
	int i; for (i = texture.length() - 1; i >= 0; i--) if (texture.at(i) == QChar('/')) break;
	name.remove(0,i+1);

	QTabBar *t = new QTabBar(ui.tabWidget);
	RenderArea *ra= new RenderArea(t, texture, map, or);
	ra->ChangeMode(ui.tabWidgetEdit->currentIndex());
	ra->setGeometry(MARGIN,MARGIN,AREADIM,AREADIM);
	ui.tabWidget->addTab(t, name);
	if (countPage == 1)
	{
		ui.tabWidget->removeTab(0);
		first = ra;
		ra->show();
		if (!or){ ui.vertRB->setChecked(true); ui.faceRB->setChecked(false); }
	}
	countPage++;
	QObject::connect(ra, SIGNAL(UpdateStat(float,float,int,int, int)),this, SLOT(UpStat(float,float,int,int, int)));

}

void TextureEditor::AddEmptyRenderArea()
{
	// Add an empty render area (the model doesn't has any texture)
	RenderArea *ra= new RenderArea(ui.tabWidget->widget(0));
	first = ra;
	ra->setGeometry(MARGIN,MARGIN,AREADIM,AREADIM);
	ra->show();
	ui.applyButton->setEnabled(false);
}


void TextureEditor::on_applyButton_clicked()
{
	// Change the uv coord of the model reading the RenderArea structure
	ui.StatusLabel->setText(QString("Saving..."));
	int value = 0;
	ui.progressBar->setValue(value);
	for (int i = 0; i < ui.tabWidget->count(); i++)
	{
		vector<Container> tmp = ((RenderArea*)ui.tabWidget->widget(i)->childAt(MARGIN,MARGIN))->map;
		for (unsigned j = 0; j < tmp.size(); j++)
		{
			ui.progressBar->setValue(++value);
			ui.progressBar->update();
			for (unsigned y = 0; y < tmp[j].GetFaceSize(); y++)
			{
				tmp[j].GetFaceAt(y)->WT(tmp[j].GetWTAt(y)).u() = tmp[j].GetU();
				tmp[j].GetFaceAt(y)->WT(tmp[j].GetWTAt(y)).v() = tmp[j].GetV();
			}
		}
		value = 0;
	}
	ui.progressBar->setValue(0);
	ui.StatusLabel->setText(QString("Idle"));
	area->update();
}

void TextureEditor::on_tabWidget_currentChanged(int)
{
	// User select a texture
	if (((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->out)
	{
		ui.faceRB->setChecked(true);
		ui.vertRB->setChecked(false);
		ui.remapRB->setChecked(false);
	}
	else
	{
		ui.faceRB->setChecked(false);
		ui.vertRB->setChecked(true);
		ui.remapRB->setChecked(false);
	}
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(ui.tabWidgetEdit->currentIndex());
}

void TextureEditor::on_SpinBoxU_valueChanged(double)
{
	// Change of U Coord
	HandleSpinBoxUV();
}

void TextureEditor::on_SpinBoxV_valueChanged(double)
{
	// Change of V Coord
	HandleSpinBoxUV();
}

void TextureEditor::HandleSpinBoxUV()
{
	// Manage the update of the uv coord from the editor to RenderArea
	if (!lock)
	{
		int a = ((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->highClick;
		if (a != -1 && (ui.SpinBoxU->value() != oldU || ui.SpinBoxV->value() != oldV)) 
		{
			((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->UpdateVertex((float)ui.SpinBoxU->value(),(float)ui.SpinBoxV->value());
			oldU = ui.SpinBoxU->value(); oldV = ui.SpinBoxV->value();
		}
	}
}
void TextureEditor::UpStat(float u, float v, int faceN, int vertN, int countFace)
{
	// Manage the update of the controls from RenderArea to the editor, also enable/disable controls
	lock = true;
	if (faceN > NO_CHANGE) 
	{
		ui.SpinBoxU->setEnabled(true);
		ui.SpinBoxV->setEnabled(true);
		ui.SpinBoxU->setValue((double)u); ui.SpinBoxU->update();
		ui.SpinBoxV->setValue((double)v); ui.SpinBoxV->update();
	}
	if (faceN == RESET)
	{
		ui.spinBoxX->setValue(0); oldX = 0;
		ui.spinBoxY->setValue(0); oldY = 0;
		ui.SpinBoxU->setEnabled(false);
		ui.SpinBoxV->setEnabled(false);
	}
	else if (faceN == ENABLECMD)
	{
		ui.spinBoxX->setEnabled(true);
		ui.spinBoxY->setEnabled(true);
		ui.spinBoxX->setValue(0); oldX = 0;
		ui.spinBoxY->setValue(0); oldY = 0;
		ui.ApplyRotate->setEnabled(true);
		ui.ApplyScale->setEnabled(true);
	}
	else if (faceN == DISABLECMD)
	{
		ui.spinBoxX->setEnabled(false);
		ui.spinBoxY->setEnabled(false);
		ui.ApplyRotate->setEnabled(false);
		ui.ApplyScale->setEnabled(false);
	}
	if (countFace > 0) ui.LabelFaceNum->setText(QString("%1").arg(countFace));
	else ui.LabelFaceNum->setText(QString("0"));
	lock = false;
}


void TextureEditor::SetProgress(int val)
{
	// Change the value of the progress bar
	ui.progressBar->setValue(val);
	ui.progressBar->update();
}

void TextureEditor::SetProgressMax(int val)
{
	// Set the max of progress bar
	ui.progressBar->setMaximum(val);
}

void TextureEditor::SetStatusText(QString text)
{
	// Change the text of the status label
	ui.StatusLabel->setText(text);
}

void TextureEditor::on_tabWidgetEdit_currentChanged(int index)
{
	// Change of edit tab from user -> change the editing mode
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(index);
}

void TextureEditor::on_spinBoxX_valueChanged(int)
{
	// Change of component X pos
	HandleSpinBoxXY();
}

void TextureEditor::on_spinBoxY_valueChanged(int)
{
	// Change of component Y pos
	HandleSpinBoxXY();
}

void TextureEditor::HandleSpinBoxXY()
{
	// Manage the update of the component position from the editor to RenderArea
	if (!lock)
	{
		int a = ((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->highComp;
		if (a != -1) 
		{
			((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->UpdateComponentPos(ui.spinBoxX->value() - oldX, ui.spinBoxY->value() - oldY);
			oldX = ui.spinBoxX->value();
			oldY = ui.spinBoxY->value();
		}
	}
}

void TextureEditor::on_ApplyRotate_clicked()
{
	// Click of the rotate button
	int a = ((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->highComp;
	if (a != -1 && ui.spinBoxAngle->value() != 0 && ui.spinBoxAngle->value() != 360 && ui.spinBoxAngle->value() != -360) 
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RotateComponent(ui.spinBoxAngle->value());
}

void TextureEditor::on_ApplyScale_clicked()
{
	// Click of the scale button
	int a = ((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->highComp;
	if (a != -1 && ui.spinBoxScale->value() != 100) 
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ScaleComponent(ui.spinBoxScale->value());
}