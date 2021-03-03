/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2008                                                \/)\/    *
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
$Log: editmeasure.cpp,v $
****************************************************************************/

#include <meshlab/glarea.h>
#include "edit_measure.h"
#include <wrap/qt/gl_label.h>

#include <QTextStream>

using namespace vcg;

EditMeasurePlugin::EditMeasurePlugin()
:rubberband(Color4b(255, 170, 85, 11)), measureband(Color4b(85, 170, 255, 11)), was_ready(false)
{
	measures.clear();
	mName = 0;
}

const QString EditMeasurePlugin::Info()
{
	return tr("Allows one to measure distances between points of a model");
}

void EditMeasurePlugin::mousePressEvent(QMouseEvent *, MeshModel &, GLArea * gla)
{
  if(rubberband.IsReady())
  {
    rubberband.Reset();
  }
  gla->update();
}

void EditMeasurePlugin::mouseMoveEvent(QMouseEvent * event, MeshModel &, GLArea * gla)
{
  rubberband.Drag(event->pos());
  gla->update();
}

void EditMeasurePlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &, GLArea * gla)
{
  rubberband.Pin(event->pos());
  gla->update();
}

void EditMeasurePlugin::decorate(MeshModel & m, GLArea * gla,QPainter* p)
{
  float measuredDistance = -1.0;

  rubberband.Render(gla);

  if(rubberband.IsReady())
  {
    Point3f a,b;
    rubberband.GetPoints(a,b);
	measuredDistance = Distance(a, b);

    suspendEditToggle();
	rubberband.Reset();

	measure newM;
	newM.ID = QString("M") + QString::number(mName++);
	newM.startP = a;
	newM.endP = b;
	newM.length = measuredDistance;
	measures.push_back(newM);

	this->log(GLLogStream::FILTER, "Distance %s: %f", newM.ID.toStdString().c_str(), measuredDistance);
  }

  for (size_t mind = 0; mind<measures.size(); mind++)
  {
	  rubberband.RenderLine(gla, measures[mind].startP, measures[mind].endP);
	  vcg::glLabel::render(p, measures[mind].endP, QString("%1: %2").arg(measures[mind].ID).arg(measures[mind].length));
  }

  QString instructions;
  instructions = "C to clear, P to print, S to save";

  QString savedmeasure = "<br>";
  for (size_t mind = 0; mind<measures.size(); mind++)
  {
	  savedmeasure.append(QString("%1 - %2<br>").arg(measures[mind].ID).arg(measures[mind].length));
  }

  if (measures.size() == 0)
	this->realTimeLog("Point to Point Measure", m.shortName(),
		" -- "
		);
  else
	this->realTimeLog("Point to Point Measure", m.shortName(),
		(instructions + savedmeasure).toStdString().c_str()
		);

  assert(!glGetError());
}

void EditMeasurePlugin::keyReleaseEvent(QKeyEvent *e, MeshModel &mod, GLArea *gla)
{
	if (e->key() == Qt::Key_C) // clear
	{
		measures.clear();
		rubberband.Reset();
		gla->update();
	}

	if (e->key() == Qt::Key_P) // print
	{
		this->log(GLLogStream::FILTER, "------- Distances -------");
		this->log(GLLogStream::FILTER, "ID: Dist [pointA][pointB]");
		for (size_t mind = 0; mind<measures.size(); mind++)
		{
			this->log(GLLogStream::FILTER, "%s: %f [%f,%f,%f][%f,%f,%f]", measures[mind].ID.toStdString().c_str(), measures[mind].length,
				measures[mind].startP[0], measures[mind].startP[1], measures[mind].startP[2], measures[mind].endP[0], measures[mind].endP[1], measures[mind].endP[2]);
		}
		this->log(GLLogStream::FILTER, "-------------------------");
	}

	if (e->key() == Qt::Key_S) // save
	{
		QString saveFileName = mod.fullName();
		saveFileName.truncate(saveFileName.lastIndexOf("."));
		saveFileName += QString("_measures.txt");

		QFile openFile(saveFileName);

		if (openFile.open(QIODevice::ReadWrite))
		{
			QTextStream openFileTS(&openFile);

			openFileTS << "-------MEASUREMENT DATA---------" << "\n\n";

			openFileTS << mod.shortName().toStdString().c_str() << "\n\n";

			openFileTS << "ID : Dist [pointA][pointB]" << "\n";
			for (size_t mind = 0; mind<measures.size(); mind++)
			{
				openFileTS << measures[mind].ID.toStdString().c_str() << " : " << measures[mind].length << " [" <<	
					measures[mind].startP[0] << ", " << measures[mind].startP[1] << ", " << measures[mind].startP[2] << "] [" << 
					measures[mind].endP[0] << ", " << measures[mind].endP[1] << ", " << measures[mind].endP[2] << "] \n";
			}

			openFile.close();
		}
		else
		{
			this->log(GLLogStream::WARNING, "- cannot save measures to file -");
		}
	}
}

bool EditMeasurePlugin::startEdit(MeshModel &/*m*/, GLArea * gla, MLSceneGLSharedDataContext* /*cont*/)
{
  gla->setCursor(QCursor(QPixmap(":/images/cur_measure.png"),15,15));
  connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );

  measures.clear();
  mName = 0;
  rubberband.Reset();

  return true;
}

void EditMeasurePlugin::endEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/)
{
  rubberband.Reset();
}
