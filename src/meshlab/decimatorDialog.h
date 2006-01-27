#ifndef DECIMATOR_DIALOG_H
#define DECIMATOR_DIALOG_H

// for options on decimator
#include <QDialog>
#include "ui_decimator.h"
#include <math.h>
#include <vcg/space/point3.h>

class DecimatorDialog : public QDialog, Ui::DecimatorDialog {

	Q_OBJECT

public:
	DecimatorDialog() : QDialog()
	{
		setupUi( this );
		Xstep = 10;
		Ystep = 10;
		Zstep = 10;
	}

public:

	inline int getXStep() {
		return Xstep;
	}

	inline int getYStep() {
		return Ystep;
	}

	inline int getZStep() {
		return Zstep;
	}

	void aggiornaLatoX()
	{
		TXL = XL / Xstep;
		XedgeLength->setValue( TXL );
	}

	void aggiornaLatoY()
	{
		TYL = YL / Ystep;
		YedgeLength->setValue(TYL);
	}	

	void aggiornaLatoZ()
	{
		TZL = ZL / Zstep;
		ZedgeLength->setValue(TZL);
	}

	void aggiornaXStep()
	{
		Xstep = floor(XL / XedgeLength->value());
		stepXSB->setValue(Xstep);
	}

	void aggiornaYStep()
	{
		Ystep = floor(YL / YedgeLength->value());
		stepYSB->setValue(Ystep);
	}
	void aggiornaZStep()
	{
		Zstep = floor(ZL / ZedgeLength->value());
		stepZSB->setValue(Zstep);
	}

	void setBboxEdge(vcg::Point3f min, vcg::Point3f max)
	{
		XL = max[0] - min[0]; XedgeLength->setMaximum(XL);
		YL = max[1] - min[1]; YedgeLength->setMaximum(YL);
		ZL = max[2] - min[2]; ZedgeLength->setMaximum(ZL);
		aggiornaLatoX();
		aggiornaLatoY();
		aggiornaLatoZ();
	}

	
public slots:
	void on_stepXSB_valueChanged(double val) 
	{
		Xstep = floor(val);
		aggiornaLatoX();
	}

	void on_stepYSB_valueChanged(double val) 
	{
		Ystep = floor(val);
		aggiornaLatoY();
	}

	void on_stepZSB_valueChanged(double val) 
	{
		Zstep = floor(val);
		aggiornaLatoZ();
	}

	void on_XedgeLength_valueChanged(double pd)
	{
		TXL = pd;
		aggiornaXStep();
	}

	void on_YedgeLength_valueChanged(double pd)
	{
		TYL = pd;
		aggiornaYStep();
	}

	void on_ZedgeLength_valueChanged(double pd)
	{
		TZL = pd;
		aggiornaZStep();
	}


private:
	int Xstep,Ystep,Zstep;
	double XL,YL,ZL; //lenght of the edge of the bounding box
	double TXL,TYL,TZL;
};

#endif 
