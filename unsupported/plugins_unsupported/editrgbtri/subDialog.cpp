/****************************************************************************
 * Rgb Triangulations Plugin                                                 *
 *                                                                           *
 * Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
 * Copyright(C) 2007                                                         *
 * DISI - Department of Computer Science                                     *
 * University of Genova                                                      *
 *                                                                           *
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
 ****************************************************************************/

#include "subDialog.h"
#include <QPushButton>

namespace rgbt
{

SubDialog::SubDialog()
{
	setupUi(this);
	QPushButton* mbButton = new QPushButton(tr("&Modified Butterfly"));
	//mbButton->setDefault(false);

	QPushButton* lButton = new QPushButton(tr("&Loop"));
	//lButton->setDefault(true);
	//lButton->setCheckable(true);
	//lButton->setAutoDefault(true);
	//lButton->setFocus();

	buttonBox->addButton(lButton, QDialogButtonBox::AcceptRole);
	buttonBox->addButton(mbButton, QDialogButtonBox::RejectRole);	
}

SubDialog::~SubDialog()
{
}

}
