/****************************************************************************
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

#ifndef FILTER_PHOTOSYNTH_PLUGIN_H
#define FILTER_PHOTOSYNTH_PLUGIN_H

#include <common/interfaces.h>
#include "synthData.h"


class FilterPhotosynthPlugin : public QObject, public MeshFilterInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshFilterInterface)

public:
  enum { FP_IMPORT_PHOTOSYNTH };

  FilterPhotosynthPlugin();

  virtual QString pluginName(void) const { return "PhotosynthImporterPlugin"; }

  QString filterName(FilterIDType filter) const;
  QString filterInfo(FilterIDType filter) const;
  void initParameterSet(QAction *action,MeshModel &/*m*/, RichParameterSet &/*parent*/);
  bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet &/*parent*/, vcg::CallBackPos *cb);
  int postCondition(QAction*) const { return MeshModel::MM_UNKNOWN; }
  FilterClass getClass(QAction *a);
  QString filterScriptFunctionName(FilterIDType filterID);
  void outputToFile(QTextStream &out, Shotf &s, Image &img, CameraParameters &cam);
};

#endif /* FILTER_PHOTOSYNTH_PLUGIN_H */
