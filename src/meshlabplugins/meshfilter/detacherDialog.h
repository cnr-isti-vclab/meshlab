#ifndef DETACHER_DIALOG_H
#define DETACHER_DIALOG_H

// for options on decimator
#include <QDialog>
#include "ui_detacherDialog.h"
#include <vcg/math/histogram.h>

class DetacherDialog : public QDialog, Ui::DetacherDialog {

Q_OBJECT

public:
  DetacherDialog() : QDialog()
  {
   setupUi( this );
		threshold = 0.00;
		diagonal_percentual = 0.00;
		diagonale= 0.00;
		perc = 0.0f;
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

	void setHistogram(vcg::Histogram<float> *hi)
	{
		histo = hi;
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

	void aggiornaPercentile()
	{
		threshold = histo->Percentile(perc);
		thresholdSB->setValue(threshold);
		diagonal_percentual = (threshold / diagonale) * 100;
		percdiag->setValue(diagonal_percentual);
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


void on_percentileSB_valueChanged(int pd)
{
		perc = 1.0f - (pd * 0.01);
		aggiornaPercentile();
}

private:
	double threshold;
	double diagonal_percentual;
	double diagonale;
	vcg::Histogram<float> *histo;
	float perc;
};

#endif 
