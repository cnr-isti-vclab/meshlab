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

#ifndef AMBIENT_OCCLUSION_H
#define AMBIENT_OCCLUSION_H

#include <QObject>

#include <common/interfaces.h>
#include <wrap/gl/gl_surface.h>
class AOGLWidget;
class AmbientOcclusionPlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
    Q_INTERFACES(MeshFilterInterface)

        // Attributes
protected:
    std::vector<vcg::Point3f> viewDirVec;
    vcg::Point3f cameraDir;
    GLuint  fboDepth,
        fboResult,
        depthBufferTex,
        vertexCoordTex,
        vertexNormalsTex,
        *resultBufferTex;

    GLenum *resultBufferMRT,
        colorFormat,
        dataTypeFP;

    unsigned int numViews,
        depthTexArea,
        numTexPages;

    bool useGPU,
        errInit;
    bool perFace;
public:
    unsigned int depthTexSize,
        maxTexSize;

    // Methods
public:
    enum { FP_AMBIENT_OCCLUSION } ;

    AmbientOcclusionPlugin();
    ~AmbientOcclusionPlugin();

    QString pluginName() const;
    QString filterName(FilterIDType filter) const;
    QString	filterInfo(FilterIDType filterId) const;
	FILTER_ARITY filterArity(QAction*) const;
    int getRequirements (QAction *action);
    FilterClass getClass(const QAction* filter) const;

    void initParameterSet(QAction *,MeshModel &/*m*/,RichParameterList & /*parent*/);
    bool applyFilter(const QAction* filter, MeshDocument &md, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
    void initTextures(void);
    void initGL(vcg::CallBackPos *cb,unsigned int numVertices);
    bool processGL(MeshModel &m, std::vector<vcg::Point3f> &posVect);
    bool checkFramebuffer();

    void vertexCoordsToTexture(MeshModel &m);

    void setCamera(vcg::Point3f camDir,Box3m &meshBBox);

    void generateOcclusionHW();
    void generateOcclusionSW(MeshModel &m);
    void generateFaceOcclusionSW(MeshModel &m, std::vector<vcg::Point3f> & faceCenterVec);


    void applyOcclusionHW(MeshModel &m);
    void applyOcclusionSW(MeshModel &m,GLfloat *aoValues);

    void set_shaders(char *shaderName,GLuint &vs,GLuint &fs,GLuint &pr);

    void dumpFloatTexture(QString filename, float *texdata, int elems);
private:
    bool init;

};

#endif
