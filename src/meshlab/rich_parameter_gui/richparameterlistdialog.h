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

#include "../../common/parameters/rich_parameter_list.h"
#include "richparameterlistframe.h"

/**
 * @brief This class provide a modal dialog box for asking a generic parameter list
 * It can be used by anyone needing for some values in a structured form and having some integrated help
 *
 * When the user clicks ok, the dialog will apply the modified values in the RichParameterList given
 * as input in the Dialog constructor.
 * Used by some I/O and Edit plugins
 *
 */
class RichParameterListDialog: public QDialog
{
	Q_OBJECT
public:
	RichParameterListDialog(QWidget *p, RichParameterList& curParList, const QString& title=QString());
	~RichParameterListDialog();

	void createFrame();

public slots:
	void getAccept();
	void toggleHelp();

	//reset the values on the gui back to the ones originally given to the dialog
	void resetValues();

private:
	RichParameterList& curParList;
	RichParameterListFrame *stdParFrame;

};

#endif // RICHPARAMETERLISTDIALOG_H
