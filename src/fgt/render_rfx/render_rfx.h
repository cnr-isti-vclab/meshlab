/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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

#ifndef RENDERRFX_H
#define RENDERRFX_H

#include <GL/glew.h>
#include <QGLWidget>
#include <QMapIterator>
#include <QtGui>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "rfx_uniform.h"
#include "rfx_shader.h"
#include "rfx_parser.h"
#include "rfx_dialog.h"
#include "rfx_specialattribute.h"
 

class RenderRFX : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshRenderInterface)

public:
	RenderRFX();
	~RenderRFX();
	void Init(QAction*, MeshDocument&, RenderMode&, QGLWidget*);
	void Render(QAction*, MeshDocument&, RenderMode&, QGLWidget*);
	void Finalize(QAction*, MeshDocument&, GLArea*);
	bool isSupported()         { return shadersSupported; }
	bool AlternativeRender(vcg::GLW::DrawMode _dm, vcg::GLW::ColorMode _cm, vcg::GLW::TextureMode _tm);
	QList<QAction*> actions();

private:
	void initActionList();

	bool shadersSupported;
	QList<QAction*> actionList;
	int shaderPass;
	int totPass;
	QString shaderDir;
	RfxDialog *dialog;
	RfxShader *activeShader;

void Draw(MeshDocument *md, GLuint *program, QList<RfxSpecialAttribute*> *sal)
	{
		// Needed to be defined here for splatrender as long there is no "MeshlabCore" library.
		using namespace vcg;
		CMeshO *cm = &(md->mm()->cm);
		
		CMeshO::FaceIterator fi;
		glPushMatrix();
		glMultMatrix(cm->Tr);
		
		QListIterator<RfxSpecialAttribute*> sai = QListIterator<RfxSpecialAttribute*>(*sal);
		GLint *loc;
		int i=0;
		RfxSpecialAttribute* temp;
		while(sai.hasNext())
		{
			temp = sai.next();
			loc[i]=glGetAttribLocation(*program, temp->getTypeName());
			i++;
		}
		//glEnable (GL_BLEND); 
		
		cm->sfn=0;
		glBegin(GL_TRIANGLES);
		for(fi=cm->face.begin();fi!=cm->face.end();++fi)
			{
			sai.toFront();
			i=0;
			while(sai.hasNext())
			{
				temp = sai.next();
				temp->setValueByType(loc[i], (*fi).V(0));
				temp->setValueByType(loc[i], (*fi).V(1));
				temp->setValueByType(loc[i], (*fi).V(2));
				i++;
			}
		
			glNormal((*fi).V(0)->N());
			glVertex((*fi).cP(0));
			
			glNormal((*fi).V(1)->N());
			glVertex((*fi).cP(1));
			
			glNormal((*fi).V(2)->N());
			glVertex((*fi).cP(2));
			++cm->sfn;

			}		


		glEnd();
		
		glPopMatrix();
	}
	
};




#endif /* RENDERRFX_H */
