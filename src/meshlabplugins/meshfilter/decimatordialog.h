#ifndef DECIMATOR_DIALOG_H
#define DECIMATOR_DIALOG_H

// for options on decimator
#include <QDialog>
#include "ui_decimator.h"
#include <math.h>
class DecimatorDialog : public QDialog, Ui::DecimatorDialog {

Q_OBJECT

public:
  DecimatorDialog() : QDialog()
  {
    setupUi( this );
		step = 10;
	}
public slots:
	void on_stepSB_valueChanged(double step) {
		this->step = floor(step);
	}
public:

	inline int getStep() {
		return step;
	}

	
private:
  int step;

};

#endif //REFINE_DIALOG_H
