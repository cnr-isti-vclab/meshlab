/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <QTabBar>
#include "textureeditor.h"
#include <qfiledialog.h>
#include <meshlab/glarea.h>

static int countPage = 1;	// Number of Tab in the texture's TabWidgets

TextureEditor::TextureEditor(QWidget *parent, MeshModel *m, GLArea *gla, bool deg) : QWidget(parent)
{
	ui.setupUi(this);
	area = gla;
	model = m;
	degenere = deg;
	QObject::connect(this, SIGNAL(updateTexture(void)), gla, SLOT(updateTexture(void)));
}

TextureEditor::~TextureEditor()
{
	Reset();
}

void TextureEditor::Reset()
{
	for (int i = 1; i < countPage; i++) ui.tabWidget->removeTab(i);
	countPage = 1;
}

void TextureEditor::AddRenderArea(QString texture, MeshModel *m, unsigned index)
{
	// Add a RenderArea widget to the TabWidget
	QTabBar *t = new QTabBar(ui.tabWidget);
	RenderArea *ra= new RenderArea(t, texture, m, index);
	ra->setGeometry(MARGIN,MARGIN,MAXW,MAXH);
	ui.tabWidget->addTab(t, texture);
	if (countPage == 1)
	{
		ui.tabWidget->removeTab(0);
		ra->show();
		ui.labelName->setText(texture);
	}
	ra->SetDegenerate(degenere);
	countPage++;
	QObject::connect(ra, SIGNAL(UpdateModel()),this, SLOT(UpdateModel()));
}

void TextureEditor::AddEmptyRenderArea()
{
	// Add an empty render area (the model doesn't has any texture)
	RenderArea *ra= new RenderArea(ui.tabWidget->widget(0));
	ra->setGeometry(MARGIN,MARGIN,400,400);
	ra->show();
}

void TextureEditor::SelectFromModel()
{
	// Update the Render Area according to the select faces from the model
	for (int i = 0; i < countPage-1; i++)
		((RenderArea*)ui.tabWidget->widget(i)->childAt(MARGIN,MARGIN))->ImportSelection();
	ResetLayout();
	ui.selectButton->setChecked(true);
}

void TextureEditor::UpdateModel()
{
	// Update the mesh after an edit
	area->update();
}

void TextureEditor::ResetLayout()
{
	// uncheck all the buttons
	ui.connectedButton->setChecked(false);
	ui.moveButton->setChecked(false);
	ui.selectButton->setChecked(false);
	ui.vertexButton->setChecked(false);
	ui.unifySetButton->setChecked(false);
}

void TextureEditor::SmoothTextureCoordinates()
{
	// Set up the smooth function
	for (int i = 0; i < ui.spinBoxIteration->value(); i++)
		SmoothTextureWEdgeCoords(model->cm, ui.radioButtonWhole->isChecked());
	area->update();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->RecalculateSelectionArea();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->update();
}

// Buttons
void TextureEditor::on_moveButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(VIEWMODE);
	ui.moveButton->setChecked(true);
}

void TextureEditor::on_connectedButton_clicked()
{
	ResetLayout();
	vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(model->cm);
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeSelectMode(SELECTCONNECTED);
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(SELECTMODE);
	ui.connectedButton->setChecked(true);
}

void TextureEditor::on_selectButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeSelectMode(SELECTAREA);
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(SELECTMODE);
	ui.selectButton->setChecked(true);
}

void TextureEditor::on_vertexButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeSelectMode(SELECTVERTEX);
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(SELECTMODE);
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

void TextureEditor::on_invertButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->InvertSelection();
}

void TextureEditor::on_cancelButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ClearSelection();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(VIEWMODE);
	ui.cancelButton->setChecked(false);
	ui.moveButton->setChecked(true);
}

void TextureEditor::on_flipHButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->Flip(true);
}

void TextureEditor::on_flipVButton_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->Flip(false);
}

void TextureEditor::on_unify2Button_clicked()
{
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->UnifyCouple();
}

void TextureEditor::on_unifySetButton_clicked()
{
	ResetLayout();
	((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ChangeMode(UNIFYMODE);
	ui.unifySetButton->setChecked(true);
}

void TextureEditor::on_smoothButton_clicked()
{
	// Apply the smooth
	vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(model->cm);
	SmoothTextureCoordinates();
}

void TextureEditor::on_browseButton_clicked()
{
	QString t = QFileDialog::getOpenFileName(this->parentWidget(), tr("Open Image File"), ".");
	int j = t.lastIndexOf(QChar('/'));
	QString s = t.mid(j+1, t.length() - j - 1);
	if (s.size() > 0)
	{
		int i = ui.tabWidget->currentIndex();
		unsigned int cap = model->cm.textures.capacity();
		if (model->cm.textures.capacity() <= i)
			model->cm.textures.resize(i + 1);
		model->cm.textures[i] = s.toStdString();
		ui.tabWidget->setTabText(i,s);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->setTexture(s);
		((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->update();
		ui.labelName->setText(s);
		emit updateTexture();
		area->update();
	}
}

void TextureEditor::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_H) ((RenderArea*)ui.tabWidget->currentWidget()->childAt(MARGIN,MARGIN))->ResetPosition();
	else e->ignore();
}

void TextureEditor::on_tabWidget_currentChanged(int index)
{
	int button = 0, mode = -1;
	if (ui.selectButton->isChecked()) {button = 2; mode = 0;}
	else if (ui.connectedButton->isChecked()) {button = 2; mode = 1;}
	else if (ui.vertexButton->isChecked()) {button = 2; mode = 2;}
	((RenderArea*)ui.tabWidget->widget(index)->childAt(MARGIN,MARGIN))->ChangeMode(button);
	if (mode != -1) ((RenderArea*)ui.tabWidget->widget(index)->childAt(MARGIN,MARGIN))->ChangeSelectMode(mode);
	ui.labelName->setText(((RenderArea*)ui.tabWidget->widget(index)->childAt(MARGIN,MARGIN))->GetTextureName());
}