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
/****************************************************************************
  History
sampledecorate.h,v
Revision 1.1  2007/12/13 00:33:55  cignoni
New small samples

****************************************************************************/
#ifndef SAMPLE_DECORATE_PLUGIN_H
#define SAMPLE_DECORATE_PLUGIN_H

#include <QObject>
#include <QAction>
#include <QList>

#include <common/interfaces.h>
#include "cubemap.h"
//#include "../../meshlab/mainwindow.h"

class SampleMeshDecoratePlugin : public QObject, public MeshDecorateInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshDecorateInterface)
  QString filterName(FilterIDType filter) const;
  QString filterInfo(QAction *) const;

  enum {
    DP_SHOW_CUBEMAPPED_ENV,
		};

private:
vcg::CICubeMap cm;

inline QString CubeMapPathParam() const { return  "MeshLab::Decoration::CubeMapPath" ; }

public:
     
	SampleMeshDecoratePlugin()
	{
    typeList << 
    DP_SHOW_CUBEMAPPED_ENV ;

    FilterIDType tt;
    foreach(tt , types()){
	      actionList << new QAction(filterName(tt), this);
    }
    QAction *ap;
    foreach(ap,actionList){
        ap->setCheckable(true);
    }
  }

	QList<QAction *> actions () const {return actionList;}
	QString basename;
	
	bool StartDecorate(QAction * /*mode*/, MeshModel &/*m*/, RichParameterSet * /*parent*/ par, GLArea * /*parent*/);
  void Decorate(QAction *a, MeshModel &m, GLArea *gla,QFont qf);
	void initGlobalParameterSet(QAction *, RichParameterSet &/*globalparam*/);	
		
};

#endif
