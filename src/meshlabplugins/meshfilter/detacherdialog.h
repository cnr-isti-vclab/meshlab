#ifndef DETACHER_DIALOG_H
#define DETACHER_DIALOG_H

// for options on decimator
#include <QDialog>
#include "ui_detacher.h"

class DetacherDialog : public QDialog, Ui::DetacherDialog {

Q_OBJECT

public:
  DetacherDialog() : QDialog()
  {
   setupUi( this );
		threshold = 0.00;
	}
public slots:
	void on_thresholdSB_valueChanged(double threshold) {
		this->threshold = threshold;
	}
public:

	inline double getThreshold() {
		return threshold;
	}
	
private:
  // threshold value for refine
	double threshold;
};

#endif 
