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
#include "edit_referencing.h"
#include "edit_referencingDialog.h"

#include <QFileDialog>

#define MAX_REFPOINTS 128

using namespace std;
using namespace vcg;

EditReferencingPlugin::EditReferencingPlugin() {
	qFont.setFamily("Helvetica");
    qFont.setPixelSize(12);

    referencingDialog = NULL;

    //referencing mode
    referencingMode = EditReferencingPlugin::REF_ABSOLUTE;

    //setup
    usePoint.reserve(MAX_REFPOINTS);
    pointID.reserve(MAX_REFPOINTS);
    pickedPoints.reserve(MAX_REFPOINTS);
    refPoints.reserve(MAX_REFPOINTS);
    pointError.reserve(MAX_REFPOINTS);

    useDistance.reserve(MAX_REFPOINTS);
    distanceID.reserve(MAX_REFPOINTS);
    distPointA.reserve(MAX_REFPOINTS);
    distPointB.reserve(MAX_REFPOINTS);
    currDist.reserve(MAX_REFPOINTS);
    targDist.reserve(MAX_REFPOINTS);
	scaleFact.reserve(MAX_REFPOINTS);
    distError.reserve(MAX_REFPOINTS);
	globalScale = 0.0;

    transfMatrix.SetIdentity();

    lastname = 0;

    lastAskedPick = EditReferencingPlugin::REF_ABSOLUTE;

    referencingResults.clear();
    referencingResults.reserve(4096);
}

const QString EditReferencingPlugin::Info()
{
    return tr("Reference layer(s) using fiducial points or scale layer(s) using reference distances.");
}
 
void EditReferencingPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
    gla->update();
    cur=event->pos();
}

void EditReferencingPlugin::Decorate(MeshModel &m, GLArea *gla, QPainter *p)
{
	if (referencingMode == EditReferencingPlugin::REF_ABSOLUTE)
		DecorateAbsolute(m, gla, p);
	else if (referencingMode == EditReferencingPlugin::REF_SCALE)
		DecorateScale(m, gla, p);
}

void EditReferencingPlugin::DecorateAbsolute(MeshModel &m, GLArea * /*gla*/, QPainter *p)
{
    //status
    int cindex;

    cindex = referencingDialog->ui->tableWidget->currentRow();
    if(cindex == -1)
        status_line1.sprintf("Active Point: ----");
    else
        status_line1.sprintf("Active Point: %s",pointID[cindex].toStdString().c_str());

    this->RealTimeLog("Edit Referencing", m.shortName(),
                      "Absolute Referencing<br>"
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
        vcg::Point3d currpoint;
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

            currpoint = pickedPoints[pindex];
            glBegin(GL_LINES);
                glVertex3f(currpoint[0]-lineLen, currpoint[1],         currpoint[2]);
                glVertex3f(currpoint[0]+lineLen, currpoint[1],         currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1]-lineLen, currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1]+lineLen, currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1],         currpoint[2]-lineLen);
                glVertex3f(currpoint[0],         currpoint[1],         currpoint[2]+lineLen);
            glEnd();

            buf = pointID[pindex] + " (moving)";
            vcg::glLabel::render(p,currpoint,buf);


            if(pindex == cindex)            //if current
                glColor3ub(0, 255, 255);
            else if(usePoint[pindex])       //if active
                glColor3ub(0, 150, 150);
            else
                glColor3ub(0, 75, 75);

            currpoint = refPoints[pindex];
            glBegin(GL_LINES);
                glVertex3f(currpoint[0]-lineLen, currpoint[1],         currpoint[2]);
                glVertex3f(currpoint[0]+lineLen, currpoint[1],         currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1]-lineLen, currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1]+lineLen, currpoint[2]);
                glVertex3f(currpoint[0],         currpoint[1],         currpoint[2]-lineLen);
                glVertex3f(currpoint[0],         currpoint[1],         currpoint[2]+lineLen);
            glEnd();

            buf = pointID[pindex] + " (reference)";
            vcg::glLabel::render(p,currpoint,buf);

            if(validMatrix)
            {
                if(usePoint[pindex])    //if active
                {
                    if(pindex == cindex)            //if current
                        glColor3ub(0, 255, 0);
                    else
                        glColor3ub(75, 150, 75);

                    currpoint = transfMatrix * pickedPoints[pindex];
                    glBegin(GL_LINES);
                        glVertex3f(currpoint[0]-lineLen, currpoint[1],         currpoint[2]);
                        glVertex3f(currpoint[0]+lineLen, currpoint[1],         currpoint[2]);
                        glVertex3f(currpoint[0],         currpoint[1]-lineLen, currpoint[2]);
                        glVertex3f(currpoint[0],         currpoint[1]+lineLen, currpoint[2]);
                        glVertex3f(currpoint[0],         currpoint[1],         currpoint[2]-lineLen);
                        glVertex3f(currpoint[0],         currpoint[1],         currpoint[2]+lineLen);
                    glEnd();

                    buf = pointID[pindex] + " (transformed)";
                    vcg::glLabel::render(p,currpoint,buf);
                }
            }
        }

        glEnable(GL_LIGHTING);
        glPopAttrib();
    }

}

void EditReferencingPlugin::DecorateScale(MeshModel &m, GLArea * /*gla*/, QPainter *p)
{
	//status
	int cindex;

	cindex = referencingDialog->ui->tableWidgetDist->currentRow();
	if (cindex == -1)
	{
		status_line1.sprintf("Active Distance: ----");
	}
	else
	{
		status_line1.sprintf("Active Distance: %s", distanceID[cindex].toStdString().c_str());

		if ((currDist[cindex] != 0.0) && (targDist[cindex] != 0.0))
			status_line2.sprintf("%.3f  -->  %.3f", currDist[cindex], targDist[cindex]);
		else
			status_line2.sprintf(" ");
	}

	if (globalScale!=0)
		status_line3.sprintf("SCENE SCALE: %.3f", globalScale);
	else
		status_line3.sprintf("NO VALID SCENE SCALE");

	this->RealTimeLog("Edit Referencing", m.shortName(),
		"Scale Referencing<br>"
		"%s<br>"
		"%s<br><br>"
		"%s<br>"
		"%s",
		status_line1.toStdString().c_str(),
		status_line2.toStdString().c_str(),
		status_line3.toStdString().c_str(),
		status_error.toStdString().c_str());

	// draw picked distances
	if (true)
	{
		int pindex;
		vcg::Point3d currPointA, currPointB;
		QString buf;
		float lineLen = m.cm.bbox.Diag() / 50.0;

		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
		glLineWidth(2.0f);

		glDisable(GL_LIGHTING);

		for (pindex = 0; pindex<useDistance.size(); pindex++)
		{
			if (pindex == cindex)            //if current
				glColor3ub(200, 200, 0);
			else if (useDistance[pindex])    //if active
				glColor3ub(100, 100, 0);
			else
				glColor3ub(70, 70, 0);

			currPointA = distPointA[pindex];
			currPointB = distPointB[pindex];

			glBegin(GL_LINES);
			glVertex3f(currPointA[0] - lineLen, currPointA[1], currPointA[2]);
			glVertex3f(currPointA[0] + lineLen, currPointA[1], currPointA[2]);
			glVertex3f(currPointA[0], currPointA[1] - lineLen, currPointA[2]);
			glVertex3f(currPointA[0], currPointA[1] + lineLen, currPointA[2]);
			glVertex3f(currPointA[0], currPointA[1], currPointA[2] - lineLen);
			glVertex3f(currPointA[0], currPointA[1], currPointA[2] + lineLen);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(currPointB[0] - lineLen, currPointB[1], currPointB[2]);
			glVertex3f(currPointB[0] + lineLen, currPointB[1], currPointB[2]);
			glVertex3f(currPointB[0], currPointB[1] - lineLen, currPointB[2]);
			glVertex3f(currPointB[0], currPointB[1] + lineLen, currPointB[2]);
			glVertex3f(currPointB[0], currPointB[1], currPointB[2] - lineLen);
			glVertex3f(currPointB[0], currPointB[1], currPointB[2] + lineLen);
			glEnd();

			buf = distanceID[pindex] + " (A)";
			vcg::glLabel::render(p, currPointA, buf);
			buf = distanceID[pindex] + " (B)";
			vcg::glLabel::render(p, currPointB, buf);

			if (pindex == cindex)            //if current
				glColor3ub(255, 255, 0);
			else if (useDistance[pindex])    //if active
				glColor3ub(155, 155, 0);
			else
				glColor3ub(75, 75, 0);

			glBegin(GL_LINES);
			glVertex3f(currPointA[0], currPointA[1], currPointA[2]);
			glVertex3f(currPointB[0], currPointB[1], currPointB[2]);
			glEnd();
		}

		glEnable(GL_LIGHTING);
		glPopAttrib();
	}
}

bool EditReferencingPlugin::StartEdit(MeshModel & m, GLArea * gla, MLSceneGLSharedDataContext* /*cont*/)
{
    qDebug("EDIT_REFERENCING: StartEdit: setup all");

    glArea=gla;

    if(referencingDialog == NULL)
    {
        referencingDialog = new edit_referencingDialog(gla->window(), this);

        //connecting buttons
        connect(referencingDialog->ui->addLine,SIGNAL(clicked()),this,SLOT(addNewPoint()));
        connect(referencingDialog->ui->delLine,SIGNAL(clicked()),this,SLOT(deleteCurrentPoint()));
        connect(referencingDialog->ui->pickCurrent,SIGNAL(clicked()),this,SLOT(pickCurrentPoint()));
        connect(referencingDialog->ui->buttonPickRef,SIGNAL(clicked()),this,SLOT(pickCurrentRefPoint()));
        connect(referencingDialog->ui->buttonCalculate,SIGNAL(clicked()),this,SLOT(calculateMatrix()));
        connect(referencingDialog->ui->buttonApply,SIGNAL(clicked()),this,SLOT(applyMatrix()));
        connect(referencingDialog->ui->loadFromFile,SIGNAL(clicked()),this,SLOT(loadFromFile()));
        connect(referencingDialog->ui->exportToFile,SIGNAL(clicked()),this,SLOT(saveToFile()));

        connect(referencingDialog->ui->addDistance,SIGNAL(clicked()),this,SLOT(addNewDistance()));
        connect(referencingDialog->ui->delDistance,SIGNAL(clicked()),this,SLOT(deleteCurrentDistance()));
        connect(referencingDialog->ui->bttPickPointA,SIGNAL(clicked()),this,SLOT(pickCurrDistPA()));
        connect(referencingDialog->ui->bttPickPointB,SIGNAL(clicked()),this,SLOT(pickCurrDistPB()));
		connect(referencingDialog->ui->bttApplyScale, SIGNAL(clicked()), this, SLOT(applyScale()));
		connect(referencingDialog->ui->bttLoadDistances, SIGNAL(clicked()), this, SLOT(loadDistances()));
		connect(referencingDialog->ui->bttExportScaling, SIGNAL(clicked()), this, SLOT(exportScaling()));

        //connecting other actions
    }
    referencingDialog->show();

    // signals for asking clicked point
    connect(gla,SIGNAL(transmitSurfacePos(QString,Point3m)),this,SLOT(receivedSurfacePoint(QString,Point3m)));
    connect(this,SIGNAL(askSurfacePos(QString)),gla,SLOT(sendSurfacePos(QString)));

    status_line1 = "";
    status_line2 = "";
    status_line3 = "";
    status_error = "";

	// reading current transformations for all layers
	layersOriginalTransf.resize(glArea->md()->meshList.size());
	int lind = 0;
	foreach(MeshModel *mmp, glArea->md()->meshList)
	{
		layersOriginalTransf[lind].Import(mmp->cm.Tr);
		lind++;
	}
	originalTransf.Import(m.cm.Tr);

    glArea->update();

    return true;
}

void EditReferencingPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/)
{
    qDebug("EDIT_REFERENCING: EndEdit: cleaning all");
    delete(referencingDialog);
    referencingDialog = NULL;

    usePoint.clear();
    pointID.clear();
    pickedPoints.clear();
    refPoints.clear();
    pointError.clear();

    useDistance.clear();
    distanceID.clear();
    distPointA.clear();
    distPointB.clear();
    currDist.clear();
    targDist.clear();
	scaleFact.clear();
    distError.clear();
}

void EditReferencingPlugin::receivedSurfacePoint(QString name,Point3m pPoint)
{
    status_error = "";

    int pindex;
    if(lastAskedPick == EditReferencingPlugin::REF_ABSOLUTE)
        pindex = referencingDialog->ui->tableWidget->currentRow();
    else if(lastAskedPick == EditReferencingPlugin::REF_SCALE)
        pindex = referencingDialog->ui->tableWidgetDist->currentRow();

    if(name=="currentMOV")
        pickedPoints[pindex] = Point3d(pPoint[0], pPoint[1], pPoint[2]);
    else if(name=="currentREF")
        refPoints[pindex] = Point3d(pPoint[0], pPoint[1], pPoint[2]);
	else if (name == "currentPA")
	{
		distPointA[pindex] = Point3d(pPoint[0], pPoint[1], pPoint[2]);
		updateDistances();
	}
	else if (name == "currentPB")
	{
		distPointB[pindex] = Point3d(pPoint[0], pPoint[1], pPoint[2]);
		updateDistances();
	}

    status_line2 = "";

    // update dialog
    referencingDialog->updateTable();
    referencingDialog->updateTableDist();
    glArea->update();
}

void EditReferencingPlugin::addNewPoint()
{
    status_error = "";
    int pindex;
    bool alreadyThere;
    QString newname;

    // i do not want to have too many refs
    if(usePoint.size() > MAX_REFPOINTS)
    {
        status_error = "Too many points";
		referencingDialog->updateTable();
		glArea->update();
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
    pickedPoints.push_back(Point3d(0.0, 0.0, 0.0));
    refPoints.push_back(Point3d(0.0, 0.0, 0.0));
    pointError.push_back(-1.0);

    // update dialog
    referencingDialog->updateTable();
    glArea->update();
}

void EditReferencingPlugin::deleteCurrentPoint()
{
    status_error = "";
    int pindex = referencingDialog->ui->tableWidget->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
		glArea->update();
        return;
    }

    usePoint.erase(usePoint.begin() + pindex);
    pointID.erase(pointID.begin() + pindex);
    pickedPoints.erase(pickedPoints.begin() + pindex);
    refPoints.erase(refPoints.begin() + pindex);
    pointError.erase(pointError.begin() + pindex);

    // update dialog
    referencingDialog->updateTable();
    glArea->update();
}

void EditReferencingPlugin::pickCurrentPoint()
{
    status_error = "";
    int pindex = referencingDialog->ui->tableWidget->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
		glArea->update();
        return;
    }

    lastAskedPick = EditReferencingPlugin::REF_ABSOLUTE;
    emit askSurfacePos("currentMOV");
    status_line2 = "Double-click on model to pick point";
    glArea->update();
}

void EditReferencingPlugin::pickCurrentRefPoint()
{
    status_error = "";
    int pindex = referencingDialog->ui->tableWidget->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
		glArea->update();
        return;
    }

    lastAskedPick = EditReferencingPlugin::REF_ABSOLUTE;
    emit askSurfacePos("currentREF");
    status_line2 = "Double-click on model to pick point";
    glArea->update();
}

void EditReferencingPlugin::loadFromFile()  //import reference list
{
    status_error = "";
    QString openFileName = "";
    openFileName = QFileDialog::getOpenFileName(NULL, "Import a List of Reference Points (ascii .txt)", QDir::currentPath(), "Text file (*.txt)");
    {
        // opening file
        QFile openFile(openFileName);

        if(openFile.open(QIODevice::ReadOnly))
        {
            QStringList tokenizedLine;
            QString firstline, secondline;
            double newX = 0.0, newY = 0.0, newZ = 0.0;
            bool parseXOK=true, parseYOK=true, parseZOK=true;
			bool found = false;
			QString separator = "";

			// clearing data
			usePoint.clear();
			pointID.clear();
			pickedPoints.clear();
			refPoints.clear();
			pointError.clear();

            //getting first line, to see if there is an header
            if(!openFile.atEnd())
                firstline = QString(openFile.readLine()).simplified();
            else
			{
				status_error = "point list format not recognized";
				referencingDialog->updateTable();
				glArea->update();
                return;
            }

			// discovering the separator 
			vector <QString> possible_separators = { QString(" "), QString(","), QString(";") };

			int sind = 0;
			while ((!found) || (sind<possible_separators.size()))
			{
				tokenizedLine = firstline.split(possible_separators[sind], QString::SkipEmptyParts);
				if (tokenizedLine.size() == 4 || (tokenizedLine.size() == 5 && firstline.right(1) == possible_separators[sind]))
				{
					newX = tokenizedLine.at(1).toDouble(&parseXOK);
					newY = tokenizedLine.at(2).toDouble(&parseYOK);
					newZ = tokenizedLine.at(3).toDouble(&parseZOK);
					if (parseXOK && parseYOK && parseZOK)
					{
						separator = possible_separators[sind]; found = true;
					}
				}
				sind++;
			}

			if (!found)
			{
				status_error = "point list format not recognized";
				openFile.close();
				referencingDialog->updateTableDist();
				glArea->update();
				return;
			}

			// inserting first line
			tokenizedLine = firstline.split(separator, QString::SkipEmptyParts);
			newX = tokenizedLine.at(1).toDouble(&parseXOK);
			newY = tokenizedLine.at(2).toDouble(&parseYOK);
			newZ = tokenizedLine.at(3).toDouble(&parseZOK);
			{
				usePoint.push_back(new bool(true));
				pointID.push_back(tokenizedLine.at(0));
				pickedPoints.push_back(Point3d(0.0, 0.0, 0.0));
				refPoints.push_back(Point3d(newX, newY, newZ));
				pointError.push_back(-1.0);
			}

			// and now, the rest of the file
			while (!openFile.atEnd()) {
				QString newline = QString(openFile.readLine()).simplified();

				tokenizedLine = newline.split(separator, QString::SkipEmptyParts);
				if (tokenizedLine.size() == 4 || (tokenizedLine.size() == 5 && newline.right(1) == separator))
				{
					newX = tokenizedLine.at(1).toDouble(&parseXOK);
					newY = tokenizedLine.at(2).toDouble(&parseYOK);
					newZ = tokenizedLine.at(3).toDouble(&parseZOK);
					if (parseXOK && parseYOK && parseZOK)   //inserting
					{
						usePoint.push_back(new bool(true));
						pointID.push_back(tokenizedLine.at(0));
						pickedPoints.push_back(Point3d(0.0, 0.0, 0.0));
						refPoints.push_back(Point3d(newX, newY, newZ));
						pointError.push_back(-1.0);
					}
				}
			}

            // update dialog
            referencingDialog->updateTable();
            glArea->update();

            openFile.close();
        }
		else
		{
			status_error = "cannot open file";
			referencingDialog->updateTable();
			glArea->update();
		}
    }
}

void EditReferencingPlugin::saveToFile() // export reference list + picked points + results
{
    status_error = "";
    // saving
    int pindex;

    QString openFileName = "";
    openFileName = QFileDialog::getSaveFileName(NULL, "Save Referencing Process", QDir::currentPath(), "Text file (*.txt)");

    if (openFileName != "")
    {
        // opening file
        QFile openFile(openFileName);

        if(openFile.open(QIODevice::ReadWrite))
        {
            QTextStream openFileTS(&openFile);

            openFileTS << "-------REFERENCING DATA---------" << "\n\n\n";

            // writing reference
            openFileTS << "Reference points:" << "\n";
            for(pindex=0; pindex<usePoint.size(); pindex++)
            {
                if(usePoint[pindex] == true)
                {
                    openFileTS << pointID[pindex] << "; " << refPoints[pindex][0] << "; " << refPoints[pindex][1] << "; " << refPoints[pindex][2] << "\n";
                }
            }

            openFileTS << "\n";

            // writing picked
            openFileTS << "Picked points:" << "\n";
            for(pindex=0; pindex<usePoint.size(); pindex++)
            {
                if(usePoint[pindex] == true)
                {
                    openFileTS << pointID[pindex] << "; " << pickedPoints[pindex][0] << "; " << pickedPoints[pindex][1] << "; " << pickedPoints[pindex][2] << "\n";
                }
            }

            // writign results
            openFileTS << "\n";
            openFileTS << referencingResults;

            openFile.close();
        }
		else
		{
			status_error = "cannot save file";
			glArea->update();
		}
    }
}

void EditReferencingPlugin::calculateMatrix()
{
    status_error = "";
    vector<vcg::Point3d> FixP;
    vector<vcg::Point3d> MovP;

    int pindex = 0;
    float TrError=0;

    // constructing a vector of only ACTIVE points, plus indices and names, just for convenience
    // matrix calculation function uses all points in the vector, while in the filter we keep
    // a larger lint, in order to turn on and off points when we need, which is more flexible

    FixP.clear();
    FixP.reserve(MAX_REFPOINTS);
    MovP.clear();
    MovP.reserve(MAX_REFPOINTS);

    referencingDialog->ui->buttonApply->setEnabled(false);
    validMatrix=false;
    isMatrixRigid=true;
    status_line3 = "NO MATRIX";

    //filling
    for(pindex=0; pindex<usePoint.size(); pindex++)
    {
        if(usePoint[pindex] == true)
        {
            MovP.push_back(pickedPoints[pindex]);
            FixP.push_back(refPoints[pindex]);
        }

        // while iterating, set all errors to zero
        pointError[pindex]=0.0;
    }

    // if less than 4 points, error
    if(MovP.size() < 4)
    {
        // tell the user the problem
        status_error = "There have to be at least 4 points";
		referencingDialog->updateTable();
		glArea->update();

        // cleaning up
        FixP.clear();
        MovP.clear();

        return;
    }

    // formatting results for saving
    referencingResults.clear();
    referencingResults.reserve(4096);
    referencingResults.append("----Referencing Results----\n\n");

    if(referencingDialog->ui->cbAllowScaling->checkState() == Qt::Checked)
    {
        this->Log(GLLogStream::FILTER, "calculating NON RIGID transformation using %d reference points:", FixP.size());
        referencingResults.append(QString("NON RIGID transformation from %1 reference points:\n").arg(QString::number(FixP.size())));
        ComputeSimilarityMatchMatrix(FixP, MovP, transfMatrix);
        validMatrix=true;
        isMatrixRigid=false;
    }
    else
    {
        this->Log(GLLogStream::FILTER, "calculating RIGID transformation using %d reference points:", FixP.size());
        referencingResults.append(QString("RIGID transformation from %1 reference points:\n").arg(QString::number(FixP.size())));
        ComputeRigidMatchMatrix(FixP, MovP, transfMatrix);
        validMatrix=true;
        isMatrixRigid=true;
    }

    referencingDialog->ui->buttonApply->setEnabled(true);

    status_line3.sprintf("MATRIX:<br>"
                         "%.2f %.2f %.2f %.3f<br>"
                         "%.2f %.2f %.2f %.3f<br>"
                         "%.2f %.2f %.2f %.3f<br>"
                         "%.2f %.2f %.2f %.3f<br>",
                         transfMatrix[0][0],transfMatrix[0][1],transfMatrix[0][2],transfMatrix[0][3],
                         transfMatrix[1][0],transfMatrix[1][1],transfMatrix[1][2],transfMatrix[1][3],
                         transfMatrix[2][0],transfMatrix[2][1],transfMatrix[2][2],transfMatrix[2][3],
                         transfMatrix[3][0],transfMatrix[3][1],transfMatrix[3][2],transfMatrix[3][3]);

    referencingResults.append("\n");
    referencingResults.append(status_line3);
    referencingResults.replace("<br>","\n");


    if(isMatrixRigid)
        this->Log(GLLogStream::FILTER, "RIGID MATRIX:");
    else
        this->Log(GLLogStream::FILTER, "NON-RIGID MATRIX:");
    this->Log(GLLogStream::FILTER, "%f %f %f %f",transfMatrix[0][0],transfMatrix[0][1],transfMatrix[0][2],transfMatrix[0][3]);
    this->Log(GLLogStream::FILTER, "%f %f %f %f",transfMatrix[1][0],transfMatrix[1][1],transfMatrix[1][2],transfMatrix[1][3]);
    this->Log(GLLogStream::FILTER, "%f %f %f %f",transfMatrix[2][0],transfMatrix[2][1],transfMatrix[2][2],transfMatrix[2][3]);
    this->Log(GLLogStream::FILTER, "%f %f %f %f",transfMatrix[3][0],transfMatrix[3][1],transfMatrix[3][2],transfMatrix[3][3]);

    this->Log(GLLogStream::FILTER, "  ");
    this->Log(GLLogStream::FILTER, "Residual Errors:");

    referencingResults.append("\n\nResidual Errors:\n\n");

	for (pindex = 0; pindex<usePoint.size(); pindex++)
    {
		if (usePoint[pindex])
		{
			pointError[pindex] = (refPoints[pindex] - (transfMatrix * pickedPoints[pindex])).Norm();
			this->Log(GLLogStream::FILTER, "%s: %f", pointID[pindex].toStdString().c_str(), pointError[pindex]);
			referencingResults.append(QString("Point %1: %2\n").arg(pointID[pindex]).arg(QString::number(pointError[pindex])));
		}
		else
		{
			pointError[pindex] = -1.0;
		}
    }

    // update dialog
    referencingDialog->updateTable();
    glArea->update();

    // cleaning up
    FixP.clear();
    MovP.clear();
}

void EditReferencingPlugin::applyMatrix()
{
    status_error = "";

    Matrix44m newMat;

    newMat.Import(transfMatrix);

    if(referencingDialog->ui->cbApplyToAll->checkState() == Qt::Checked)
    {
		int lind = 0;
        foreach(MeshModel *mmp, glArea->md()->meshList)
        {
            if(mmp->visible)
            {
				mmp->cm.Tr = newMat * layersOriginalTransf[lind];
            }
			lind++;
        }
    }
    else
    {
		glArea->mm()->cm.Tr = newMat * originalTransf;
    }

    glArea->update();
}

void EditReferencingPlugin::updateDistances()
{
	// recalculate all current distances
	int dindex;
	Point3d currPA, currPB;

	for (dindex = 0; dindex < useDistance.size(); dindex++)
	{
		currPA = distPointA[dindex];
		currPB = distPointB[dindex];

		currDist[dindex] = (currPA - currPB).Norm();
	}

	// now update scale
	double curr_scale = 0;
	int numValid = 0;
	for (dindex = 0; dindex < useDistance.size(); dindex++)
	{
		if (currDist[dindex] == 0.0)
		{
			scaleFact[dindex] = 0.0;
		}
		else
		{
			scaleFact[dindex] = targDist[dindex] / currDist[dindex];

			if ((useDistance[dindex]) && (!scaleFact[dindex] == 0.0))
			{
				curr_scale += scaleFact[dindex];
				numValid++;
			}
		}
	}

	globalScale = (numValid == 0) ? 0.0 : curr_scale / (double)numValid;

	if (globalScale == 0.0)
		this->referencingDialog->ui->bttApplyScale->setEnabled(false);
	else
		this->referencingDialog->ui->bttApplyScale->setEnabled(true);

	// finally update error
	for (dindex = 0; dindex < useDistance.size(); dindex++)
	{
		distError[dindex] = (currDist[dindex] * globalScale) - targDist[dindex];
	}

	referencingDialog->updateTableDist();
	glArea->update();
}

void EditReferencingPlugin::addNewDistance()
{
    status_error = "";
    int pindex;
    bool alreadyThere;
    QString newname;

    // i do not want to have too many refs
    if(useDistance.size() > MAX_REFPOINTS)
    {
        status_error = "Too many points";
        return;
    }

    // I should check the name is really new... hehe :)
    do
    {
        alreadyThere = false;
        newname = "DD" + QString::number(lastname++);
        for(pindex=0; pindex<distanceID.size(); pindex++)
        {
            if(distanceID[pindex] == newname)
               alreadyThere=true;
        }
    }
    while(alreadyThere);

    useDistance.push_back(new bool(true));
    distanceID.push_back(newname);
    distPointA.push_back(Point3d(0.0, 0.0, 0.0));
    distPointB.push_back(Point3d(0.0, 0.0, 0.0));
    currDist.push_back(0.0);
    targDist.push_back(0.0);
	scaleFact.push_back(0.0);
    distError.push_back(0.0);

    // update dialog
	updateDistances();
    referencingDialog->updateTableDist();
    glArea->update();
}

void EditReferencingPlugin::deleteCurrentDistance()
{
    status_error = "";
    int pindex = referencingDialog->ui->tableWidgetDist->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
        return;
    }

    usePoint.erase(usePoint.begin() + pindex);
    pointID.erase(pointID.begin() + pindex);
    pickedPoints.erase(pickedPoints.begin() + pindex);
    refPoints.erase(refPoints.begin() + pindex);
    pointError.erase(pointError.begin() + pindex);

    // update dialog
	updateDistances();
    referencingDialog->updateTableDist();
    glArea->update();
}

void EditReferencingPlugin::pickCurrDistPA()
{
    status_error = "";
    int pindex = referencingDialog->ui->tableWidgetDist->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
        return;
    }

    lastAskedPick = EditReferencingPlugin::REF_SCALE;
    emit askSurfacePos("currentPA");
    status_line2 = "Double-click on model to pick point";
    glArea->update();
}

void EditReferencingPlugin::pickCurrDistPB()
{
    status_error = "";
    int pindex = referencingDialog->ui->tableWidgetDist->currentRow();

    // if nothing selected, skip
    if(pindex == -1)
    {
        status_error = "No point selected";
        return;
    }

    lastAskedPick = EditReferencingPlugin::REF_SCALE;
    emit askSurfacePos("currentPB");
    status_line2 = "Double-click on model to pick point";
    glArea->update();
}

void EditReferencingPlugin::applyScale()
{
	status_error = "";

	Matrix44m newMat;
	newMat.Identity();
	newMat.SetScale(globalScale, globalScale, globalScale);

	if (referencingDialog->ui->cbApplyToAll->checkState() == Qt::Checked)
	{
		int lind = 0;
		foreach(MeshModel *mmp, glArea->md()->meshList)
		{
			if (mmp->visible)
			{
				mmp->cm.Tr = newMat * layersOriginalTransf[lind];
			}
			lind++;
		}
	}
	else
	{
		glArea->mm()->cm.Tr = newMat * originalTransf;
	}

	glArea->update();
}

void EditReferencingPlugin::loadDistances()
{
	status_error = "";
	QString openFileName = "";
	
	openFileName = QFileDialog::getOpenFileName(NULL, "Import a List of Distances (ascii .txt)", QDir::currentPath(), "Text file (*.txt)");
	{
		// opening file
		QFile openFile(openFileName);

		if (openFile.open(QIODevice::ReadOnly))
		{
			QStringList tokenizedLine;
			QString firstline;
			double newXa = 0.0, newYa = 0.0, newZa = 0.0, newXb = 0.0, newYb = 0.0, newZb = 0.0, newDist = 0.0;
			bool parseXaOK = true, parseYaOK = true, parseZaOK = true, parseXbOK = true, parseYbOK = true, parseZbOK = true, parseDistOK = true;
			bool found = false;
			QString separator = "";

			useDistance.clear();
			distanceID.clear();
			distPointA.clear();
			distPointB.clear();
			currDist.clear();
			targDist.clear();
			scaleFact.clear();
			distError.clear();

			//getting first line, to understand separator
			if (!openFile.atEnd())
				firstline = QString(openFile.readLine()).simplified();
			else
			{
				status_error = "distance list format not recognized";
				openFile.close();
				referencingDialog->updateTableDist();
				glArea->update();
				return;
			}

			// discovering the separator 
			vector <QString> possible_separators = { QString(" "), QString(","), QString(";") };

			int sind = 0;
			while ((!found) || (sind<possible_separators.size()))
			{
				tokenizedLine = firstline.split(possible_separators[sind], QString::SkipEmptyParts);
				if (tokenizedLine.size() == 8 || (tokenizedLine.size() == 9 && firstline.right(1) == possible_separators[sind]))
				{
					newXa = tokenizedLine.at(1).toDouble(&parseXaOK);
					newYa = tokenizedLine.at(2).toDouble(&parseYaOK);
					newZa = tokenizedLine.at(3).toDouble(&parseZaOK);
					newXb = tokenizedLine.at(4).toDouble(&parseXbOK);
					newYb = tokenizedLine.at(5).toDouble(&parseYbOK);
					newZb = tokenizedLine.at(6).toDouble(&parseZbOK);
					newDist = tokenizedLine.at(7).toDouble(&parseDistOK);
					if (parseXaOK && parseYaOK && parseZaOK && parseXbOK && parseYbOK && parseZbOK && parseDistOK)
					{
						separator = possible_separators[sind]; found = true;
					}
				}
				sind++;
			}

			if (!found)
			{
				status_error = "distance list format not recognized";
				openFile.close();
				referencingDialog->updateTableDist();
				glArea->update();
				return;
			}

			// inserting first line
			tokenizedLine = firstline.split(separator, QString::SkipEmptyParts);
			newXa = tokenizedLine.at(1).toDouble(&parseXaOK);
			newYa = tokenizedLine.at(2).toDouble(&parseYaOK);
			newZa = tokenizedLine.at(3).toDouble(&parseZaOK);
			newXb = tokenizedLine.at(4).toDouble(&parseXbOK);
			newYb = tokenizedLine.at(5).toDouble(&parseYbOK);
			newZb = tokenizedLine.at(6).toDouble(&parseZbOK);
			newDist = tokenizedLine.at(7).toDouble(&parseDistOK);
			{
				useDistance.push_back(new bool(true));
				distanceID.push_back(tokenizedLine.at(0));
				distPointA.push_back(Point3d(newXa, newYa, newZa));
				distPointB.push_back(Point3d(newXb, newYb, newZb));
				currDist.push_back(0.0);
				targDist.push_back(newDist);
				scaleFact.push_back(0.0);
				distError.push_back(0.0);
			}

			// and now, the rest of the file
			while (!openFile.atEnd()) {
				QString newline = QString(openFile.readLine()).simplified();

				tokenizedLine = newline.split(separator, QString::SkipEmptyParts);
				if (tokenizedLine.size() == 8 || (tokenizedLine.size() == 9 && newline.right(1) == separator))
				{
					newXa = tokenizedLine.at(1).toDouble(&parseXaOK);
					newYa = tokenizedLine.at(2).toDouble(&parseYaOK);
					newZa = tokenizedLine.at(3).toDouble(&parseZaOK);
					newXb = tokenizedLine.at(4).toDouble(&parseXbOK);
					newYb = tokenizedLine.at(5).toDouble(&parseYbOK);
					newZb = tokenizedLine.at(6).toDouble(&parseZbOK);
					newDist = tokenizedLine.at(7).toDouble(&parseDistOK);
					if (parseXaOK && parseYaOK && parseZaOK && parseXbOK && parseYbOK && parseZbOK && parseDistOK)    //inserting
					{
						useDistance.push_back(new bool(true));
						distanceID.push_back(tokenizedLine.at(0));
						distPointA.push_back(Point3d(newXa, newYa, newZa));
						distPointB.push_back(Point3d(newXb, newYb, newZb));
						currDist.push_back(0.0);
						targDist.push_back(newDist);
						scaleFact.push_back(0.0);
						distError.push_back(0.0);
					}
				}
			}

			openFile.close();
			referencingDialog->updateTableDist();
			updateDistances();
			glArea->update();
		}
		else
		{
			status_error = "cannot open file";
			referencingDialog->updateTable();
			glArea->update();
		}
	}
}

void EditReferencingPlugin::exportScaling()
{
	// saving to file
	status_error = "";
	// saving
	int pindex;

	QString openFileName = "";
	openFileName = QFileDialog::getSaveFileName(NULL, "Save Scaling Process", QDir::currentPath(), "Text file (*.txt)");

	if (openFileName != "")
	{
		// opening file
		QFile openFile(openFileName);

		if (openFile.open(QIODevice::ReadWrite))
		{
			QTextStream openFileTS(&openFile);

			openFileTS << "---------------------SCALING DATA----------------------" << "\n\n";

			// writign results
			openFileTS << "-------------------------------------------------------\n";
			openFileTS << "[active][ID][Xa][Ya][Za][Xb][Yb][Zb][CurrD][TargD][Err]\n";
			openFileTS << "-------------------------------------------------------\n";
			for (pindex = 0; pindex<useDistance.size(); pindex++)
			{
				openFileTS << (useDistance[pindex] ? "Active" : "Inactive") << " " << distanceID[pindex] << " " <<
					distPointA[pindex][0] << " " << distPointA[pindex][1] << " " << distPointA[pindex][2] << " " <<
					distPointB[pindex][0] << " " << distPointB[pindex][1] << " " << distPointB[pindex][2] << " " <<
					currDist[pindex] << " " << targDist[pindex] << " ";
				if (useDistance[pindex])
					openFileTS << distError[pindex] << " \n";
				else
					openFileTS << "--" << " \n";
			}
			openFileTS << "-------------------------------------------------------\n";

			// writing reference
			openFileTS << "\n"  << "--- scaling results ---" << "\n";

			openFileTS << "\n" << "SCALE FACTOR: " << globalScale << "\n";

			for (pindex = 0; pindex<useDistance.size(); pindex++)
			{
				if ((useDistance[pindex] == true)&&(scaleFact[pindex]!=0.0))
				{
					openFileTS << "\n" << distanceID[pindex] << "\n";
					openFileTS << "A: " << distPointA[pindex][0] << " " << distPointA[pindex][1] << " " << distPointA[pindex][2] << "\n";
					openFileTS << "B: " << distPointB[pindex][0] << " " << distPointB[pindex][1] << " " << distPointB[pindex][2] << "\n";
					openFileTS << "Current Distance: " << currDist[pindex] << " Target Distance: " << targDist[pindex] << " Residual Error: " << distError[pindex] << "\n";
				}
			}

			openFile.close();
		}
	}


}
