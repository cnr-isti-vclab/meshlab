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

#include "../../meshlab/meshmodel.h"
#include "ui_savemaskexporter.h"

class SaveMaskExporterDialog : public QDialog
{
	Q_OBJECT
public:
	SaveMaskExporterDialog(QWidget *parent,MeshModel *m,int type,int capability);

	void InitDialog();
	void SetWindowTitle();
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
};//end class

namespace vcg {
namespace tri {
namespace io {
	
	class SaveMaskToExporter
	{
	public:	

		enum FileType
		{
			_OBJ, //0
			_PLY, //1
			_OFF, //2
			_STL, //3
			_3DS  //4
		};

		inline static int GetMaskToExporter(MeshModel *m,int type,int capability)
		{
			SaveMaskExporterDialog dialog(new QWidget(),m,type,capability);
			dialog.exec();
			int newmask = dialog.GetNewMask();
			dialog.close();
			return newmask;
		}
	};
} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif