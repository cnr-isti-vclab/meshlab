#ifndef QUALITYMAPPERDIALOG_H
#define QUALITYMAPPERDIALOG_H

#include <QDialog>
#include <vcg/math/histogram.h>
#include "ui_qualitymapperdialog.h"

class QualityMapperSettings
{
public:
	//int percentile;
	int range;
	float meshMaxQ;
	float meshMidQ;
	float meshMinQ;
	float manualMaxQ;
	float manualMidQ;
	float manualMinQ;
	//float histoMaxQ;
	//float histoMinQ;
	//bool useManual;

	QualityMapperSettings()
	{
		//percentile=20;
		//range=10000;
		range=500;
		meshMaxQ=meshMinQ=meshMidQ=manualMaxQ=manualMinQ=manualMidQ=0.0f;
		//useManual=false;
	};
};

class QualityMapperDialog : public QDialog
{
	Q_OBJECT

public:
	QualityMapperDialog(QWidget *parent = 0);
	~QualityMapperDialog();
	
	void setValues(const QualityMapperSettings& qms);
	QualityMapperSettings getValues();

	void initEqualizerHistogram(vcg::Histogramf *h);
	void drawCartesianChartBasics(int maxRoundedY, vcg::Histogramf *h);

private:
	Ui::QualityMapperDialogClass ui;
	QualityMapperSettings _settings;
	QGraphicsScene _equalizerScene;

	int border;
	int chartRectangleThickness;
	int leftBorder;
    int rightBorder;
    int upperBorder;
    int lowerBorderForCartesians;


	private slots:

};

#endif // QUALITYMAPPERDIALOG_H
