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

#include "filter_sample_gpu.h"
#include <common/GLExtensionsManager.h>
#include <wrap/glw/glw.h>
#include <QImage>

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

using namespace glw;

ExtraSampleGPUPlugin::ExtraSampleGPUPlugin()
{
	typeList << FP_GPU_EXAMPLE;

  foreach(FilterIDType tt , types())
      actionList << new QAction(filterName(tt), this);
}

QString ExtraSampleGPUPlugin::pluginName() const
{
    return "FilterSampleGPU";
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString ExtraSampleGPUPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_GPU_EXAMPLE :  return QString("GPU Filter Example ");
		default : assert(0);
	}
  return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString ExtraSampleGPUPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
	case FP_GPU_EXAMPLE : 
		return QString("Small useless filter added only to show how to work with a gl render context inside a filter.");
	default:
		assert(0);
	}
	return QString("Unknown Filter");
}
 
bool ExtraSampleGPUPlugin::requiresGLContext(const QAction* action) const
{
	switch(ID(action)){
	case FP_GPU_EXAMPLE :
		return true;
	default:
		assert(0);
	}
	return false;
}

// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be chosen.
ExtraSampleGPUPlugin::FilterClass ExtraSampleGPUPlugin::getClass(const QAction *a) const
{
	switch(ID(a))
	{
		case FP_GPU_EXAMPLE:  return FilterPluginInterface::RasterLayer;  //should be generic, but better avoid it
		default : assert(0);
	}
	return FilterPluginInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void ExtraSampleGPUPlugin::initParameterList(const QAction * action, MeshModel & m, RichParameterList & parlst)
{
	(void)m;

	switch(ID(action))
	{
		case FP_GPU_EXAMPLE :
		{
			parlst.addParam(RichColor    ("ImageBackgroundColor", QColor(50, 50, 50),                 "Image Background Color", "The color used as image background."        ));
			parlst.addParam(RichInt      ("ImageWidth",           512,                                "Image Width",            "The width in pixels of the produced image." ));
			parlst.addParam(RichInt      ("ImageHeight",          512,                                "Image Height",           "The height in pixels of the produced image."));
			//QString curr = QDir::currentPath();
			parlst.addParam(RichSaveFile ("ImageFileName",        /*curr + "/" */+"gpu_generated_image.png", "*.png", "Base Image File Name",   "The file name used to save the image."      ));
			break;
		}
		default : assert(0);
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool ExtraSampleGPUPlugin::applyFilter(const QAction * a, MeshDocument & md , std::map<std::string, QVariant>&, unsigned int& /*postConditionMask*/, const RichParameterList & par, vcg::CallBackPos * /*cb*/)
{
	if (glContext == nullptr){
		errorMessage = "Fatal error: glContext not initialized";
		return false;
	}
	switch(ID(a))
	{
		case FP_GPU_EXAMPLE:
		{
			CMeshO & mesh = md.mm()->cm;
			if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

//			const unsigned char * p0      = (const unsigned char *)(&(mesh.vert[0].P()));
//			const unsigned char * p1      = (const unsigned char *)(&(mesh.vert[1].P()));
//			const void *          pbase   = p0;
//			GLsizei               pstride = GLsizei(p1 - p0);

//			const unsigned char * n0      = (const unsigned char *)(&(mesh.vert[0].N()));
//			const unsigned char * n1      = (const unsigned char *)(&(mesh.vert[1].N()));
//			const void *          nbase   = n0;
//			GLsizei               nstride = GLsizei(n1 - n0);

			glContext->makeCurrent();
			GLExtensionsManager::initializeGLextensions();

			glPushAttrib(GL_ALL_ATTRIB_BITS);

			Context ctx;
			ctx.acquire();

			/*const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
            BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

            const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
            BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

            const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
            BufferHandle hIndexBuffer = createBuffer(ctx, isize);

            {
                BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

                const CMeshO::VertexType * vbase   = &(mesh.vert[0]);
                GLuint *                   indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
                for (size_t i=0; i<mesh.face.size(); ++i)
                {
                    const CMeshO::FaceType & f = mesh.face[i];
                    if (f.IsD()) continue;
                    for (int v=0; v<3; ++v)
                    {
                        *indices++ = GLuint(vcg::tri::Index(mesh,f.cV(v)));
                    }
                }
                indexBuffer->unmap();

                ctx.unbindIndexBuffer();
            }*/

			const GLsizei width  = GLsizei(par.getInt("ImageWidth" ));
			const GLsizei height = GLsizei(par.getInt("ImageHeight"));

			RenderbufferHandle hDepth       = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
			Texture2DHandle    hColor       = createTexture2D(ctx, GL_RGBA8, width, height, GL_RGBA, GL_UNSIGNED_BYTE);
			FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));

			const std::string vertSrc = GLW_STRINGIFY
			(
				varying vec3 vNormalVS;
				void main(void)
				{
					vNormalVS   = gl_NormalMatrix * gl_Normal;
					gl_Position = ftransform();
				}
			);

			const std::string fragSrc = GLW_STRINGIFY
			(
				uniform vec3 uLightDirectionVS;
				varying vec3 vNormalVS;
				void main(void)
				{
					vec3  normal  = normalize(vNormalVS);
					float lambert = max(0.0, dot(normal, -uLightDirectionVS));
					gl_FragColor  = vec4(vec3(lambert), 1.0);
				}
			);

			ProgramHandle hProgram = createProgram(ctx, "", vertSrc, fragSrc);
			GLW_ASSERT(hProgram->isLinked());

			const QColor       backgroundColor  = par.getColor("ImageBackgroundColor");
			const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

			glEnable(GL_DEPTH_TEST);
			glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, GLfloat(backgroundColor.alpha()) / 255.0f);

			glViewport(0, 0, width, height);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluPerspective(50.0f, float(width) / float(height), 0.1f, 2.0f);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

			const vcg::Point3f center = mesh.bbox.Center();
			const float        scale  = 1.0f / mesh.bbox.Diag();

			glScalef(scale, scale, scale);
			glTranslatef(-center[0], -center[1], -center[2]);

			QImage image(int(width), int(height), QImage::Format_ARGB32);

			ctx.bindReadDrawFramebuffer(hFramebuffer);
				GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				BoundProgramHandle program = ctx.bindProgram(hProgram);
					program->setUniform("uLightDirectionVS", lightDirectionVS[0], lightDirectionVS[1], lightDirectionVS[2]);
                    MLRenderingData dt;
                    MLRenderingData::RendAtts atts;
                    atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
                    atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
                    dt.set(MLRenderingData::PR_POINTS,atts);
                    dt.set(MLRenderingData::PR_WIREFRAME_TRIANGLES,atts);
                    MLPerViewGLOptions opts;
                    opts._perpoint_pointsize = 5.0;
                    dt.set(opts);
                    glContext->setRenderingData(md.mm()->id(),dt);
                    glContext->drawMeshModel(md.mm()->id());
				ctx.unbindProgram();

				glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
			ctx.unbindReadDrawFramebuffer();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			ctx.release();
			glPopAttrib();
			glContext->doneCurrent();

			QString st = par.getSaveFileName("ImageFileName");

			image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));

			break;
		}
	}
	return true;
}

MESHLAB_PLUGIN_NAME_EXPORTER(ExtraSampleGPUPlugin)
