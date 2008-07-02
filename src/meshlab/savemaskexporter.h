/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
 Revision 1.4  2007/11/25 09:48:39  cignoni
 Changed the interface of the io filters. Now also a default bit set for the capabilities has to specified

 Revision 1.3  2007/03/26 08:25:10  zifnab1974
 added eol at the end of the files

 Revision 1.2  2007/02/25 21:26:22  cignoni
 corrected include path

 Revision 1.1  2006/01/26 18:39:19  fmazzant
 moved mask dialog exporter from mashio to meshlab

 Revision 1.11  2006/01/19 15:59:00  fmazzant
 moved savemaskexporter to mainwindows

 Revision 1.10  2006/01/19 12:45:00  fmazzant
 deleted SaveMaskExporterDialog::Initialize()

 Revision 1.9  2006/01/19 09:25:28  fmazzant
 cleaned code & deleted history log

 Revision 1.8  2006/01/17 13:48:54  fmazzant
 added capability mask on export file format

 Revision 1.7  2006/01/17 09:08:36  fmazzant
 update Check Button -> Radio Button and
 connect slot for AllButton and NoneButton

 Revision 1.6  2006/01/16 23:53:22  fmazzant
 bux-fix MeshModel &m -> MeshModel *m

 ****************************************************************************/

#ifndef __VCGLIB_SAVEMASK_EXPORT
#define __VCGLIB_SAVEMASK_EXPORT

#include <wrap/io_trimesh/io_mask.h>

#include "meshmodel.h"
#include "filterparameter.h"
#include "stdpardialog.h"
#include "ui_savemaskexporter.h"
//
// Each file format exposes:
//  a capability bit vector with all the things that it can save (the bits are the one indicated in the IOM_XXX bit mask) ; 
//  a default bit vector that indicate what things are saved by defaults (eg. by default normals and flags are not saved in ply)
//  a vector of optional parameters (like for example a bool for ascii/binary, a bool for choosing what  
// This dialog allow to select what things actually save. 
// 
// 
// 

class SaveMaskExporterDialog : public QDialog
{
	Q_OBJECT
public:
	SaveMaskExporterDialog(QWidget *parent, MeshModel *m, int capability, int defaultBits, FilterParameterSet *par);
	
	void InitDialog();
	void SetTextureName();
	int GetNewMask();
	void SetMaskCapability();

private slots:
	void SlotOkButton();
	void SlotCancelButton();
	void SlotRenameTexture();
	void SlotSelectionTextureName();
	void SlotSelectionAllButton();
	void SlotSelectionNoneButton();

private:
	Ui::MaskExporterDialog ui;
	MeshModel *m;
	int mask;
	int type;
	int capability;
	int defaultBits;
	FilterParameterSet *par;
	StdParFrame *stdParFrame;
};//end class

#endif
