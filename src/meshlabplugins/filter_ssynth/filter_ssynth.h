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

class FilterSSynth : public QObject,public MeshIOInterface, public MeshFilterInterface{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)
        Q_INTERFACES(MeshIOInterface)
    public:
            enum {CR_SSYNTH} ;

            FilterSSynth();
            ~FilterSSynth(){}
            virtual QString filterName(FilterIDType filter) const;
            virtual QString filterInfo(FilterIDType filter) const;
            virtual int getRequirements(QAction *);
            virtual void initParameterSet(QAction* filter,MeshModel &,RichParameterSet &){};
            virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
            virtual bool applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
            virtual FilterClass getClass(QAction* filter);
            void setAttributes(CMeshO::VertexIterator &vi, CMeshO &m);
            static void openX3D(const QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb, QWidget *parent=0);
            virtual int postCondition(QAction* filter) const;
                        QList<Format> importFormats() const;
                        QList<Format> exportFormats() const;

                        virtual void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const;
                    void initPreOpenParameter(const QString &formatName, const QString &filename, RichParameterSet &parlst);
                        bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet & par, vcg::CallBackPos *cb=0, QWidget *parent=0);
                        bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent);

private:
         QString ssynth(QString grammar,int maxdepth,int seed,vcg::CallBackPos *cb);
         QString GetTemplate(int sphereres);
             void ParseGram(QString* grammar,int max,QString pattern);
                int seed;
               QString renderTemplate;
               QString spheres[6];
        };
#endif // FILTER_SSYNTH_H
