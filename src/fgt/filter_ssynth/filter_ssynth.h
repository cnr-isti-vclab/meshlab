/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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

#ifndef FILTER_SSYNTH_H
#define FILTER_SSYNTH_H



#include <QObject>
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include <meshlabplugins/io_x3d/io_x3d.h>

class FilterSSynth : public QObject, public MeshFilterInterface{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

    public:
            enum {CR_SSYNTH} ;
                    
            FilterSSynth();
            ~FilterSSynth(){};
    
            virtual QString filterName(FilterIDType filter) const;
            virtual QString filterInfo(FilterIDType filter) const;
            virtual int getRequirements(QAction *);
            virtual bool autoDialog(QAction *) {return true;}
            virtual void initParameterSet(QAction* filter,MeshModel &,RichParameterSet &){};
            virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
            virtual bool applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
            virtual bool applyFilter(QAction * /*filter */, MeshModel &, RichParameterSet & /*parent*/, vcg::CallBackPos *) { assert(0); return false;} ;
            virtual FilterClass getClass(QAction *);
            void setAttributes(CMeshO::VertexIterator &vi, CMeshO &m);
            bool openX3D(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent=0);
        private:
           // int &mask=0;
            QString ssynth(QString grammar,int seed);
        };
#endif // FILTER_SSYNTH_H
