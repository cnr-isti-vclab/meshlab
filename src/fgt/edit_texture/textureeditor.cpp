#include <QTabBar>
#include "textureeditor.h"

static int countPage = 1;	// Number of Tab in the texture's TabWidgets

TextureEditor::TextureEditor(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.groupBoxUVMapEdit->setVisible(false);
	ui.groupBoxSelect->setVisible(false);
	ui.groupBoxOptimize->setVisible(false);
	ui.groupBoxOption->setVisible(false);
	this->resize(this->width(), this->height() - ui.groupBoxUVMapEdit->sizeHint().height() 
		- ui.groupBoxSelect->sizeHint().height() - ui.groupBoxOptimize->sizeHint().height());
	initialDim = this->height() - 20;
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
	ra->setGeometry(MARGIN,MARGIN,400,400);
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
	ra->setGeometry(MARGIN,MARGIN,400,400);
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
	// This button does not hide the current panel...
	ui.editButton->setChecked(false);
	ui.optionButton->setChecked(false);
	ui.selectButton->setChecked(false);
	ui.optimizeButton->setChecked(false);
	ui.moveButton->setChecked(true);
}

void TextureEditor::on_editButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(1);
	ResetLayout();
	ui.editButton->setChecked(true);
	ui.groupBoxUVMapEdit->setVisible(true);
	//this->setMinimumHeight(initialDim + n);
	this->resize(this->width(), initialDim + ui.groupBoxUVMapEdit->sizeHint().height());
}

void TextureEditor::on_selectButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(2);
	ResetLayout();
	ui.selectButton->setChecked(true);
	ui.groupBoxSelect->setVisible(true);
	this->resize(this->width(),initialDim + ui.groupBoxSelect->sizeHint().height());
}

void TextureEditor::on_optimizeButton_clicked()
{
	ResetLayout();
	ui.optimizeButton->setChecked(true);
	ui.groupBoxOptimize->setVisible(true);
	this->resize(this->width(),initialDim + ui.groupBoxOptimize->sizeHint().height());
}

void TextureEditor::on_optionButton_clicked()
{
	ResetLayout();
	ui.optionButton->setChecked(true);
	ui.groupBoxOption->setVisible(true);
	this->resize(this->width(),initialDim + ui.groupBoxOption->sizeHint().height());
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

void TextureEditor::on_buttonClear_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ClearSelection();
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

void TextureEditor::on_radioButtonArea_toggled(bool t)
{
	if (t)
	{
		ui.radioButtonConnected->setChecked(false);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeSelectMode(0);
	}
}

void TextureEditor::on_radioButtonConnected_toggled(bool t)
{
	if (t)
	{
		ui.radioButtonArea->setChecked(false);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeSelectMode(1);
	}
}

void TextureEditor::ResetLayout()
{
	// Reset the layout of the widget:
	// uncheck all the buttons
	ui.editButton->setChecked(false);
	ui.moveButton->setChecked(false);
	ui.optionButton->setChecked(false);
	ui.selectButton->setChecked(false);
	ui.optimizeButton->setChecked(false);
	// Hide all the group box
	ui.groupBoxSelect->hide();
	ui.groupBoxUVMapEdit->hide();
	ui.groupBoxOptimize->hide();
	ui.groupBoxOption->hide();
}
