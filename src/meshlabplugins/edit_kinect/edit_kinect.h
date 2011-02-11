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

#ifndef KinectEditPlugin_H
#define KinectEditPlugin_H

#include <QObject>
#include <common/interfaces.h>
#include <common/meshmodel.h>
#include <vector>
#include "ui_kinect.h"



/* OCME include */
#include <wrap/gui/trackball.h>

class KinectEditPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)



public:
        KinectEditPlugin();
        virtual ~KinectEditPlugin() {}

    static const QString Info();

	virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);

	virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
        virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
        virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
        virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );

	void setTrackBall();
	void resetPlugin();
	QFont qFont;
        QDockWidget * kinect_panel;
        Ui::KinectDockWidget * odw;
	GLArea * gla;
	MeshModel * mm;						// mesh associated with ocme
	vcg::Trackball curr_track;
        bool cumulate_frames;
        float tilt,toinitialize;
        GLuint  gl_depth_tex,    // texture to upload the kinect values
                gl_depth_tex_avg,
                gl_color_tex,
                foc_loc,
                shx_loc,
                point_cloud,    // VBO of 680*480 vertex
				pr,             // program to displace vertices
				gl_undistort_rgb_tex,
				gl_undistort_depth_tex;

        enum {VIEW_SCANNED_MODEL,VIEW_REAL_TIME} ViewMode;
        struct Frame{ unsigned short data[640*480];};
        float frames_avg[640*480];
        float frames_var[640*480];
        std::vector<Frame> frames;
        unsigned char  rgb_stored[640*480*4];
        bool ScanningConverged();
        void InitializeGL();
        void CleanupGL();
		void UndistortMaps();
public slots:
        void loadCalibration();
        void startScan();
        void stopScan();
        void saveScan();
        void setViewMode();
        void setTilt(double);
};

#endif
