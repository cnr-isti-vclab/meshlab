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
		diagonal_percentual = 0.00;
		diagonale= 0.00;
	}

public:

	inline float getThreshold() 
	{
		return threshold;
	}
	
	void setDiagonale(float diag)
	{
		diagonale = diag;
		thresholdSB->setMaximum(diag);
	}

	void aggiornaPercentualeDiagonale()
	{
		diagonal_percentual = (threshold / diagonale) * 100;
		percdiag->setValue(diagonal_percentual);
	}

	void aggiornaTrashold()
	{
		threshold = (diagonal_percentual * diagonale) /100;
		thresholdSB->setValue(threshold);
	}


public slots:

	void on_thresholdSB_valueChanged(double threshold) 
	{
		if(threshold <= this->diagonale)
		{
			this->threshold = threshold;
			aggiornaPercentualeDiagonale();
		}
	}

	void on_percdiag_valueChanged(double pd)
	{
		diagonal_percentual = pd;
		aggiornaTrashold();
	}



private:
  // threshold value for refine
	double threshold;
	double diagonal_percentual;
	double diagonale;
};

#endif 
