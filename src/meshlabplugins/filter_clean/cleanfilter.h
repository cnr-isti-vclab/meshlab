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

 $Log$
 Revision 1.8  2007/10/02 08:13:39  cignoni
 New filter interface. Hopefully more clean and easy to use.

 Revision 1.7  2007/04/16 09:25:28  cignoni
 ** big change **
 Added Layers managemnt.
 Interfaces are changing again...

 Revision 1.6  2007/02/08 23:46:16  pirosu
 merged srcpar and par in the GetStdParameters() function

 Revision 1.5  2007/01/11 19:52:25  pirosu
 fixed bug for QT 4.1.0/dotnet2003
 removed the request of the window title to the plugin. The action description is used instead.

 Revision 1.4  2006/12/27 21:41:58  pirosu
 Added improvements for the standard plugin window:
 split of the apply button in two buttons:ok and apply
 added support for parameters with absolute and percentage values

 Revision 1.3  2006/12/13 17:37:27  pirosu
 Added standard plugin window support


 Revision 1.2  2006/11/29 00:59:15  cignoni
 Cleaned plugins interface; changed useless help class into a plain string

 Revision 1.1  2006/11/07 09:09:27  cignoni
 First Working release, moved in from epoch svn

 Revision 1.1  2006/01/20 13:03:27  cignoni
 *** empty log message ***

 *****************************************************************************/
#ifndef EXTRAIOPLUGINV3D_H
#define EXTRAIOPLUGINV3D_H

#include <QObject>
#include <common/interfaces.h>

class CleanFilter : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

		public:
	/* naming convention :
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum { 
    FP_BALL_PIVOTING, 
    FP_REMOVE_ISOLATED_COMPLEXITY, 
    FP_REMOVE_ISOLATED_DIAMETER, 
    FP_REMOVE_WRT_Q,
    FP_ALIGN_WITH_PICKED_POINTS,
    FP_SELECTBYANGLE,
    FP_REMOVE_TVERTEX_FLIP,
    FP_REMOVE_TVERTEX_COLLAPSE,
    FP_REMOVE_FOLD_FACE,
    FP_REMOVE_DUPLICATE_FACE,
		FP_MERGE_CLOSE_VERTEX
  } ;
	
	/* default values for standard parameters' values of the plugin actions */
    float maxDiag1;
    float maxDiag2;
    int minCC;
    float val1;


	CleanFilter();
	~CleanFilter();

	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;
MeshFilterInterface::Remeshing;

  virtual FilterClass getClass(QAction *);
  virtual int getRequirements(QAction *);

  virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
  virtual bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
	
};






#endif
