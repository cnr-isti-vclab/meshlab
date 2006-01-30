#ifndef DECIMATOR_DIALOG_H
#define DECIMATOR_DIALOG_H

// for options on decimator
#include <QDialog>
#include "ui_decimatorDialog.h"
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
		return stepXSB->value();
	}

	inline int getYStep() {
		return stepYSB->value();
	}

	inline int getZStep() {
		return stepZSB->value();;
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
		float app;
		if(XL>YL)app= XL; else app=YL;
		if(app<ZL)app=ZL;
		XedgeLength->setValue(app/10);
		YedgeLength->setValue(app/10);
		ZedgeLength->setValue(app/10);
				
		aggiornaLatoX();
		aggiornaLatoY();
		aggiornaLatoZ();
	}


public slots:
	void on_stepXSB_valueChanged(double val) 
	{
		if(stepXSB->hasFocus())
		{
			Xstep = floor(val);
			if(Uniform->checkState () ==  2)
			{
				stepYSB->setValue(Xstep);
				stepZSB->setValue(Xstep);
			}
			aggiornaLatoX();
		}
	}

	void on_stepYSB_valueChanged(double val) 
	{
		if(stepYSB->hasFocus())
		{
			Ystep = floor(val);
			if(Uniform->checkState () ==  2)
			{
				stepXSB->setValue(Ystep);
				stepZSB->setValue(Ystep);
			}
			aggiornaLatoY();
		}
	}

	void on_stepZSB_valueChanged(double val) 
	{
		if(stepZSB->hasFocus())
		{
			Zstep = floor(val);
			if(Uniform->checkState () ==  2)
			{
				stepYSB->setValue(Zstep);
				stepXSB->setValue(Zstep);
			}
			aggiornaLatoZ();
		}
	}

	void on_XedgeLength_valueChanged(double pd)
	{
		if(XedgeLength->hasFocus())
		{
			TXL = pd;
			if(Uniform->checkState () ==  2)
			{
				YedgeLength->setValue(TXL);
				aggiornaYStep();
				ZedgeLength->setValue(TXL);
				aggiornaZStep();
			}
			aggiornaXStep();
		}
	}

	void on_YedgeLength_valueChanged(double pd)
	{
		if(YedgeLength->hasFocus())
		{
			TYL = pd;
			if(Uniform->checkState () ==  2)
			{
				XedgeLength->setValue(TYL);
				aggiornaXStep();
				ZedgeLength->setValue(TYL);
				aggiornaZStep();
			}
			aggiornaYStep();
		}
	}

	void on_ZedgeLength_valueChanged(double pd)
	{
		if(ZedgeLength->hasFocus())
		{
			TZL = pd;
			if(Uniform->checkState () ==  2)
			{
				YedgeLength->setValue(TZL);
				aggiornaYStep();
				XedgeLength->setValue(TZL);
				aggiornaXStep();
			}
			aggiornaZStep();
		}
	}


private:
	int Xstep,Ystep,Zstep;
	double XL,YL,ZL; //lenght of the edge of the bounding box
	double TXL,TYL,TZL;
};

#endif 
