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

#ifndef FILTER_CREATE_H
#define FILTER_CREATE_H
#include <common/plugins/interfaces/filter_plugin_interface.h>

class FilterCreate : public QObject, public FilterPluginInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_INTERFACE_IID)
	Q_INTERFACES(FilterPluginInterface)

  public:
    enum {
    CR_BOX,
        CR_ANNULUS,
        CR_SPHERE,
        CR_SPHERE_CAP,
        CR_RANDOM_SPHERE,
        CR_ICOSAHEDRON,
        CR_DODECAHEDRON,
        CR_TETRAHEDRON,
        CR_OCTAHEDRON,
        CR_CONE,
        CR_TORUS,
		CR_FITPLANE,
  } ;

  FilterCreate();
  QString pluginName() const;

  QString filterName(FilterIDType filter) const;
  QString filterInfo(FilterIDType filter) const;
  FilterClass getClass(const QAction*) const;
  void initParameterList(const QAction*, MeshModel &/*m*/, RichParameterList & /*parent*/);
  bool applyFilter(const QAction* filter, MeshDocument &md, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
  QString filterScriptFunctionName(FilterIDType filterID);
  FILTER_ARITY filterArity(const QAction *) const {return NONE;}
};

#endif
