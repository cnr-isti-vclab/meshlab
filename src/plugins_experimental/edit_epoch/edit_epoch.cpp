/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
  History
$Log: meshedit.cpp,v $
****************************************************************************/
#include <Qt>
#include <QtGui>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>

#include "edit_epoch.h"
#include <meshlab/stdpardialog.h>
#include <vcg/complex/append.h>
#include <vcg/math/histogram.h>
#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/complex/algorithms/hole.h>
#include <wrap/io_trimesh/export_ply.h>
#include <meshlab/alnParser.h>

using namespace std;
using namespace vcg;
FILE *logFP=0; 
EpochReconstruction er;

EditEpochPlugin::EditEpochPlugin() {
  epochDialog = 0;
	//epochDialog->hide();
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10); 
	//connect(epochDialog, SLOT(reject()), this, SLOT(endEdit()) );
	
}

const QString EditEpochPlugin::Info() 
{
	return tr("Culo.");
}

void EditEpochPlugin::Decorate(MeshModel &m, GLArea * gla)
{
	
}

bool EditEpochPlugin::StartEdit(MeshDocument &_md, GLArea *_gla )
{
	this->md=&_md;
	gla=_gla;
	///////
	epochDialog=new v3dImportDialog(gla->window(),this);
	//MeshModel *m;// = md->mm();
	//MeshModel* mm=_md.addNewMesh("","dummy",true);

	
	QString fileName=epochDialog->fileName;

 	if (fileName.isEmpty()) return false;
		// initializing progress bar status
	//if (cb != NULL) (*cb)(0, "Loading...");

	// this change of dir is needed for subsequent texture/material loading
	QString FileNameDir = fileName.left(fileName.lastIndexOf("/")); 
	QDir::setCurrent(FileNameDir);

	QString errorMsgFormat = "Error encountered while loading file %1:\n%2";
	string stdfilename = QFile::encodeName(fileName).constData ();
  //string filename = fileName.toUtf8().data();

	QDomDocument doc;
	
	
  		QFile file(fileName);
			if (file.open(QIODevice::ReadOnly) && doc.setContent(&file)) 
        {
					file.close();
					QDomElement root = doc.documentElement();
					if (root.nodeName() == tr("reconstruction")) 
          {
            QDomNode nhead = root.firstChildElement("head");
            for(QDomNode n = nhead.firstChildElement("meta"); !n.isNull(); n = n.nextSiblingElement("meta"))
              {
               if(!n.hasAttributes()) return false;
               QDomNamedNodeMap attr= n.attributes();
               if(attr.contains("name")) er.name = (attr.namedItem("name")).nodeValue() ;
               if(attr.contains("author")) er.author = (attr.namedItem("author")).nodeValue() ;
               if(attr.contains("created")) er.created = (attr.namedItem("created")).nodeValue() ;
              }    
             for(QDomNode n = root.firstChildElement("model"); !n.isNull(); n = n.nextSiblingElement("model"))
              {
                EpochModel em;
                em.Init(n);
                er.modelList.push_back(em);
              }
          }
        }
    

	epochDialog->setEpochReconstruction( &er);

	//connect(epochDialog, SIGNAL(closing()),gla,SLOT(endEdit()) );

	//int continueValue = epochDialog->exec();
	//// OK till here
	
	epochDialog->exportToPLY=false;


	connect(epochDialog, SIGNAL(closing()),gla,SLOT(endEdit()) );
	connect(epochDialog->ui.plyButton, SIGNAL(clicked()),this,SLOT(ExportPly()) );
	connect(this,SIGNAL(resetTrackBall()),gla,SLOT(resetTrackBall()));

	return true;
}

void EditEpochPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/)
{
	gla->update();
	assert(epochDialog);
    delete epochDialog;
    epochDialog=0;

}  

void EditEpochPlugin::ExportPly()
{
	md->setBusy(true);
	md->addNewMesh("",er.name,true);
	MeshModel* m=md->mm();
	do
	{
			epochDialog->exportToPLY=false;
			
			//Here we invoke the modal dialog and wait for its termination
			//int continueValue = epochDialog->ui.exec();

			// The user has pressed the ok button: now start the real processing:
			
			if(epochDialog->exportToPLY == true) qDebug("Starting the ply exporting process");
			
			int t0=clock();
			logFP=fopen("epoch.log","w");

			int subSampleVal = epochDialog->ui.subsampleSpinBox->value();
			int minCountVal= epochDialog->ui.minCountSpinBox->value();
			float maxCCDiagVal= epochDialog->ui.maxCCDiagSpinBox->value();
			int mergeResolution=epochDialog->ui.mergeResolutionSpinBox->value();
			int smoothSteps=epochDialog->ui.smoothSpinBox->value();
			bool closeHole = epochDialog->ui.holeCheckBox->isChecked();
			int maxHoleSize = epochDialog->ui.holeSpinBox->value();
			/*if (continueValue == QDialog::Rejected)
			{
					QMessageBox::warning(parent, "Open V3d format","Aborted");    
					return false;
			}*/
			CMeshO mm;
			QTableWidget *qtw=epochDialog->ui.imageTableWidget;
			float MinAngleCos=cos(vcg::math::ToRad(epochDialog->ui.qualitySpinBox->value()));
			bool clustering=epochDialog->ui.fastMergeCheckBox->isChecked();
			bool removeSmallCC=epochDialog->ui.removeSmallCCCheckBox->isChecked();
			vcg::tri::Clustering<CMeshO, vcg::tri::AverageColorCell<CMeshO> > Grid;

			int selectedNum=0,selectedCount=0;
			int i;
			 for(i=0;i<qtw->rowCount();++i) if(qtw->isItemSelected(qtw->item(i,0))) ++selectedNum;
			/*if(selectedNum==0)
				{
					QMessageBox::warning(parent, "Open V3d format","No range map selected. Nothing loaded");    
					return false;
			}*/

			bool dilationFlag = epochDialog->ui.dilationCheckBox->isChecked();
			int dilationN = epochDialog->ui.dilationNumPassSpinBox->value();
			int dilationSz = epochDialog->ui.dilationSizeSlider->value() * 2 + 1;
			bool erosionFlag = epochDialog->ui.erosionCheckBox->isChecked();
			int erosionN = epochDialog->ui.erosionNumPassSpinBox->value();
			int erosionSz = epochDialog->ui.erosionSizeSlider->value() * 2 + 1;
			float scalingFactor = epochDialog->ui.scaleLineEdit->text().toFloat();
			std::vector<string> savedMeshVector;

			bool firstTime=true;
			QList<EpochModel>::iterator li;
			for(li=er.modelList.begin(), i=0;li!=er.modelList.end();++li,++i)
			{
					if(qtw->isItemSelected(qtw->item(i,0)))
					{
						++selectedCount;
						mm.Clear();
						int tt0=clock();
						(*li).BuildMesh(mm,subSampleVal,minCountVal,MinAngleCos,smoothSteps, 
							dilationFlag, dilationN, dilationSz, erosionFlag, erosionN, erosionSz,scalingFactor);
						int tt1=clock();
						if(logFP) fprintf(logFP,"** Mesh %i : Build in %i\n",selectedCount,tt1-tt0);

						if(epochDialog->exportToPLY) 
								{
									QString plyFilename =(*li).textureName.left((*li).textureName.length()-4);
									plyFilename.append(".x.ply");	
									savedMeshVector.push_back(qPrintable(plyFilename));
									int mask= tri::io::Mask::IOM_VERTCOORD + tri::io::Mask::IOM_VERTCOLOR + tri::io::Mask::IOM_VERTQUALITY;
                                    tri::io::ExporterPLY<CMeshO>::Save(mm,qPrintable(plyFilename),mask);
								}
						else 
								{
								if(clustering)
									{
										if (firstTime) 
											{
												//Grid.Init(mm.bbox,100000);
												vcg::tri::UpdateBounding<CMeshO>::Box(mm);	
												//Grid.Init(mm.bbox,1000.0*pow(10.0,mergeResolution),mm.bbox.Diag()/1000.0f);
												Grid.Init(mm.bbox,100000.0*pow(10.0,mergeResolution));
												firstTime=false;
											}
                                                                                Grid.AddMesh(mm);
									}
								else  
									tri::Append<CMeshO,CMeshO>::Mesh(m->cm,mm); // append mesh mr to ml
							}
							int tt2=clock();
							if(logFP) fprintf(logFP,"** Mesh %i : Append in %i\n",selectedCount,tt2-tt1);

					}
					//if (cb)(*cb)(selectedCount*90/selectedNum, "Building meshes");
			}
			 
			//if (cb != NULL) (*cb)(90, "Final Processing: clustering");
			if(clustering)  
			{
                                Grid.ExtractPointSet(m->cm);
			}
			
			if(epochDialog->exportToPLY) 
			{
				///OKKIO
				/*QString ALNfilename = fileName.left(fileName.length()-4).append(".aln");
				ALNParser::SaveALN(qPrintable(ALNfilename), savedMeshVector);*/
			}
			int t1=clock();
			if(logFP) fprintf(logFP,"Extracted %i meshes in %i\n",selectedCount,t1-t0);

			//if (cb != NULL) (*cb)(95, "Final Processing: Removing Small Connected Components");
			if(removeSmallCC)
			{
				m->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_FACEMARK);
				tri::Clean<CMeshO>::RemoveSmallConnectedComponentsDiameter(m->cm,m->cm.bbox.Diag()*maxCCDiagVal/100.0);
			}

			int t2=clock();
			if(logFP) fprintf(logFP,"Topology and removed CC in %i\n",t2-t1);

			vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);					// updates bounding box
			
			//if (cb != NULL) (*cb)(97, "Final Processing: Closing Holes");
			if(closeHole)
			{
				m->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_FACEMARK);
				tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m->cm);	    
				vcg::tri::Hole<CMeshO>::EarCuttingFill<vcg::tri::MinimumWeightEar< CMeshO> >(m->cm,maxHoleSize,false);
			}

			//if (cb != NULL) (*cb)(100, "Done");
		//  vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);		// updates normals
			 
				m->updateDataMask(MeshModel::MM_VERTCOLOR);

			int t3=clock();
			if(logFP) fprintf(logFP,"---------- Total Processing Time%i\n\n\n",t3-t0);
			if(logFP) fclose(logFP);
			logFP=0;

			vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);					// updates bounding box
			tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m->cm);
	        

	//// Importing rasters

            //QList<EpochModel>::iterator li;
			for(int i=0; i<er.modelList.size(); ++i)
			{
				RasterModel* rm=md->addNewRaster();
				rm->addPlane(new Plane(md->rm(),er.modelList[i].textureName,QString("RGB")));
				rm->setLabel(er.modelList[i].textureName);
				rm->shot=er.modelList[i].shot;
			}


	} while(epochDialog->exportToPLY);
	md->mm()->visible=true;
	md->setBusy(false);
	emit this->resetTrackBall();
	gla->update();

	
}    

void EditEpochPlugin::mousePressEvent(QMouseEvent *e, MeshModel &, GLArea * )
{
	
}

void EditEpochPlugin::mouseMoveEvent(QMouseEvent *e, MeshModel &, GLArea * ) 
{
	
	
}

void EditEpochPlugin::mouseReleaseEvent(QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
{
	
}


// this function toggles on and off all the buttons (according to the "modal" states of the interface),
// do not confuse it with the updatebuttons function of the epochDialog class.
void EditEpochPlugin::toggleButtons()
{

}

