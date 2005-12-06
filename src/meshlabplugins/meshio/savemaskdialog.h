/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
 Revision 1.2  2005/12/06 15:54:59  fmazzant
 update dialog export obj

 Revision 1.1  2005/12/02 17:40:26  fmazzant
 added dialog obj exporter.


*****************************************************************************/

#include "ui_savemask.h"
struct Mask
{
	int vertexs;//salva i vertici 1, non li salva 0
	int faces;//salva le facce 1, non li salva 0
	int texture;//salva le texture 1, non li salva 0
	int normal;//salva le normali 1, non li salva 0
	int binary;//salva in binario 1, salva in ascii 0
	int colorV;//salva colore vertici 1, non li salva 0
	const static int args = 6;
};

class SaveMaskDialog : public QDialog
{
	Q_OBJECT
public:
	SaveMaskDialog(QWidget *parent);
	bool ReadMask();

	static Mask GetMask();
	static bool WriteMask(Mask *mask);
	static int MaskToInt(Mask *mask); //converte la Mask nell'intero definito come maschera.

private slots:
	void SlotOkButton();
	void SlotCancelButton();

private:
	Ui::Dialog ui;	
};