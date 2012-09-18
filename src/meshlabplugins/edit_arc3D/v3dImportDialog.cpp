/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
#include <Qt>
#include <QtGui>
#include <QDialog>

#include "v3dImportDialog.h"
#include "maskImageWidget.h"
using namespace vcg;

// small helper function for generating a color ramp pixmap to be used in the import dialog.

QPixmap generateColorRamp()
{
  const int width=100;
  const int height=15;
  Color4b ramp;
  QImage newImage(width,height,QImage::Format_RGB32);

  for(int x=0;x<width;++x)
     for(int y=0;y<height;++y)
        {
          ramp.SetColorRamp(0,width,x);
          newImage.setPixel(x,y,qRgb(ramp.V(0),ramp.V(1),ramp.V(2)));
        }
  
  return QPixmap::fromImage(newImage);
}

// This signal is used to make a connection between the closure of the align dialog and the end of the editing in the GLArea
void v3dImportDialog::closeEvent ( QCloseEvent * /*event*/ )
{
  emit closing();
}


v3dImportDialog::v3dImportDialog(QWidget *parent,EditArc3DPlugin *_edit )    : QDockWidget(parent)
{
	v3dImportDialog::ui.setupUi(this);
	for(int ii = 0;ii < 2;++ii)
		ui.saveShotCombo->addItem(exportShotsString(ExportShots(ii)));
	ui.saveShotCombo->setCurrentIndex(v3dImportDialog::EXPORT_ONLY_SELECTED);
	this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setFloating(true);
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+40,width(),height() );
    this->edit = _edit;

	ui.subsampleSpinBox->setValue(2);
    ui.minCountSpinBox->setValue(3);

		// connections
		connect(ui.dilationSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(ui.dilationSizeChanged(int)));
		connect(ui.erosionSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(ui.erosionSizeChanged(int)));
		connect(ui.cancelButton, SIGNAL(pressed()), this, SIGNAL(closing()));
				
    er=0;
		exportToPLY=false;

	fileName = QFileDialog::getOpenFileName(this->parentWidget(), tr("Select v3D File"), ".", "*.v3d");

	
}

QString v3dImportDialog::exportShotsString( const ExportShots exp )
{
	switch (exp)
	{
		case (EXPORT_ALL):
			return QString("Export all images");
		case (EXPORT_ONLY_SELECTED):
			return QString("Export only selected images");
	}
	return QString();
}

/* 
Main function that populate the dialog, loading all the images and eventually creating the thumbs.
called directly by the open before invoking this dialog.
*/
void v3dImportDialog::setArc3DReconstruction(Arc3DReconstruction *_er)
{
  // if the epoch reconstruction has not changed do nothing
  if(erCreated == _er->created) 
  {
    er=_er;
    return;
  }
  
  er=_er;
  erCreated=er->created;
  ui.infoLabel->setText(er->name + " - " + er->author + " - " + er->created);
  
  ui.imageTableWidget->clear();
  ui.imageTableWidget->setRowCount(er->modelList.size());
  ui.imageTableWidget->setColumnCount(4);
  //imageTableWidget->setColumnWidth (1,64);
  ui.imageTableWidget->setSelectionBehavior (QAbstractItemView::SelectRows);
  ui.imageTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui.imageTableWidget->setMinimumWidth(64*8);
  
  ui.rangeLabel->setPixmap(generateColorRamp());
  ui.rangeLabel->setMaximumHeight(10);
	ui.rangeLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	ui.rangeLabel->setScaledContents(true);
	
  ui.minCountSlider->setMaximumHeight(12);
    
  int i;
  for(i=0; i<er->modelList.size() ;++i)
  {
        
    QString ThumbImgName=Arc3DModel::ThumbName(er->modelList[i].textureName);
    QString ThumbCntName=Arc3DModel::ThumbName(er->modelList[i].countName);

    ui.imageTableWidget->setRowHeight (i,64);
    QTableWidgetItem *emptyHeaderItem = new QTableWidgetItem(i*2);
    QTableWidgetItem *texNameHeaderItem = new QTableWidgetItem(er->modelList[i].textureName,i);
    ui.imageTableWidget->setItem(i, 0, texNameHeaderItem);
    ui.imageTableWidget->setItem(i, 1, emptyHeaderItem);
    QLabel *imageLabel = new QLabel(ui.imageTableWidget);
    if(!QFile::exists(ThumbImgName))
    {
      QPixmap(er->modelList[i].textureName).scaledToHeight(64).save(ThumbImgName,"jpg");
      if(!QFile::exists(ThumbImgName)) 
        QMessageBox::warning(this,"Error in Thumb creation",
           QString("Unable to create '%1' from '%2'").arg(ThumbImgName),er->modelList[i].textureName);
    }

    imageLabel->setPixmap(QPixmap(Arc3DModel::ThumbName(er->modelList[i].textureName)));
    ui.imageTableWidget->setCellWidget(i,1,imageLabel);
    if(QFile::exists(er->modelList[i].maskName))
    {
      QTableWidgetItem *emptyHeaderItem = new QTableWidgetItem(i*4);
      ui.imageTableWidget->setItem(i, 2, emptyHeaderItem);
      QLabel *maskLabel = new QLabel(ui.imageTableWidget);
      maskLabel->setPixmap(QPixmap(er->modelList[i].maskName).scaledToHeight(64));
      ui.imageTableWidget->setCellWidget(i,2,maskLabel);     
    }
    else
    {
      QTableWidgetItem *maskHeaderItem = new QTableWidgetItem(QString("double click for\nediting the mask"),i*3);
      ui.imageTableWidget->setItem(i, 2, maskHeaderItem);
    }

    if(!QFile::exists(ThumbCntName))
      {
        CharImage chi;
        bool ret=chi.Open(er->modelList[i].countName.toAscii());
        if(!ret) QMessageBox::warning(this,"Error in Thumb creation",QString("Unable to create '%1' from '%2'").arg(ThumbCntName,er->modelList[i].textureName));

        CharImage::colorizedScaledToHeight(64,chi).save(ThumbCntName,"jpg");
        if(!QFile::exists(ThumbCntName)) 
            QMessageBox::warning(this,"Error in Thumb creation",QString("Unable to create '%1' from '%2'").arg(ThumbCntName,er->modelList[i].textureName));
      }
    QLabel *countLabel = new QLabel(ui.imageTableWidget);
    countLabel->setPixmap(QPixmap(ThumbCntName));
    QPixmap tmp(ThumbCntName);
    if(tmp.isNull())
       QMessageBox::warning(this,"Error in Thumb creation",QString("Null Pixmap '%1'").arg(ThumbCntName));
    ui.imageTableWidget->setCellWidget(i,3,countLabel);
  }  

    
show(); // necessary to make the size of the image preview correct.
ui.imageTableWidget->setItemSelected(ui.imageTableWidget->item(0,0),true);    
ui.imageTableWidget->setItemSelected(ui.imageTableWidget->item(0,1),true);    
ui.imageTableWidget->setItemSelected(ui.imageTableWidget->item(0,2),true);    


}

void v3dImportDialog::on_imageTableWidget_itemSelectionChanged()
{
  if(ui.imageTableWidget->selectedItems().size()==3)
  {
    int row= ui.imageTableWidget->row(ui.imageTableWidget->selectedItems().first());
    QPixmap tmp(er->modelList[row].textureName);
    imgSize=tmp.size();
    ui.previewLabel->setPixmap(tmp.scaled(ui.previewLabel->size(),Qt::KeepAspectRatio) );
    on_subsampleSpinBox_valueChanged(ui.subsampleSpinBox->value());
  }
}
void v3dImportDialog::on_plyButton_clicked()
{
	exportToPLY = true;
	//accept();
}
void v3dImportDialog::on_selectButton_clicked()
{
  int itemNum=ui.imageTableWidget->rowCount();
  int modVal=ui.subsampleSequenceSpinBox->value();
  if(modVal==0) return;

  for(int i=0;i<itemNum;i+=modVal)
     ui.imageTableWidget->setRangeSelected(QTableWidgetSelectionRange(i,0,i,2),true);
}

void v3dImportDialog::on_imageTableWidget_itemClicked(QTableWidgetItem * item )
{
    int row= ui.imageTableWidget->row(item);
    ui.previewLabel->setPixmap(
      QPixmap(er->modelList[row].textureName).scaled(ui.previewLabel->size(),Qt::KeepAspectRatio) );

}

void v3dImportDialog::on_imageTableWidget_itemDoubleClicked(QTableWidgetItem * item )
{
  int row= ui.imageTableWidget->row(item);
  int col= ui.imageTableWidget->column(item);
  if(col!=2) return;
  qDebug("DoubleClicked on image %s",qPrintable(er->modelList[row].textureName));
	QImage ttt(er->modelList[row].textureName);
	qDebug("'%s' %i x %i",qPrintable(er->modelList[row].textureName),ttt.width(),ttt.height());
	//ui::maskImageWidget masker(QImage(er->modelList[row].textureName));
	ui::maskImageWidget masker(ttt);
	if (QFile::exists(er->modelList[row].maskName))
		masker.loadMask(er->modelList[row].maskName);

	QImage mymask;
	if (masker.exec() == QDialog::Accepted) mymask = masker.getMask();
	if (!mymask.isNull())	
  {
    mymask.save(er->modelList[row].maskName,"png");
    QLabel *imageLabel = new QLabel(ui.imageTableWidget);
    imageLabel->setPixmap(QPixmap(er->modelList[row].maskName).scaledToHeight(64));
    ui.imageTableWidget->itemAt(row,2)->setText("");
    ui.imageTableWidget->setCellWidget(row,2,imageLabel);
  }
}



void v3dImportDialog::on_minCountSpinBox_valueChanged(int val)
{
 if(ui.minCountSlider->value()!=val) ui.minCountSlider->setValue(val);
}

void v3dImportDialog::on_minCountSlider_valueChanged(int val)
{
 if(ui.minCountSpinBox->value()!=val) ui.minCountSpinBox->setValue(val);
}

void v3dImportDialog::on_subsampleSpinBox_valueChanged(int)
{
  int ss=ui.subsampleSpinBox->value();
  if(ss==0) 
  {
    ui.subsampleSpinBox->setValue(1);
    return;
  }
  ui.imgSizeLabel->setText(QString("(%1 x %2) -> (%3 x %4)").arg(imgSize.width()).arg(imgSize.height()).arg(imgSize.width()/ss).arg(imgSize.height()/ss) );
}

void v3dImportDialog::dilationSizeChanged(int size)
{
	int winsize = size * 2 + 1;
	QString str = QString("%1 x %2").arg(winsize).arg(winsize);
	ui.lblDilationSizeValue->setText(str);
}

void v3dImportDialog::erosionSizeChanged(int size)
{
	int winsize = size * 2 + 1;
	QString str = QString("%1 x %2").arg(winsize).arg(winsize);
	ui.lblErosionSizeValue->setText(str);
}
