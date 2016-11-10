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

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include <wrap/gl/pick.h>
#include <wrap/qt/gl_label.h>
#include "edit_mutualcorrs.h"
#include "edit_mutualcorrsDialog.h"
#include "solver.h"
#include "mutual.h"
#include <wrap/gl/shot.h>
#include <wrap/gl/camera.h>

#include <QFileDialog>

#define MAX_CORRPOINTS 128

using namespace std;
using namespace vcg;

EditMutualCorrsPlugin::EditMutualCorrsPlugin() {
	qFont.setFamily("Helvetica");
    qFont.setPixelSize(12);

    mutualcorrsDialog = NULL;

    //setup
    usePoint.reserve(MAX_CORRPOINTS);
    pointID.reserve(MAX_CORRPOINTS);
    modelPoints.reserve(MAX_CORRPOINTS);
    imagePoints.reserve(MAX_CORRPOINTS);
    pointError.reserve(MAX_CORRPOINTS);

    lastname = 0;

    }

const QString EditMutualCorrsPlugin::Info()
{
    return tr("Registration of images on 3D models using mutual correspondences");
}
 
void EditMutualCorrsPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
    gla->update();
    cur=event->pos();
}
  
void EditMutualCorrsPlugin::Decorate(MeshModel &m, GLArea *gla, QPainter *p)
{
	if (gla->md()->rasterList.size() == 0 || !gla->isRaster())
	{
		return;
	}
    //status
    int cindex;

    cindex = mutualcorrsDialog->ui->tableWidget->currentRow();
    if(cindex == -1)
		status_line1.sprintf("Check the Info Tab if you need more details <br> Active Point: ----");
    else
        status_line1.sprintf("Check the Info Tab if you need more details <br> Active Point: %s",pointID[cindex].toStdString().c_str());

    this->RealTimeLog("Raster Alignment", m.shortName(),
                      "%s<br>"
                      "%s<br>"
                      "%s<br>"
                      "%s",
                      status_line1.toStdString().c_str(),
                      status_line2.toStdString().c_str(),
                      status_line3.toStdString().c_str(),
                      status_error.toStdString().c_str());

    // draw picked & reference points
    if(true)
    {
        int pindex;
        Point3m currpoint;
		Point2m currim;
        QString buf;
        float lineLen = m.cm.bbox.Diag() / 50.0;

        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
        glLineWidth(2.0f);

        glDisable(GL_LIGHTING);

        for(pindex=0; pindex<usePoint.size(); pindex++)
        {
            if(pindex == cindex)            //if current
                glColor3ub(255, 255, 0);
            else if(usePoint[pindex])       //if active
                glColor3ub(150, 150, 0);
            else
                glColor3ub(75, 75, 0);

            currpoint = modelPoints[pindex];
            glBegin(GL_LINES);
                glVertex3f(currpoint[0]-lineLen, currpoint[1],         currpoint[2]);
                glVertex3f(currpoint[0]+lineLen, currpoint[1],         currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1]-lineLen, currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1]+lineLen, currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1],         currpoint[2]-lineLen);
                glVertex3f(currpoint[0],         currpoint[1],         currpoint[2]+lineLen);
            glEnd();

            buf = pointID[pindex] + " (m)";
            vcg::glLabel::render(p,currpoint,buf);       
        }

		for (pindex = 0; pindex < usePoint.size(); pindex++)
		{
			if (pindex == cindex)            //if current
				glColor3ub(255, 255, 0);
			else if (usePoint[pindex])       //if active
				glColor3ub(150, 150, 0);
			else
				glColor3ub(75, 75, 0);

			currim = imagePoints[pindex];
			Point2m onGL = fromImageToGL(currim);


			QImage &curImg = glArea->md()->rm()->currentPlane->image;
			//float imageRatio = float(curImg.width()) / float(curImg.height());
			float screenRatio = float(glArea->width()) / float(glArea->height());
			//set orthogonal view
			glPushMatrix();
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(-1.0f, 1.0f, -1, 1);

			//glColor4f(1, 1, 1, 1);
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, targetTex);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glBegin(GL_POLYGON);
			glVertex3f(onGL[0] - 0.005, onGL[1] - 0.005*screenRatio, 0.0);
			glVertex3f(onGL[0] - 0.005, onGL[1] + 0.005*screenRatio, 0.0);
			glVertex3f(onGL[0] + 0.005, onGL[1] + 0.005*screenRatio, 0.0);
			glVertex3f(onGL[0] + 0.005, onGL[1] - 0.005*screenRatio, 0.0);
			glEnd();

			/*vcg::Point3d cane(0, 0, 0);*/
			buf = pointID[pindex] + " (i)";
			//vcg::glLabel::render(p, cane, buf);
			//p->drawText(QPoint(onGL[0] + 0.02, onGL[1] + 0.02), buf);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);

			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);

			//restore view
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

					
		}

        glEnable(GL_LIGHTING);
        glPopAttrib();
    }

}

bool EditMutualCorrsPlugin::StartEdit(MeshModel & /*m*/, GLArea *gla, MLSceneGLSharedDataContext* /*cont*/)
{
    qDebug("EDIT_REFERENCING: StartEdit: setup all");

    glArea=gla;

	if(mutualcorrsDialog == NULL)
    {
        mutualcorrsDialog = new edit_mutualcorrsDialog(gla->window(), this);

		if (glArea->md()->rasterList.size() == 0 || !glArea->isRaster())
		{
			QMessageBox::warning(gla, tr("Mutual Correspondences"), tr("You need at least a raster layer, and to be in Current Raster View mode!"), QMessageBox::Ok);
			return false;
		}

        //connecting buttons
        connect(mutualcorrsDialog->ui->addLine,SIGNAL(clicked()),this,SLOT(addNewPoint()));
        connect(mutualcorrsDialog->ui->delLine,SIGNAL(clicked()),this,SLOT(deleteCurrentPoint()));
        connect(mutualcorrsDialog->ui->pickCurrent,SIGNAL(clicked()),this,SLOT(pickCurrentPoint()));
        connect(mutualcorrsDialog->ui->buttonPickRef,SIGNAL(clicked()),this,SLOT(pickCurrentRefPoint()));
        connect(mutualcorrsDialog->ui->buttonApply,SIGNAL(clicked()),this,SLOT(applyMutual()));

        connect(mutualcorrsDialog->ui->loadFromFile,SIGNAL(clicked()),this,SLOT(loadFromFile()));
        connect(mutualcorrsDialog->ui->exportToFile,SIGNAL(clicked()),this,SLOT(saveToFile()));

        //connecting other actions
    }
    mutualcorrsDialog->show();

    // signals for asking clicked point
    connect(gla,SIGNAL(transmitSurfacePos(QString,Point3m)),this,SLOT(receivedSurfacePoint(QString,Point3m)));
	connect(gla, SIGNAL(transmitPickedPos(QString, Point2m)), this, SLOT(receivedImagePoint(QString, Point2m)));
	connect(gla, SIGNAL(transmitShot(QString, Shotm)), this, SLOT(receivedShot(QString, Shotm)));
    connect(this,SIGNAL(askSurfacePos(QString)),gla,SLOT(sendSurfacePos(QString)));
	connect(this, SIGNAL(askPickedPos(QString)), gla, SLOT(sendPickedPos(QString)));
	connect(this, SIGNAL(askTrackShot(QString)), gla, SLOT(sendViewerShot(QString)));

    status_line1 = "";
    status_line2 = "";
    status_line3 = "";
    status_error = "";

    glArea->update();

    return true;
}

void EditMutualCorrsPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*gla*/, MLSceneGLSharedDataContext* /*cont*/)
{
    qDebug("EDIT_REFERENCING: EndEdit: cleaning all");
    assert(mutualcorrsDialog);
    delete(mutualcorrsDialog);
    mutualcorrsDialog = NULL;

    usePoint.clear();
    pointID.clear();
    modelPoints.clear();
    imagePoints.clear();
    pointError.clear();

	//setup
	usePoint.reserve(MAX_CORRPOINTS);
	pointID.reserve(MAX_CORRPOINTS);
	modelPoints.reserve(MAX_CORRPOINTS);
	imagePoints.reserve(MAX_CORRPOINTS);
	pointError.reserve(MAX_CORRPOINTS);

	lastname = 0;
}

void EditMutualCorrsPlugin::addNewPoint()
{
    status_error = "";
    int pindex;
    bool alreadyThere;
    QString newname;

    // i do not want to have too many refs
    if(usePoint.size() > MAX_CORRPOINTS)
    {
        status_error = "Too many points";
        return;
    }

    // I should check the name is really new... hehe :)
    do
    {
        alreadyThere = false;
        newname = "PP" + QString::number(lastname++);
        for(pindex=0; pindex<pointID.size(); pindex++)
        {
            if(pointID[pindex] == newname)
               alreadyThere=true;
        }
    }
    while(alreadyThere);

    usePoint.push_back(new bool(true));
    pointID.push_back(newname);
    modelPoints.push_back(Point3m(0.0, 0.0, 0.0));
    imagePoints.push_back(Point2m(0.0, 0.0));
    pointError.push_back(0.0);

    // update dialog
    mutualcorrsDialog->updateTable();
	mutualcorrsDialog->ui->tableWidget->selectRow(mutualcorrsDialog->ui->tableWidget->rowCount()-1);
    glArea->update();
}

void EditMutualCorrsPlugin::deleteCurrentPoint()
{
    status_error = "";
    int pindex = mutualcorrsDialog->ui->tableWidget->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
        return;
    }

    usePoint.erase(usePoint.begin() + pindex);
    pointID.erase(pointID.begin() + pindex);
    modelPoints.erase(modelPoints.begin() + pindex);
    imagePoints.erase(imagePoints.begin() + pindex);
    pointError.erase(pointError.begin() + pindex);

    // update dialog
    mutualcorrsDialog->updateTable();
	mutualcorrsDialog->ui->tableWidget->selectRow(0);
    glArea->update();
}

void EditMutualCorrsPlugin::pickCurrentPoint()
{
    status_error = "";
    int pindex = mutualcorrsDialog->ui->tableWidget->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
        return;
    }

    emit askSurfacePos("current_3D");
    status_line2 = "Double-click on model to pick point";
    glArea->update();
}

void EditMutualCorrsPlugin::pickCurrentRefPoint()
{
    status_error = "";
    int pindex = mutualcorrsDialog->ui->tableWidget->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
        return;
    }

    emit askPickedPos("current_2D");
    status_line2 = "Double-click on image to pick point";
    glArea->update();
}

void EditMutualCorrsPlugin::receivedSurfacePoint(QString name, Point3m pPoint)
{
    status_error = "";
    int pindex = mutualcorrsDialog->ui->tableWidget->currentRow();

    if(name=="current_3D")
        modelPoints[pindex] = Point3m(pPoint[0], pPoint[1], pPoint[2]);
	
    status_line2 = "";

    // update dialog
    mutualcorrsDialog->updateTable();
    glArea->update();
	mutualcorrsDialog->ui->tableWidget->selectRow(pindex);
}

void EditMutualCorrsPlugin::receivedImagePoint(QString name, Point2m pPoint)
{
	status_error = "";
	int pindex = mutualcorrsDialog->ui->tableWidget->currentRow();

	if (name == "current_2D")
	{
		Point3m iPoint = fromPickedToImage(pPoint);
		if (iPoint[0] >= 0 && iPoint[1] >= 0)
			imagePoints[pindex] = Point2m(iPoint[0], iPoint[1]);
	}

	status_line2 = "";

	// update dialog
	mutualcorrsDialog->updateTable();
	glArea->update();
	mutualcorrsDialog->ui->tableWidget->selectRow(pindex);
}

void EditMutualCorrsPlugin::receivedShot(QString name, Shotm shot)
{
	if (name == "current")
	{
		align.shot = shot;
		double ratio = (double)glArea->md()->rm()->currentPlane->image.height() / (double)align.shot.Intrinsics.ViewportPx[1];
		align.shot.Intrinsics.PixelSizeMm[0] /= ratio;
		align.shot.Intrinsics.PixelSizeMm[1] /= ratio;

		align.shot.Intrinsics.ViewportPx[0] = glArea->md()->rm()->currentPlane->image.width();
		align.shot.Intrinsics.CenterPx[0] = (int)(align.shot.Intrinsics.ViewportPx[0] / 2);
		align.shot.Intrinsics.ViewportPx[1] = glArea->md()->rm()->currentPlane->image.height();
		align.shot.Intrinsics.CenterPx[1] = (int)(align.shot.Intrinsics.ViewportPx[1] / 2);
	}

}

void EditMutualCorrsPlugin::loadFromFile()  //import reference list
{
    status_error = "";
    QString openFileName = "";
    openFileName = QFileDialog::getOpenFileName(NULL, "Import a List of Correspondences (ascii .txt)", QDir::currentPath(), "Text file (*.txt)");
    {
        // opening file
        QFile openFile(openFileName);

        if(openFile.open(QIODevice::ReadOnly))
        {
            QStringList tokenizedLine;
            QString newline;
            while (!openFile.atEnd()) {
                QString newline = QString(openFile.readLine()).simplified();

                tokenizedLine = newline.split(" ", QString::SkipEmptyParts);
                if(tokenizedLine.size()==7 )
                {
                    pointID.push_back(tokenizedLine.at(1));
					modelPoints.push_back(Point3m(tokenizedLine.at(2).toDouble(), tokenizedLine.at(3).toDouble(), tokenizedLine.at(4).toDouble()));
					imagePoints.push_back(Point2m(tokenizedLine.at(5).toInt(), tokenizedLine.at(6).toInt()));
					usePoint.push_back(new bool(true));
					pointError.push_back(0.0);
                }
				
            }

            // update dialog
            mutualcorrsDialog->updateTable();
            glArea->update();

            openFile.close();
        }
    }
}

void EditMutualCorrsPlugin::saveToFile() // export reference list + picked points + results
{
    status_error = "";
    // saving
    int pindex;

    QString openFileName = "";
    openFileName = QFileDialog::getSaveFileName(NULL, "Save Correspondences list", QDir::currentPath(), "Text file (*.txt)");

    if (openFileName != "")
    {
        // opening file
        QFile openFile(openFileName);

        if(openFile.open(QIODevice::ReadWrite))
        {
            QTextStream openFileTS(&openFile);

            openFileTS << "-------RASTER ALIGNMENT DATA---------" << "\n";
			openFileTS << "3D Model: " << glArea->md()->mm()->relativePathName() << "\n";
			openFileTS << "Raster: " << glArea->md()->rm()->currentPlane->fullPathFileName << "\n";

            // writing reference
            for(pindex=0; pindex<usePoint.size(); pindex++)
            {
                if(usePoint[pindex] == true)
                {
					openFileTS << "Corr " << pointID[pindex] << " " << modelPoints[pindex][0] << " " << modelPoints[pindex][1] << " " << modelPoints[pindex][2] << " " << imagePoints[pindex][0] << " " << imagePoints[pindex][1] << "\n";
                }
            }

            // writign results
            openFileTS << "\n";
            openFile.close();
        }
    }
}

void EditMutualCorrsPlugin::applyMutual()
{
	Solver solver;
	MutualInfo mutual;
	align.image = &glArea->md()->rm()->currentPlane->image;
	align.mesh = &glArea->md()->mm()->cm;
	int rendmode = mutualcorrsDialog->ui->renderingBox->currentIndex();
	solver.optimize_focal = mutualcorrsDialog->ui->checkFocal->isChecked();
	solver.maxiter = mutualcorrsDialog->ui->spinIter->value();
	solver.tolerance = mutualcorrsDialog->ui->spinTolerance->value();
	solver.variance = mutualcorrsDialog->ui->spinVariance->value();
	mutual.bweight = mutualcorrsDialog->ui->spinBackg->value();
	solver.mIweight = 1-mutualcorrsDialog->ui->mutcorrSlider->value() / 100.0;

	//PointCorrespondence *corr = new PointCorrespondence();

	Correspondence corr;
	
	for (int i = 0; i < imagePoints.size(); i++)
	{
		if (usePoint[i])
		{
			corr.Point3D = vcg::Point3f(modelPoints[i].X(), modelPoints[i].Y(), modelPoints[i].Z());
			corr.Point2D = vcg::Point2f(imagePoints[i].X(), imagePoints[i].Y());
			corr.error = 0.0;
			corr.index = i;
			align.correspList.push_back(corr);
		}
	}

	
	switch (rendmode)
	{
	case 0:
		align.mode = AlignSet::COMBINE;
		break;
	case 1:
		align.mode = AlignSet::NORMALMAP;
		break;
	case 2:
		align.mode = AlignSet::COLOR;
		break;
	case 3:
		align.mode = AlignSet::SPECULAR;
		break;
	case 4:
		align.mode = AlignSet::SILHOUETTE;
		break;
	case 5:
		align.mode = AlignSet::SPECAMB;
		break;
	default:
		align.mode = AlignSet::COMBINE;
		break;
	}

	///// Loading geometry

	vcg::Point3f *vertices = new vcg::Point3f[align.mesh->vn];
	vcg::Point3f *normals = new vcg::Point3f[align.mesh->vn];
	vcg::Color4b *colors = new vcg::Color4b[align.mesh->vn];
	unsigned int *indices = new unsigned int[align.mesh->fn * 3];

	for (int i = 0; i < align.mesh->vn; i++)
	{
		vertices[i].Import(align.mesh->vert[i].P());
		normals[i].Import(align.mesh->vert[i].N());
		colors[i] = align.mesh->vert[i].C();
	}

	emit askTrackShot("current");
	//align.shot = glArea->md()->rm()->shot;

	

	///// Initialize GLContext

	Log("Initialize GL");
	//glContext->makeCurrent();
	if (this->initGL() == false)
		return;

	Log("Done");

	for (int i = 0; i < align.mesh->fn; i++)
	for (int k = 0; k < 3; k++)
		indices[k + i * 3] = align.mesh->face[i].V(k) - &*align.mesh->vert.begin();

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, align.vbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, align.mesh->vn*sizeof(vcg::Point3f), vertices, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, align.nbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, align.mesh->vn*sizeof(vcg::Point3f), normals, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, align.cbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, align.mesh->vn*sizeof(vcg::Color4b), colors, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, align.ibo);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, align.mesh->fn * 3 * sizeof(unsigned int),
		indices, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	if (align.correspList.size() >= 2)
	{
		solver.levmar(&align, align.shot);

		glArea->md()->rm()->shot = Shotm::Construct(align.shot);
		float ratio = (float)glArea->md()->rm()->currentPlane->image.height() / (float)align.shot.Intrinsics.ViewportPx[1];
		glArea->md()->rm()->shot.Intrinsics.ViewportPx[0] = glArea->md()->rm()->currentPlane->image.width();
		glArea->md()->rm()->shot.Intrinsics.ViewportPx[1] = glArea->md()->rm()->currentPlane->image.height();
		glArea->md()->rm()->shot.Intrinsics.PixelSizeMm[1] /= ratio;
		glArea->md()->rm()->shot.Intrinsics.PixelSizeMm[0] /= ratio;
		glArea->md()->rm()->shot.Intrinsics.CenterPx[0] = (int)((float)glArea->md()->rm()->shot.Intrinsics.ViewportPx[0] / 2.0);
		glArea->md()->rm()->shot.Intrinsics.CenterPx[1] = (int)((float)glArea->md()->rm()->shot.Intrinsics.ViewportPx[1] / 2.0);

		QList<int> rl;
		rl << glArea->md()->rm()->id();
		glArea->md()->documentUpdated();

		if (solver.mIweight == 0.0)
		{
			for (int i = 0; i < align.correspList.size(); i++)
			{
				pointError[align.correspList[i].index] = align.correspList[i].error;
			}
			mutualcorrsDialog->updateTable();

			// it is safe to delete after copying data to VBO
			delete[]vertices;
			delete[]normals;
			delete[]colors;
			delete[]indices;
			align.correspList.clear();
			return;
		}

	}

	///// Mutual info calculation: every 30 iterations, the mail glarea is updated
	int rounds = (int)(solver.maxiter / 30);
	for (int i = 0; i<rounds; i++)
	{
		Log("Step %i of %i.", i + 1, rounds);

		solver.maxiter = 30;

		solver.optimize(&align, &mutual, align.shot);
		
		glArea->md()->rm()->shot = Shotm::Construct(align.shot);
		float ratio = (float)glArea->md()->rm()->currentPlane->image.height() / (float)align.shot.Intrinsics.ViewportPx[1];
		glArea->md()->rm()->shot.Intrinsics.ViewportPx[0] = glArea->md()->rm()->currentPlane->image.width();
		glArea->md()->rm()->shot.Intrinsics.ViewportPx[1] = glArea->md()->rm()->currentPlane->image.height();
		glArea->md()->rm()->shot.Intrinsics.PixelSizeMm[1] /= ratio;
		glArea->md()->rm()->shot.Intrinsics.PixelSizeMm[0] /= ratio;
		glArea->md()->rm()->shot.Intrinsics.CenterPx[0] = (int)((float)glArea->md()->rm()->shot.Intrinsics.ViewportPx[0] / 2.0);
		glArea->md()->rm()->shot.Intrinsics.CenterPx[1] = (int)((float)glArea->md()->rm()->shot.Intrinsics.ViewportPx[1] / 2.0);

		QList<int> rl;
		rl << glArea->md()->rm()->id();
		glArea->md()->documentUpdated();


	}
	for (int i = 0; i < align.correspList.size(); i++)
	{
		pointError[align.correspList[i].index] = align.correspList[i].error;
	}
	mutualcorrsDialog->updateTable();
	//this->glContext->doneCurrent();

	// it is safe to delete after copying data to VBO
	delete[]vertices;
	delete[]normals;
	delete[]colors;
	delete[]indices;
	align.correspList.clear();
}

Point3m EditMutualCorrsPlugin::fromPickedToImage(Point2m picked)
{
	int glWidth= glArea->size().width();
	int glHeight = glArea->size().height();
	int imWidth = glArea->md()->rm()->currentPlane[0].image.width();
	int imHeight = glArea->md()->rm()->currentPlane[0].image.height();
	double ratio = (double)imHeight / (double)glHeight;
	int wGLC = (int)(glWidth / 2.0) - picked[0];
	int imWPick = (int)(imWidth / 2.0) - (int)(wGLC*ratio);
	int imHPick = (int)(picked[1] * ratio);
	Point3m imPick(imWPick, imHPick, 0);
	return imPick;
}

Point2m EditMutualCorrsPlugin::fromImageToGL(Point2m picked)
{
	int glWidth = glArea->size().width();
	int glHeight = glArea->size().height();
	int imWidth = glArea->md()->rm()->currentPlane[0].image.width();
	int imHeight = glArea->md()->rm()->currentPlane[0].image.height();
	
	double ratio = (double)glHeight / (double)imHeight;

	double glX = (picked[0] - imWidth / 2) * ratio / (glWidth / 2.0);
	//double glX = (picked[0] - ((imWidth - glWidth/ratio)/2.0) - (glWidth / 2.0)) / (glWidth / 2.0);
	double glY = ((picked[1] * ratio) - (glHeight / 2.0)) / (glHeight / 2.0);


	Point2m imPick(glX, glY);
	return imPick;
}

bool EditMutualCorrsPlugin::initGL()
{
	GLenum err = glewInit();
	Log(0, "GL Initialization");
	if (GLEW_OK != err) {
		Log(0, "GLEW initialization error!");
		return false;
	}

	if (!glewIsSupported("GL_EXT_framebuffer_object")) {
		Log(0, "Graphics hardware does not support FBOs");
		return false;
	}
	if (!glewIsSupported("GL_ARB_vertex_shader") || !glewIsSupported("GL_ARB_fragment_shader") ||
		!glewIsSupported("GL_ARB_shader_objects") || !glewIsSupported("GL_ARB_shading_language")) {
		//QMessageBox::warning(this, "Danger, Will Robinson!",
		//                         "Graphics hardware does not fully support Shaders");
	}

	if (!glewIsSupported("GL_ARB_texture_non_power_of_two")) {
		Log(0, "Graphics hardware does not support non-power-of-two textures");
		return false;
	}
	if (!glewIsSupported("GL_ARB_vertex_buffer_object")) {
		Log(0, "Graphics hardware does not support vertex buffer objects");
		return false;
	}

	glEnable(GL_NORMALIZE);
	glDepthRange(0.0, 1.0);

	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);

	//AlignSet &align = Autoreg::instance().align;
	align.initializeGL();
	align.resize(800);
	//assert(glGetError() == 0);

	Log(0, "GL Initialization done");
	return true;
}