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
#include <common/gl_defs.h>
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
        align.meshid = md.mm()->id();

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

        align.shot = vcg::Shotf::Construct(env.evalShot("Shot"));

        align.shot.Intrinsics.ViewportPx[0]=int((double)align.shot.Intrinsics.ViewportPx[1]*align.image->width()/align.image->height());
        align.shot.Intrinsics.CenterPx[0]=(int)(align.shot.Intrinsics.ViewportPx[0]/2);

///// Initialize GLContext

        Log( "Initialize GL");
        align.setGLContext(glContext);
        glContext->makeCurrent(); 
        if (this->initGL() == false)
            return false;

        Log( "Done");

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

            //md.updateRenderStateRasters(rl,RasterModel::RM_ALL);

            md.documentUpdated();
        }
		this->glContext->doneCurrent();


        return true;
    }
    return false;
}

bool MutualInfoPlugin::initGL()
{
    Log(0, "GL Initialization");
    if (!initializeGLextensions_notThrowing()) {
        Log(0, "GLEW initialization error!");
        return false;
    }

    if (!glExtensionsHasEXT_framebuffer_object()) {
        Log(0, "Graphics hardware does not support FBOs");
        return false;
    }
    if (!glExtensionsHasARB_vertex_shader() || !glExtensionsHasARB_fragment_shader() ||
        !glExtensionsHasARB_shader_objects() || !glExtensionsHasARB_shading_language_100()) {
            //QMessageBox::warning(this, "Danger, Will Robinson!",
            //                         "Graphics hardware does not fully support Shaders");
    }

    if (!glExtensionsHasARB_texture_non_power_of_two()) {
        Log(0,"Graphics hardware does not support non-power-of-two textures");
        return false;
    }
    if (!glExtensionsHasARB_vertex_buffer_object()) {
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
