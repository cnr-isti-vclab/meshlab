/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mm->cm processing toolbox                             o     o   *
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
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "edit_kinect.h"
#include "ui_kinect.h"
#include <wrap/gl/picking.h>
#include<vcg/complex/append.h>
#include "freenect.h"
#include "shader_basic.h"
#include "calibration_data.h"

#define GL_TEST_ERR\
        {\
                        GLenum eCode;\
                        if((eCode=glGetError())!=GL_NO_ERROR)\
                                        std::cerr << "OpenGL error : " <<  gluErrorString(eCode) << " in " <<  __FILE__ << " : " << __LINE__ << std::endl;\
        }

using namespace std;
using namespace vcg;



KinectEditPlugin::KinectEditPlugin() {
        cumulate_frames = false;
//        n_frame = 1;
        ViewMode = VIEW_REAL_TIME;
	qFont.setFamily("Helvetica");
        qFont.setPixelSize(12);
        toinitialize = true;
        tilt = 0.0;
	mm = NULL;


}

const QString KinectEditPlugin::Info()
{
        return tr("Kinect.");
}
 
void KinectEditPlugin::mousePressEvent(QMouseEvent */*e*/, MeshModel &, GLArea * ) {
//	pickx = e->pos().x();
//	picky = e->pos().y();
//	pick = true;
//	start=e->pos();
//	cur=start;
};
void KinectEditPlugin::mouseMoveEvent(QMouseEvent */*e*/, MeshModel &, GLArea * ) {
// isDragging = true;
// prev=cur;
// cur=e->pos();
// a->update();
};



void KinectEditPlugin::mouseReleaseEvent(QMouseEvent * , MeshModel &   , GLArea *  )
{

}

bool KinectEditPlugin::ScanningConverged(){
    unsigned short n_avg[640*480];
    memset(&frames_avg[0],0,640*480*sizeof(float));
    memset(&frames_var[0],0,640*480*sizeof(float));
    memset(&n_avg[0],0,640*480*sizeof(unsigned short));

    for(unsigned int fi = 0; fi < frames.size(); ++fi)
        for(unsigned int i = 0; i < 640*480; ++i)
            if(frames[fi].data[i] < 2000){
            frames_avg[i] += frames[fi].data[i];
            n_avg[i]++;
        }

     for(unsigned int i = 0; i < 640*480; ++i)
         frames_avg[i] /= n_avg[i];

    for(unsigned int fi = 0; fi < frames.size(); ++fi)
        for(unsigned int i = 0; i < 640*480; ++i)
            if(frames[fi].data[i] < 2000)
                frames_var[i] += (frames[fi].data[i]-frames_avg[i])*(frames[fi].data[i]-frames_avg[i]);
        for(unsigned int i = 0; i < 640*480; ++i)
            frames_var[i] /= n_avg[i];


     unsigned short ttt[640*480];
     for(unsigned int i = 0; i < 640*480; ++i)
         ttt[i] = frames_avg[i];

    glBindTexture(GL_TEXTURE_2D,gl_depth_tex_avg);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,640,480,GL_RED,GL_UNSIGNED_SHORT,&ttt[0]);

    // just one scan
    return (frames.size()==1);
}

void KinectEditPlugin::CleanupGL(){
    glDeleteTextures(1,  &gl_depth_tex);
    glDeleteTextures(1,  &gl_depth_tex_avg);
	glDeleteTextures(1,  &gl_undistort_depth_tex);
	glDeleteTextures(1,  &gl_undistort_rgb_tex);
    glDeleteTextures(1,  &gl_color_tex);
    glDeleteBuffers(1,   &point_cloud);
    glDeleteProgram(pr);
}

void KinectEditPlugin::UndistortMaps()
{
	/*RGB Undistort Texture, loading on the device*/
	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &gl_undistort_rgb_tex);
	glBindTexture(GL_TEXTURE_2D, gl_undistort_rgb_tex);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_R32F, 1024, 512, 0, GL_RED, GL_UNSIGNED_SHORT,  0);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	/*Depth Undistort Texture, loading on the device*/
	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &gl_undistort_depth_tex);
	glBindTexture(GL_TEXTURE_2D, gl_undistort_depth_tex);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_R32F, 1024, 512, 0, GL_RED, GL_UNSIGNED_SHORT,  0);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

}

void KinectEditPlugin::InitializeGL(){


    /* generate texture to store kinect map*/
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &gl_depth_tex);
    glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_R32F, 1024, 512, 0, GL_RED, GL_UNSIGNED_SHORT,  0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glGenTextures(1, &gl_depth_tex_avg);
    glBindTexture(GL_TEXTURE_2D, gl_depth_tex_avg);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_R32F, 1024, 512, 0, GL_RED, GL_UNSIGNED_SHORT,  0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);


    glActiveTexture(GL_TEXTURE2);

    glGenTextures(1, &gl_color_tex);
    glBindTexture(GL_TEXTURE_2D, gl_color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE,  0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);


    /* create the VBO for the vertices  */
    glGenBuffersARB(1,&point_cloud);
    float points[640*480][3];
    for(unsigned int i = 0 ; i < 640;++i)
        for(unsigned int j = 0 ; j < 480;++j){
            points[j*640+i][0] = i/float(1024);
            points[j*640+i][1] = j/float(512);
            }

    glBindBuffer(GL_ARRAY_BUFFER,  point_cloud );
    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*640*480,&points[0][0], GL_STATIC_DRAW);

    GLuint fs, vs;

    std::string str_vs =
    std::string("varying bool kill;")+
	std::string("uniform sampler2D rgb_undistort;")+
	std::string("uniform sampler2D depth_undistort;")+
    std::string("uniform sampler2D col;")+
    std::string("uniform sampler2D dv;")+
    std::string("uniform float focal;")+

    std::string("uniform mat3 R;")+
    std::string("uniform mat3 rgb_intrinsics;")+
    std::string("uniform vec3 T;")+

    std::string("vec4 point(float X, float Y){")+
    std::string(" vec4 pp;")+
    std::string(" vec4 p = texture2D(dv,vec2(X,Y));")+
    std::string(" pp.z =  0.1236 *tan(p.x*65536.0 / 2842.5 + 1.1863);")+
    std::string(" pp.x = (X*1024.0/640.0-0.5)*pp.z * 1.1904;")+
    std::string(" pp.y = (Y*512.0/480.0-0.5)*pp.z * 0.8832;")+
    std::string(" if( (pp.z> 5.0) || (pp.z < 0.50)) pp.w= 0.0; else pp.w = 1.0;")+
    std::string(" return pp;}")+

    std::string("vec2 proj_color(vec4 p){")+
    std::string(" vec3 pp;")+
    std::string(" pp = R * vec3(p.x,p.y,p.z)+T ;")+
    std::string(" pp.x =  pp.x * rgb_intrinsics[0][0] / pp.z + rgb_intrinsics[0][2];")+
    std::string(" pp.y =  pp.y * rgb_intrinsics[1][1] / pp.z + rgb_intrinsics[1][2];")+
    std::string(" pp.x =  pp.x / 1024.f;")+
    std::string(" pp.y =  pp.y  / 512.f;")+
    std::string(" return vec2(pp.x,pp.y);}")+

    std::string("void main() { ")+
    std::string(" vec4 pp  = point(gl_Vertex.x,gl_Vertex.y);")+
    std::string(" gl_Position  = gl_ModelViewProjectionMatrix * pp;")+
    std::string(" vec4 p_dx = point(gl_Vertex.x+3.0/1024.0,gl_Vertex.y);")+
    std::string(" vec4 p_dy = point(gl_Vertex.x,gl_Vertex.y+3.0/480.0);")+
    std::string(" vec3 nn = normalize(cross( p_dx-pp,p_dy-pp));")+
    std::string(" nn = gl_NormalMatrix * nn;")+
    std::string(" nn = normalize(nn);")+
    std::string(" vec2  col_coord = proj_color(pp);")+

    std::string(" vec3 lightVec = normalize(gl_LightSource[0].position.xyz);")+
    std::string(" float gray = abs(lightVec*nn);")+

//    std::string(" vec4 color  = texture2D(col,vec2(gl_Vertex.x,gl_Vertex.y));")+
    std::string(" vec4 color  = texture2D(col,col_coord);")+

 //   std::string(" vec4 color  = texture2D(col,proj_color(pp));")+
  //      std::string(" gl_FrontColor = vec4(gray,gray,gray,1.0);")+
    std::string(" gl_FrontColor = vec4(color.xyz,1.0);")+
    std::string(" if(pp.w==0) kill = true;else kill=false;")+
    std::string("}");
    std::string str_fs =
    std::string("varying bool kill;")+
    std::string("void main() { ")+
    std::string(" gl_FragColor = gl_Color;")+
    std::string(" if (kill) discard;")+
    std::string("}");

    if(Shader::SetFromString(str_vs.c_str(),str_fs.c_str(),fs,vs,pr)==0)
        Shader::Validate(pr);
    glUseProgram(pr);
    GLuint depth_loc = glGetUniformLocation(pr,"dv");
    glUniform1i(depth_loc,0);
    GLuint col_loc = glGetUniformLocation(pr,"col");
    glUniform1i(col_loc,2);

	GLuint rgb_dis_loc = glGetUniformLocation(pr,"rgb_undistort");
	glUniform1i(rgb_dis_loc,3);
	GLuint depth_dis_loc = glGetUniformLocation(pr,"depth_undistort");
	glUniform1i(depth_dis_loc,4);

    GLuint R_loc = glGetUniformLocation(pr,"R");
    glUniformMatrix3fv(R_loc,1,false,&R[0][0]);
    GLuint T_loc = glGetUniformLocation(pr,"T");
    glUniform3fv(T_loc,1,&T[0]);

    GLuint rgb_intrinsics_loc = glGetUniformLocation(pr,"rgb_intrinsics");
    glUniformMatrix3fv(rgb_intrinsics_loc,1,false,&rgb_intrinsics[0][0]);

    glUseProgram(0);

}

void KinectEditPlugin::Decorate(MeshModel &, GLArea *  )
{


    if(toinitialize){
        InitializeGL();
        toinitialize = false;
    }
    kinect_qt::qbackbuf_mutex.lock();

    while ( kinect_qt::got_frames < 2) {
            kinect_qt::qwait.wait(& kinect_qt::qbackbuf_mutex);
    }

    glActiveTexture(GL_TEXTURE0);

    if(ViewMode==VIEW_REAL_TIME){
        glBindTexture(GL_TEXTURE_2D,gl_depth_tex);
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,640,480,GL_RED,GL_UNSIGNED_SHORT,&kinect_qt::gl_depth_front[0]);
    }


    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,gl_color_tex);

    if(ViewMode==VIEW_REAL_TIME)
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,640,480,GL_RGB,GL_UNSIGNED_BYTE,&kinect_qt::gl_rgb_back[0]);
       else
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,640,480,GL_RGB,GL_UNSIGNED_BYTE,&rgb_stored[0]);

    if(cumulate_frames){
        frames.push_back(Frame());
        memcpy(&frames.back().data[0],&kinect_qt::gl_depth_front[0], sizeof(kinect_qt::gl_depth_front));
        if( ScanningConverged()){
            cumulate_frames = false;
            frames.clear();
            memcpy(&rgb_stored[0],&kinect_qt::gl_rgb_back[0],640*480*4);
            }
    }

    kinect_qt::qbackbuf_mutex.unlock();


    glActiveTexture(GL_TEXTURE0);
     if(ViewMode==VIEW_REAL_TIME)
        glBindTexture(GL_TEXTURE_2D,gl_depth_tex);
     else
         glBindTexture(GL_TEXTURE_2D,gl_depth_tex_avg);




    glBindBufferARB(GL_ARRAY_BUFFER_ARB, point_cloud);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);


    glUseProgram(pr);

    glDrawArrays(GL_POINTS,0,640*480);
    glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
    glUseProgram(0);


}


void KinectEditPlugin::loadCalibration(){
    QString filename = QFileDialog::getOpenFileName((QWidget*)0,tr("Open YML"), QDir::currentPath(),
                                             tr("Ocm file (*.yml )"));
    if(!filename.isEmpty())
        if(!::LoadCalibrationData(filename))
            ::LoadCalibrationData("default_calibration.yml");
	UndistortMaps();

}

void KinectEditPlugin::startScan(){
    cumulate_frames = true;
    odw->vsmRadioButton->setChecked(true);
    odw->vrtRadioButton->setChecked(false);
    ViewMode = VIEW_SCANNED_MODEL;


}
void KinectEditPlugin::stopScan(){
    cumulate_frames = false;
    frames.clear()  ;
}

void KinectEditPlugin::setViewMode(){
    if(odw->vsmRadioButton->isChecked())
        ViewMode = VIEW_SCANNED_MODEL;
    else
        ViewMode = VIEW_REAL_TIME;
}

void KinectEditPlugin::setTilt(double v){
    tilt = v;
    kinect_qt::set_tilt_degs(v);
}

vcg::Point3f Point(float v, float X, float Y){
    vcg::Point3f pp;
    pp[2] =  0.1236 *tan(v / 2842.5 + 1.1863);
    pp[0] = (X - 640 / 2) * pp[2] * 0.00186;
    pp[1] = (Y - 480 / 2) * pp[2] * 0.00184;
    return pp;
    }

void KinectEditPlugin::saveScan(){
    mm = (MeshModel*)1;
	mm = gla->md()->addNewMesh("Kinect Mesh",false);

    CMeshO::VertexIterator vi =
            vcg::tri::Allocator<CMeshO>::AddVertices(mm->cm,640*480);

    mm->updateDataMask( MeshModel::MM_VERTCOLOR|MeshModel::MM_VERTQUALITY);
    for(unsigned int i = 0; i < 640; ++i)
        for(unsigned int j = 0; j < 480; ++j){

            (*vi).P() = Point(frames_avg[j*640+i],i,j);
            (*vi).Q() =  1.0/(frames_var[j*640+i]+1);
            (*vi).C() =  vcg::Color4b(rgb_stored[3*(j*640+i)], rgb_stored[3*(j*640+i)+1],rgb_stored[3*(j*640+i)+2],255);
            if((*vi).P()!=(*vi).P())
                vcg::tri::Allocator<CMeshO>::DeleteVertex(mm->cm,*vi);
            vi++;
        }
    mm->cm.bbox.Add(vcg::Point3f(-3,4,5));
    mm->cm.bbox.Add(vcg::Point3f(3,-4,0));

}

bool KinectEditPlugin::StartEdit(MeshModel &/*m*/, GLArea *_gla )
{
        gla = _gla;

        DefaultCalibrationData();
        odw = new Ui::KinectDockWidget ();
        kinect_panel  = new QDockWidget(gla);
        odw->setupUi(kinect_panel);

        kinect_panel->show();

        kinect_qt::start_kinect();
        QObject::connect(odw->loadCalibrationPushButton,SIGNAL(clicked()),this,SLOT(loadCalibration()));
        QObject::connect(odw->startScanPushButton,SIGNAL(clicked()),this,SLOT(startScan()));
        QObject::connect(odw->saveScanPushButton,SIGNAL(clicked()),this,SLOT(saveScan()));

        QObject::connect(odw->vrtRadioButton,SIGNAL(clicked()),this,SLOT(setViewMode()));
        QObject::connect(odw->vsmRadioButton,SIGNAL(clicked()),this,SLOT(setViewMode()));
        QObject::connect(odw->tiltSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setTilt(double)));

        odw->vrtRadioButton->setChecked(true);


        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), _gla, SLOT(update()));
        timer->start(50);

        {
            this->curr_track.track.tra=gla->trackball.track.tra  ;
            this->curr_track.track.sca=gla->trackball.track.sca  ;
            this->curr_track.track.rot=gla->trackball.track.rot  ;


            vcg::Quaternionf q; q.FromEulerAngles(3.14,0,0);
            gla->trackball.Reset();
            float newScale= 3.0f/5.0;
            gla->trackball.track.sca = newScale;
            gla->trackball.track.tra =  vcg::Point3f(0,0,-2.5);
            gla->trackball.track.rot = q;
        }


	return true;
}

void KinectEditPlugin::EndEdit(MeshModel &/*m*/, GLArea *gla )
{
     //if(mm) return;
        kinect_panel->hide();
        kinect_qt::stop_kinect();
        toinitialize = true;
        {
            gla->trackball.track.tra = this->curr_track.track.tra;
            gla->trackball.track.sca = this->curr_track.track.sca;
            gla->trackball.track.rot = this->curr_track.track.rot;
        }

}

void KinectEditPlugin::setTrackBall(){
// gla->trackball.Reset();
// float newScale= 3.0f/ocme_bbox.Diag();
// gla->trackball.track.sca = newScale;
// gla->trackball.track.tra =  -ocme_bbox.Center();

}
void KinectEditPlugin::resetPlugin(){

}
