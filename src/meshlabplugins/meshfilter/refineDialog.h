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
Revision 1.2  2006/01/31 14:46:39  mariolatronico
added license and log variable


*/



#ifndef REFINE_DIALOG_H
#define REFINE_DIALOG_H

// for options on refine and decimator
#include <QDialog>
#include "ui_refineDialog.h"

class RefineDialog : public QDialog, Ui::Dialog {

Q_OBJECT

public:
  RefineDialog() : QDialog()
  {
    setupUi( this );
		threshold = 0.00;
		selected = false;
  }
public slots:

void on_refineSelectedCB_stateChanged(int selected) {
	
	if (selected == Qt::Checked)
		this->selected = true;
	else
		this->selected = false;

	
}

	void on_thresholdSB_valueChanged(double threshold) {
		this->threshold = threshold;
	}

public:

	inline double getThreshold() {
		return threshold;
	}

	inline bool isSelected() {
		return selected;
	}


private:
  // affect only selected vertices ?
  bool selected;
  // threshold value for refine
	double threshold;

};

#endif //REFINE_DIALOG_H
