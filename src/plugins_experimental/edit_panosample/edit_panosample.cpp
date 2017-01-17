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
#include "edit_panosample.h"

#include <vcg/math/perlin_noise.h>
#include <wrap/glw/glw.h>


using namespace std;
using namespace vcg;
using namespace glw;

EditPanosamplePlugin::EditPanosamplePlugin()
{

}

EditPanosamplePlugin::~EditPanosamplePlugin() {}

const QString EditPanosamplePlugin::Info() {
	return tr("Improved Painting");
}



bool EditPanosamplePlugin::StartEdit(MeshModel & m, GLArea * parent, MLSceneGLSharedDataContext* /*cont*/)
{
	glarea = parent;
	meshmodel = &m;

	GLenum err = glewInit();
	if (err != GLEW_OK)
		return false;

	dock1 = new QDockWidget(parent->window());
	this->qualitychecker = new QualityChecker(dock1);
	//dock->setAllowedAreas(Qt::NoDockWidgetArea);
	//dock->setWidget(paintbox);
	//QPoint p = parent->mapToGlobal(QPoint(0, 0));
	//dock->setGeometry(5 + p.x(), p.y() + 5, paintbox->width(), parent->height() - 10);
	//dock->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
	dock1->setFloating(true);
	dock1->setVisible(true);
	

 	QObject::connect(qualitychecker, SIGNAL(createImageSpaceClicked()), this, SLOT(on_createImageSpace()));

	QObject::connect(qualitychecker, SIGNAL(layerChosenChanged), this, SLOT(on_layerChosenChanged()));


	createImageSpaceTrigger = false;

	return true;


}

void EditPanosamplePlugin::EndEdit(MeshModel &/* m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/)
{
	glarea->setMouseTracking(false);
	delete dock1;
}

void EditPanosamplePlugin::mousePressEvent(QMouseEvent * event, MeshModel &, GLArea * gla)
{
	gla->update();
}

void EditPanosamplePlugin::mouseMoveEvent(QMouseEvent* event, MeshModel & m, GLArea * gla)
{
	if (gla == NULL)
		return;

	gla->update();
}

void EditPanosamplePlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &, GLArea * gla)
{
	gla->updateAllSiblingsGLAreas();
}


void EditPanosamplePlugin::drawScene(GLArea * gla){
	MLSceneGLSharedDataContext* datacont = gla->mvc()->sharedDataContext();
	if (datacont == NULL)
		return;

	foreach(MeshModel * mp, gla->md()->meshList){
		MLRenderingData curr;
		datacont->getRenderInfoPerMeshView(mp->id(), gla->context(), curr);
		MLPerViewGLOptions opts;
		curr.get(opts);
		//if (curr.get(opts) == false)
		//	throw MLException(QString("GLArea: invalid MLPerViewGLOptions"));
		//gla->setLightingColors(opts);


		if (opts._back_face_cull)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		datacont->setMeshTransformationMatrix(mp->id(), mp->cm.Tr);
		
		if(mp->cm.fn)
			datacont->draw(mp->id(), gla->context());
	}
}

void EditPanosamplePlugin::panoRender(GLArea * gla){
	int winsize = 512;
	int width  = winsize * 3;
	int height = winsize * 2;
	

	glarea = gla;
	if (gla->mvc() == NULL)
		return;
	MLSceneGLSharedDataContext* shared = gla->mvc()->sharedDataContext();
	if (shared == NULL)
		return;

	Context ctx;
	ctx.acquire();

	RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
	Texture2DHandle    hColor = createTexture2D(ctx, GL_RGBA8, width, height, GL_RGBA, GL_UNSIGNED_BYTE);
	FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	glViewport(0, 0, winsize, winsize);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1.0, 0.1, 100); // SISTEMARE NEAR E FAR
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ctx.bindReadDrawFramebuffer(hFramebuffer);
	GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;


	MLSceneGLSharedDataContext::PerMeshRenderingDataMap dt;
	shared->getRenderInfoPerMeshView(gla->context(), dt);

	for (int i = 0; i < meshmodel->cm.vert.size(); ++i){
		CMeshO::CoordType p = meshmodel->cm.vert[i].cP();

		QImage image(int(width), int(height), QImage::Format_ARGB32);

		
		//*** Z+
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, winsize, winsize);
		glPushMatrix();
		gluLookAt(p.X(), p.Y(), p.Z(), p.X(), p.Y(), p.Z() + 1, 0, 1, 0);
		drawScene(gla);
		glPopMatrix();

		//*** X-
		glViewport(winsize, 0, winsize, winsize);
		glPushMatrix();
		glViewport(winsize, 0, winsize, winsize);
		gluLookAt(p.X(), p.Y(), p.Z(), p.X()-1, p.Y(), p.Z() , 0, 1, 0);
		drawScene(gla);
		glPopMatrix();

		//*** Z-
		glViewport(winsize * 2, 0, winsize, winsize);
		glPushMatrix();
		gluLookAt(p.X(), p.Y(), p.Z(), p.X(), p.Y(), p.Z() - 1, 0, 1, 0);
		drawScene(gla);
		glPopMatrix();

		//*** Y+
		glViewport(0, winsize, winsize, winsize);
		glPushMatrix();
		gluLookAt(p.X(), p.Y(), p.Z(), p.X(), p.Y()+ 1, p.Z() , 0, 0, 1);
		drawScene(gla);
		glPopMatrix();

		//*** X+
		glViewport(winsize, winsize, winsize, winsize);
		glPushMatrix();
		gluLookAt(p.X(), p.Y(), p.Z(), p.X() + 1, p.Y(), p.Z(), 0, 0, 1);
		drawScene(gla);
		glPopMatrix();

		//*** Y-
		glViewport(winsize * 2, winsize, winsize, winsize);
		glPushMatrix();
		gluLookAt(p.X(), p.Y(), p.Z(), p.X(), p.Y() - 1, p.Z(), 0, 0, 1);
		drawScene(gla);
		glPopMatrix();

		glReadPixels(0, 0, 3 * winsize, 2 * winsize, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

		image.rgbSwapped().mirrored().save("pano_"+QString().setNum(i)+".jpg");
	}
		ctx.unbindReadDrawFramebuffer();
		ctx.release();
	glViewport(vp[0], vp[1], vp[2], vp[3]);
	return;

}
/**
 * Since only on a Decorate call it is possible to obtain correct values
 * from OpenGL, all operations are performed during the execution of this
 * method and not where mouse events are processed.
 *
 */
void EditPanosamplePlugin::Decorate(MeshModel &m, GLArea * gla)
{
	if (createImageSpaceTrigger){
		panoRender(gla);
		createImageSpaceTrigger = false;
	}
}


void EditPanosamplePlugin::on_createImageSpace(){
	createImageSpaceTrigger = true;
	update();
}



void EditPanosamplePlugin::on_layerChosenChanged(){
	createImageSpaceTrigger = true;
	update();
}


/**
 * Request an async repainting of the glarea
 *
 * This slot is connected to undo and redo
 */
void EditPanosamplePlugin::update()
{
	glarea->update();
	glarea->updateAllSiblingsGLAreas();
}

