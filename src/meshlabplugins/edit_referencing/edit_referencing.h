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

#ifndef EDITREFERENCINGPLUGIN_H
#define EDITREFERENCINGPLUGIN_H

#include <QObject>
#include <common/interfaces.h>
#include "edit_referencingDialog.h"

// function to calculate rototranslaton and rototranslaton+scale matrices from series of points
#include <vcg/space/point_matching.h>


class EditReferencingPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
		
public:

    enum refModeType {REF_ABSOLUTE, REF_SCALE};

    EditReferencingPlugin();
    virtual ~EditReferencingPlugin() {}

    static const QString Info();

	bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
    void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);

    void Decorate(MeshModel &/*m*/, GLArea *parent, QPainter *p);
	void DecorateAbsolute(MeshModel &/*m*/, GLArea *parent, QPainter *p);
	void DecorateScale(MeshModel &/*m*/, GLArea *parent, QPainter *p);
    void Decorate (MeshModel &/*m*/, GLArea * ){};
    void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
		
	void updateDistances();

    QPoint cur;
    QFont qFont;

    // the dialog
    edit_referencingDialog *referencingDialog;

    // used to draw over the rendering
    GLArea *glArea;

    //referencing mode
    refModeType                  referencingMode;

	// original transform(s)
	std::vector<Matrix44m>		layersOriginalTransf;
	Matrix44m					originalTransf;

    //referencing data (points)
    std::vector<bool>            usePoint;
    std::vector<QString>         pointID;
    std::vector<vcg::Point3d>    pickedPoints;
    std::vector<vcg::Point3d>    refPoints;
    std::vector<double>          pointError;

    //referencing data (distances)
    std::vector<bool>            useDistance;
    std::vector<QString>         distanceID;
    std::vector<vcg::Point3d>    distPointA;
    std::vector<vcg::Point3d>    distPointB;
    std::vector<double>          currDist;
    std::vector<double>          targDist;
	std::vector<double>          scaleFact;
    std::vector<double>          distError;
	double globalScale;


    vcg::Matrix44d transfMatrix;

    int lastname;

    refModeType lastAskedPick;

    bool validMatrix;
    bool isMatrixRigid;

    // status text or log
    QString status_line1;
    QString status_line2;
    QString status_line3;
    QString status_error;

    // string to hold referencing result (for exporting documentation)
    QString referencingResults;

public slots:

    void receivedSurfacePoint(QString name, Point3m pPoint);

    void addNewPoint();
    void deleteCurrentPoint();
    void pickCurrentPoint();
    void pickCurrentRefPoint();
	void calculateMatrix();
	void applyMatrix();
	void loadFromFile();
    void saveToFile();

    void addNewDistance();
    void deleteCurrentDistance();
    void pickCurrDistPA();
	void pickCurrDistPB();
	void applyScale();
	void loadDistances();
	void exportScaling();


signals:
    void askSurfacePos(QString);
};

#endif
