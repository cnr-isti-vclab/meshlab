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


void TextureEditor::AddRenderArea(QString texture, MeshModel *m, unsigned index)
{
	// Add a RenderArea widget to the TabWidget
	QString name = QString(texture);
	int i; for (i = texture.length() - 1; i >= 0; i--) if (texture.at(i) == QChar('/')) break;
	name.remove(0,i+1);

	QTabBar *t = new QTabBar(ui.tabWidget);
	RenderArea *ra= new RenderArea(t, texture, m, index);
	ra->setGeometry(MARGIN,MARGIN,AREADIM,AREADIM);
	ui.tabWidget->addTab(t, name);
	if (countPage == 1)
	{
		ui.tabWidget->removeTab(0);
		first = ra;
		ra->show();
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
}

void TextureEditor::UpStat(float u, float v, int faceN, int vertN, int countFace)
{
	// Manage the update of the controls from RenderArea to the editor, also enable/disable controls
	// <----- TODO
	area->update();
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

// Buttons
void TextureEditor::on_moveButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(0);
	ui.editButton->setChecked(false);
	ui.selectButton->setChecked(false);
	ui.optionButton->setChecked(false);
	ui.moveButton->setChecked(true);
}

void TextureEditor::on_editButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(1);
	ui.moveButton->setChecked(false);
	ui.selectButton->setChecked(false);
	ui.optionButton->setChecked(false);
	ui.editButton->setChecked(true);
}

void TextureEditor::on_selectButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(2);
	ui.editButton->setChecked(false);
	ui.moveButton->setChecked(false);
	ui.optionButton->setChecked(false);
	ui.selectButton->setChecked(true);
}

void TextureEditor::on_optionButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(3);
	ui.editButton->setChecked(false);
	ui.selectButton->setChecked(false);
	ui.moveButton->setChecked(false);
	ui.optionButton->setChecked(true);
}

void TextureEditor::on_clampButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RemapClamp();
}

void TextureEditor::on_modulusButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RemapMod();
}

void TextureEditor::on_rotateButton_clicked()
{
	if (ui.spinBoxAngle->value() != 0 && ui.spinBoxAngle->value() != 360 && ui.spinBoxAngle->value() != -360) 
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RotateComponent(ui.spinBoxAngle->value());
}

void TextureEditor::on_scaleButton_clicked()
{
	if (ui.spinBoxScale->value() != 100) 
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ScaleComponent(ui.spinBoxScale->value());
}

// Radio buttons
void TextureEditor::on_radioButtonMove_toggled(bool t)
{
	if (t)
	{
		ui.radioButtonSelect->setChecked(false);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeEditMode(0);
	}
}

void TextureEditor::on_radioButtonSelect_toggled(bool t)
{
	if (t)
	{
		ui.radioButtonMove->setChecked(false);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeEditMode(1);
	}
}
