#ifndef DETACHER_DIALOG_H
#define DETACHER_DIALOG_H

// for options on decimator
#include <QDialog>
#include "ui_GenericELDialog.h"
#include <vcg/math/histogram.h>

class GenericELDialog : public QDialog, Ui::DetacherDialog {

Q_OBJECT

public:
  GenericELDialog() : QDialog()
  {
   setupUi( this );
//		threshold = 0.00;
	//	diagonal_percentual = 0.00;
		diagonale= 0.00;
		perc = 0.0f;
		selected=false;
	}

	~GenericELDialog()
	{
		delete histo;
	}

public:

	inline float getThreshold() 
	{
		return thresholdSB->value();
	}
	
	inline bool getSelected() 
	{
		return selected;
	}

	void setDiagonale(float diag)
	{
		diagonale = diag;
		thresholdSB->setMaximum(diag);
	}

  void setStartingPerc(float perc)
	{
    percdiag->setValue(perc);
    updateAbsoluteThr();
	}

	void setHistogram(vcg::Histogram<float> *hi)
	{
		histo = hi;
	}

	void updatePercDiagThr()
	{
		percdiag->setValue((thresholdSB->value()/ diagonale) * 100);
	}

	void updateAbsoluteThr()
	{
		thresholdSB->setValue((percdiag->value() * diagonale) /100);
	}

	void updatePercentile()
	{
		/*threshold = histo->Percentile(perc);
		thresholdSB->setValue(threshold);
		diagonal_percentual = (threshold / diagonale) * 100;
		percdiag->setValue(diagonal_percentual);*/
	}

public slots:

	void on_thresholdSB_valueChanged(double /* threshold */) 
	{
		updatePercDiagThr();
	}

	void on_percdiag_valueChanged(double /* pd */)
	{
		//diagonal_percentual = pd;
		updateAbsoluteThr();
	}


void on_percentileSB_valueChanged(int pd)
{
		perc = 1.0f - (pd * 0.01);
		updatePercentile();
}

	void on_selctedCB_stateChanged(int pd) {
	
	if (pd == Qt::Checked)
		selected= true;
	else
		selected = false;
	}

private:
	//double threshold;
	//double diagonal_percentual;
	double diagonale;
	vcg::Histogram<float> *histo;
	float perc;
	bool selected;
};

#endif 
