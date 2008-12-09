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
          ramp.ColorRamp(0,width,x);          
          newImage.setPixel(x,y,qRgb(ramp.V(0),ramp.V(1),ramp.V(2)));
        }
  
  return QPixmap::fromImage(newImage);
}
/* 
Main function that populate the dialog, loading all the images and eventually creating the thumbs.
called directly by the open before invoking this dialog.
*/
void v3dImportDialog::setEpochReconstruction(EpochReconstruction *_er,CallBackPos *cb)
{
  // if the epoch reconstruction has not changed do nothing
  if(erCreated == _er->created) 
  {
    er=_er;
    return;
  }
  
  er=_er;
  erCreated=er->created;
  infoLabel->setText(er->name + " - " + er->author + " - " + er->created);
  
  imageTableWidget->clear();
  imageTableWidget->setRowCount(er->modelList.size());
  imageTableWidget->setColumnCount(4);
  //imageTableWidget->setColumnWidth (1,64);
  imageTableWidget->setSelectionBehavior (QAbstractItemView::SelectRows);
  imageTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  imageTableWidget->setMinimumWidth(64*8);
  
  rangeLabel->setPixmap(generateColorRamp());
  rangeLabel->setMaximumHeight(10);
	rangeLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	rangeLabel->setScaledContents(true);
	
  minCountSlider->setMaximumHeight(12);
    
  int i;
  for(i=0; i<er->modelList.size() ;++i)
  {
    cb(i*100/er->modelList.size(),"Reading images");   
    
    QString ThumbImgName=EpochModel::ThumbName(er->modelList[i].textureName);
    QString ThumbCntName=EpochModel::ThumbName(er->modelList[i].countName);

    imageTableWidget->setRowHeight (i,64);
    QTableWidgetItem *emptyHeaderItem = new QTableWidgetItem(i*2);
    QTableWidgetItem *texNameHeaderItem = new QTableWidgetItem(er->modelList[i].textureName,i);
    imageTableWidget->setItem(i, 0, texNameHeaderItem);
    imageTableWidget->setItem(i, 1, emptyHeaderItem);
    QLabel *imageLabel = new QLabel(imageTableWidget);
    if(!QFile::exists(ThumbImgName))
    {
      QPixmap(er->modelList[i].textureName).scaledToHeight(64).save(ThumbImgName,"jpg");
      if(!QFile::exists(ThumbImgName)) 
        QMessageBox::warning(this,"Error in Thumb creation",
           QString("Unable to create '%1' from '%2'").arg(ThumbImgName),er->modelList[i].textureName);
    }

    imageLabel->setPixmap(QPixmap(EpochModel::ThumbName(er->modelList[i].textureName)));
    imageTableWidget->setCellWidget(i,1,imageLabel);
    if(QFile::exists(er->modelList[i].maskName))
    {
      QTableWidgetItem *emptyHeaderItem = new QTableWidgetItem(i*4);
      imageTableWidget->setItem(i, 2, emptyHeaderItem);
      QLabel *maskLabel = new QLabel(imageTableWidget);
      maskLabel->setPixmap(QPixmap(er->modelList[i].maskName).scaledToHeight(64));
      imageTableWidget->setCellWidget(i,2,maskLabel);     
    }
    else
    {
      QTableWidgetItem *maskHeaderItem = new QTableWidgetItem(QString("double click for\nediting the mask"),i*3);
      imageTableWidget->setItem(i, 2, maskHeaderItem);
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
    QLabel *countLabel = new QLabel(imageTableWidget);
    countLabel->setPixmap(QPixmap(ThumbCntName));
    QPixmap tmp(ThumbCntName);
    if(tmp.isNull())
       QMessageBox::warning(this,"Error in Thumb creation",QString("Null Pixmap '%1'").arg(ThumbCntName));
    imageTableWidget->setCellWidget(i,3,countLabel);
  }  

cb(100,"Completed Image Reading.");   
    
show(); // necessary to make the size of the image preview correct.
imageTableWidget->setItemSelected(imageTableWidget->item(0,0),true);    
imageTableWidget->setItemSelected(imageTableWidget->item(0,1),true);    
imageTableWidget->setItemSelected(imageTableWidget->item(0,2),true);    
//on_imageTableWidget_itemSelectionChanged();

}

void v3dImportDialog::on_imageTableWidget_itemSelectionChanged()
{
  if(imageTableWidget->selectedItems().size()==3)
  {
    int row= imageTableWidget->row(imageTableWidget->selectedItems().first());
    QPixmap tmp(er->modelList[row].textureName);
    imgSize=tmp.size();
    previewLabel->setPixmap(tmp.scaled(previewLabel->size(),Qt::KeepAspectRatio) );
    on_subsampleSpinBox_valueChanged(subsampleSpinBox->value());
  }
}
void v3dImportDialog::on_plyButton_clicked()
{
	exportToPLY = true;
	accept();
}
void v3dImportDialog::on_selectButton_clicked()
{
  int itemNum=imageTableWidget->rowCount();
  int modVal=subsampleSequenceSpinBox->value();
  if(modVal==0) return;

  for(int i=0;i<itemNum;i+=modVal)
     imageTableWidget->setRangeSelected(QTableWidgetSelectionRange(i,0,i,2),true);
}

void v3dImportDialog::on_imageTableWidget_itemClicked(QTableWidgetItem * item )
{
    int row= imageTableWidget->row(item);
    previewLabel->setPixmap(
      QPixmap(er->modelList[row].textureName).scaled(previewLabel->size(),Qt::KeepAspectRatio) );

}

void v3dImportDialog::on_imageTableWidget_itemDoubleClicked(QTableWidgetItem * item )
{
  int row= imageTableWidget->row(item);
  int col= imageTableWidget->column(item);
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
    QLabel *imageLabel = new QLabel(imageTableWidget);
    imageLabel->setPixmap(QPixmap(er->modelList[row].maskName).scaledToHeight(64));
    imageTableWidget->itemAt(row,2)->setText("");
    imageTableWidget->setCellWidget(row,2,imageLabel);
  }
}



void v3dImportDialog::on_minCountSpinBox_valueChanged(int val)
{
 if(minCountSlider->value()!=val) minCountSlider->setValue(val);
}

void v3dImportDialog::on_minCountSlider_valueChanged(int val)
{
 if(minCountSpinBox->value()!=val) minCountSpinBox->setValue(val);
}

void v3dImportDialog::on_subsampleSpinBox_valueChanged(int)
{
  int ss=subsampleSpinBox->value();
  if(ss==0) 
  {
    subsampleSpinBox->setValue(1);
    return;
  }
  imgSizeLabel->setText(QString("(%1 x %2) -> (%3 x %4)").arg(imgSize.width()).arg(imgSize.height()).arg(imgSize.width()/ss).arg(imgSize.height()/ss) );
}

void v3dImportDialog::on_mergeResolutionSpinBox_valueChanged(int)
{
	fastMergeCheckBox->setChecked(true); // if someone touch the slider check the fast merging box
}

void v3dImportDialog::dilationSizeChanged(int size)
{
	int winsize = size * 2 + 1;
	QString str = QString("%1 x %2").arg(winsize).arg(winsize);
	lblDilationSizeValue->setText(str);
}

void v3dImportDialog::erosionSizeChanged(int size)
{
	int winsize = size * 2 + 1;
	QString str = QString("%1 x %2").arg(winsize).arg(winsize);
	lblErosionSizeValue->setText(str);
}
