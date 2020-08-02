/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#ifndef RICHPARAMETERLISTDIALOG_H
#define RICHPARAMETERLISTDIALOG_H

#include <QDialog>

#include "../../common/filter_parameter/rich_parameter_list.h"
#include "richparameterlistframe.h"

// This class provide a modal dialog box for asking a generic parameter set
// It can be used by anyone needing for some values in a structured form and having some integrated help
class RichParameterListDialog: public QDialog
{
	Q_OBJECT
public:
	RichParameterListDialog(QWidget *p, RichParameterList *_curParSet, QString title=QString(), MeshDocument *_meshDocument = 0);
	~RichParameterListDialog();

	RichParameterList *curParSet;
	RichParameterListFrame *stdParFrame;

	void createFrame();

	public slots:
	void getAccept();
	void toggleHelp();

	//reset the values on the gui back to the ones originally given to the dialog
	void resetValues();

private:
	MeshDocument *meshDocument;

};

#endif // RICHPARAMETERLISTDIALOG_H
