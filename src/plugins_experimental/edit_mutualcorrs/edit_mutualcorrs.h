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

#ifndef EditMutualCorrsPlugin_H
#define EditMutualCorrsPlugin_H

#include <QObject>
#include <common/interfaces.h>
#include "edit_mutualcorrsDialog.h"
#include "alignset.h"

// function to calculate rototranslaton and rototranslaton+scale matrices from series of points
#include <vcg/space/point_matching.h>


class EditMutualCorrsPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
		
public:
    EditMutualCorrsPlugin();
    virtual ~EditMutualCorrsPlugin() {}

    static const QString Info();

    bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
    void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);

    void Decorate(MeshModel &/*m*/, GLArea * /*parent*/, QPainter *p);
    void Decorate (MeshModel &/*m*/, GLArea * ){};
    void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
		
    QPoint cur;
    QFont qFont;

    // the dialog
    edit_mutualcorrsDialog *mutualcorrsDialog;

    // used to draw over the rendering
    GLArea *glArea;

    //referencing data
    std::vector<bool>            usePoint;
    std::vector<QString>         pointID;
    std::vector<vcg::Point3d>    modelPoints;
    std::vector<vcg::Point2i>    imagePoints;
    std::vector<double>          pointError;

    int lastname;

    // status text or log
    QString status_line1;
    QString status_line2;
    QString status_line3;
    QString status_error;


private:
	AlignSet align;

public slots:
    void addNewPoint();
    void deleteCurrentPoint();

    void pickCurrentPoint();
    void pickCurrentRefPoint();
	vcg::Point3f fromPickedToImage(vcg::Point2f picked);
	vcg::Point2d fromImageToGL(vcg::Point2i picked);
    void receivedSurfacePoint(QString name,vcg::Point3f pPoint);
	void receivedImagePoint(QString name, vcg::Point2f pPoint);
	void receivedShot(QString name, Shotm shot);
	bool initGL();

    void loadFromFile();
    void saveToFile();

    void applyMutual();

signals:
    void askSurfacePos(QString);
	void askPickedPos(QString);
	void askTrackShot(QString);
};

#endif
