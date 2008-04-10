#include <QTabBar>
#include "textureeditor.h"

static int countPage = 1;	// Number of Tab in the texture's TabWidgets

TextureEditor::TextureEditor(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
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
	ra->setGeometry(MARGIN,MARGIN,MAXW,MAXH);
	ui.tabWidget->addTab(t, name);
	if (countPage == 1)
	{
		ui.tabWidget->removeTab(0);
		first = ra;
		ra->show();
	}
	countPage++;
	QObject::connect(ra, SIGNAL(UpdateModel()),this, SLOT(UpdateModel()));

}

void TextureEditor::AddEmptyRenderArea()
{
	// Add an empty render area (the model doesn't has any texture)
	RenderArea *ra= new RenderArea(ui.tabWidget->widget(0));
	first = ra;
	ra->setGeometry(MARGIN,MARGIN,400,400);
	ra->show();
}

void TextureEditor::UpdateModel()
{
	// Update the mesh after an edit
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

void TextureEditor::ResetLayout()
{
	// uncheck all the buttons
	ui.connectedButton->setChecked(false);
	ui.moveButton->setChecked(false);
	ui.selectButton->setChecked(false);
	ui.vertexButton->setChecked(false);
}

void TextureEditor::SmoothTextureCoordinates()
{
	// <-------
}

// Buttons
void TextureEditor::on_moveButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(0);
	ui.moveButton->setChecked(true);
}

void TextureEditor::on_connectedButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeSelectMode(1);
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(2);
	ui.connectedButton->setChecked(true);
}

void TextureEditor::on_selectButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeSelectMode(0);
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(2);
	ui.selectButton->setChecked(true);
}

void TextureEditor::on_vertexButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeSelectMode(2);
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(2);
	ui.vertexButton->setChecked(true);
}

void TextureEditor::on_clampButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RemapClamp();
}

void TextureEditor::on_modulusButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RemapMod();
}

void TextureEditor::on_smoothButton_clicked()
{
	// Apply the smooth
	SmoothTextureCoordinates();
}