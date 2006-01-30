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
