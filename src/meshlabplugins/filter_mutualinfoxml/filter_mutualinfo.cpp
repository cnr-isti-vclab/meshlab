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

#include "filter_mutualinfo.h"
#include "solver.h"
#include "mutual.h"
#include <wrap/gl/shot.h>
#include <wrap/gl/camera.h>

bool MutualInfoPlugin::applyFilter( const QString& filterName,MeshDocument& md,EnvWrap&env, vcg::CallBackPos * /*cb*/ )
{
    if (md.mm() == NULL)
        return false;
    if (filterName == "Image alignment: Mutual Information")
    {
        Solver solver;
        MutualInfo mutual;
        if (md.rasterList.size()==0)
        {
            Log(0, "You need a Raster Model to apply this filter!");
            return false;
        }
        else
            align.image=&md.rm()->currentPlane->image;

        align.mesh=&md.mm()->cm;

        int rendmode = env.evalEnum("RenderingMode");
        solver.optimize_focal = env.evalBool("EstimateFocal");
        solver.fine_alignment = env.evalBool("Fine");
        solver.variance = env.evalFloat("ExpectedVariance");
        solver.tolerance = env.evalFloat("Tolerance");
        solver.maxiter = env.evalInt("NumOfIterations");
        mutual.bweight = env.evalInt("BackgroundWeight");

        switch(rendmode)
        {
        case 0:
            align.mode=AlignSet::COMBINE;
            break;
        case 1:
            align.mode=AlignSet::NORMALMAP;
            break;
        case 2:
            align.mode=AlignSet::COLOR;
            break;
        case 3:
            align.mode=AlignSet::SPECULAR;
            break;
        case 4:
            align.mode=AlignSet::SILHOUETTE;
            break;
        case 5:
            align.mode=AlignSet::SPECAMB;
            break;
        default:
            align.mode=AlignSet::COMBINE;
            break;
        }

///// Loading geometry

        vcg::Point3f *vertices = new vcg::Point3f[align.mesh->vn];
        vcg::Point3f *normals = new vcg::Point3f[align.mesh->vn];
        vcg::Color4b *colors = new vcg::Color4b[align.mesh->vn];
        unsigned int *indices = new unsigned int[align.mesh->fn*3];

        for(int i = 0; i < align.mesh->vn; i++)
        {
            vertices[i].Import(align.mesh->vert[i].P());
            normals[i].Import(align.mesh->vert[i].N());
            colors[i] = align.mesh->vert[i].C();
        }

        align.shot = vcg::Shotf::Construct(env.evalShot("Shot"));

        align.shot.Intrinsics.ViewportPx[0]=int((double)align.shot.Intrinsics.ViewportPx[1]*align.image->width()/align.image->height());
        align.shot.Intrinsics.CenterPx[0]=(int)(align.shot.Intrinsics.ViewportPx[0]/2);

///// Initialize GLContext

        Log( "Initialize GL");
        this->glContext->makeCurrent();
            if (this->initGL() == false)
                return false;

            Log( "Done");

            for(int i = 0; i < align.mesh->fn; i++)
                for(int k = 0; k < 3; k++)
                    indices[k+i*3] = align.mesh->face[i].V(k) - &*align.mesh->vert.begin();

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, align.vbo);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, align.mesh->vn*sizeof(vcg::Point3f), vertices, GL_STATIC_DRAW_ARB);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, align.nbo);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, align.mesh->vn*sizeof(vcg::Point3f), normals, GL_STATIC_DRAW_ARB);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, align.cbo);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, align.mesh->vn*sizeof(vcg::Color4b), colors, GL_STATIC_DRAW_ARB);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, align.ibo);
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, align.mesh->fn*3*sizeof(unsigned int),
                indices, GL_STATIC_DRAW_ARB);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

///// Mutual info calculation: every 30 iterations, the mail glarea is updated
        int rounds=(int)(solver.maxiter/30);
        for (int i=0; i<rounds; i++)
        {
            Log( "Step %i of %i.", i+1, rounds );

            solver.maxiter=30;

            if (solver.fine_alignment)
                solver.optimize(&align, &mutual, align.shot);
            else
                solver.iterative(&align, &mutual, align.shot);

            md.rm()->shot = Shotm::Construct(align.shot);
            float ratio=(float)md.rm()->currentPlane->image.height()/(float)align.shot.Intrinsics.ViewportPx[1];
            md.rm()->shot.Intrinsics.ViewportPx[0]=md.rm()->currentPlane->image.width();
            md.rm()->shot.Intrinsics.ViewportPx[1]=md.rm()->currentPlane->image.height();
            md.rm()->shot.Intrinsics.PixelSizeMm[1]/=ratio;
            md.rm()->shot.Intrinsics.PixelSizeMm[0]/=ratio;
            md.rm()->shot.Intrinsics.CenterPx[0]=(int)((float)md.rm()->shot.Intrinsics.ViewportPx[0]/2.0);
            md.rm()->shot.Intrinsics.CenterPx[1]=(int)((float)md.rm()->shot.Intrinsics.ViewportPx[1]/2.0);

            QList<int> rl;
            rl << md.rm()->id();
            md.updateRenderStateRasters(rl,RasterModel::RM_ALL);
        }
        this->glContext->doneCurrent();

        // it is safe to delete after copying data to VBO
        delete []vertices;
        delete []normals;
        delete []colors;
        delete []indices;


        return true;
    }
    return false;
}

bool MutualInfoPlugin::initGL()
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
        Log(0,"Graphics hardware does not support non-power-of-two textures");
        return false;
    }
    if (!glewIsSupported("GL_ARB_vertex_buffer_object")) {
        Log(0, "Graphics hardware does not support vertex buffer objects");
        return false;
    }

    glEnable(GL_NORMALIZE);
    glDepthRange (0.0, 1.0);

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
MESHLAB_PLUGIN_NAME_EXPORTER(MutualInfoPlugin)
