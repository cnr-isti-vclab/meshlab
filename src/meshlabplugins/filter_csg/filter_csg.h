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

#ifndef FILTERCSGPLUGIN_H
#define FILTERCSGPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <common/plugins/interfaces/filter_plugin_interface.h>


//FILE _iob[] = { *stdin, *stdout, *stderr };
//extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

class FilterCSG : public QObject, public FilterPluginInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_INTERFACE_IID)
	Q_INTERFACES(FilterPluginInterface)

    enum {
        CSG_OPERATION_INTERSECTION  = 0,
        CSG_OPERATION_UNION         = 1,
        CSG_OPERATION_DIFFERENCE    = 2
    };

public:
    enum {FP_CSG};

    FilterCSG();
    ~FilterCSG() {};

    QString pluginName() const;
    virtual QString filterName(FilterIDType filter) const;
    virtual QString filterInfo(FilterIDType filter) const;

    virtual bool autoDialog(QAction *) { return true; }

    virtual void initParameterList(const QAction*, MeshDocument &, RichParameterList &);

    virtual bool applyFilter(const QAction*, MeshDocument &, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList &, vcg::CallBackPos *);

    virtual FilterClass getClass(const QAction *) const { return FilterPluginInterface::FilterClass( FilterPluginInterface::Layer + FilterPluginInterface::Remeshing ); }
    FILTER_ARITY filterArity(const QAction*) const {return FIXED;}
};


#endif
