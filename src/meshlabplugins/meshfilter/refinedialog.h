#ifndef REFINE_DIALOG_H
#define REFINE_DIALOG_H

// for options on refine and decimator
#include <QDialog>
#include "ui_refine.h"

class RefineDialog : public QDialog, Ui::Dialog {

Q_OBJECT

public:
  RefineDialog() : QDialog()
  {
    setupUi( this );
  }
public slots:

void on_refineSelectedCB_stateChanged(int selected) {
	
  this->selected = selected;

	
}

	void on_thresholdSB_valueChanged(double threshold) {
		this->threshold = threshold;
	}

	inline double getThreshold() {
		return threshold;
	}

	inline bool isSelected() {
		return selected;
	}


private:
  // affect only selected vertices ?
  int selected;
  // threshold value for refine
	int threshold;

};

#endif //REFINE_DIALOG_H
