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
 Revision 1.1  2006/01/30 06:32:33  buzzelli
 added a dialog used to select type of data being imported


 ****************************************************************************/
#ifndef __CHANGEMASK_DIALOG_H
#define __CHANGEMASK_DIALOG_H

#include "ui_changeMaskDialog.h"

class ChangeMaskDialog : public QDialog, Ui::ChangeMaskDialog
{
	Q_OBJECT

public:
	ChangeMaskDialog(int capability = 0, QWidget *parent = 0);
	
	void	setMaskCapability();
	int		getNewMask();

private slots:
	void	slotOkButtonPressed();
	void	slotSelectionAllButtonPressed();
	void slotSelectionNoneButtonPressed();

private:
	int		capability;
	int		mask;
};

#endif // __CHANGEMASK_DIALOG_H