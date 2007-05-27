#ifndef DIALOGSLICE_H
#define DIALOGSLICE_H

#include <QWidget>
#include <QtGui/QDockWidget>
#include "ui_slicedialog.h"

class dialogslice : public QDockWidget
{
	Q_OBJECT

public:
	dialogslice(QWidget *parent = 0);
	~dialogslice();
	
	inline int getPlaneNumber() {return numPlane;}
	inline float getDistance() {return (distance*distanceRange);}
	inline bool getDefaultTrackball(){return defaultTrackball;}
	inline bool getdistanceDefault(){return distanceDefault;}
	inline bool getRestoreDefalut(){return restoreDefalut;}
	inline void setRestoreDefalut(bool in){ 
		restoreDefalut=in;}
	inline bool getExportOption(){
	    return multi_sigleExportFile;
	}
	inline void setDefaultDistance(float f){
	defaultdistance=f;
	}
		
	inline void setDistanceRange(float dRange){
		this->distanceRange=dRange/100;
		ui.labelDistanceRange->setText(QString("Distance range from 0 to "+ QString::number(dRange)));
		
	}
	
Q_SIGNALS:
	void exportMesh(); 
	void Update_glArea();
     void RestoreDefault();
private:
	Ui::dialogsliceClass ui;
	int numPlane; //numeber of plane
	float distance; 
	float distanceRange;
	float defaultdistance;
    QWidget* parent;
	bool distanceDefault; // enable/disable distance 
	bool multi_sigleExportFile; 
	bool  defaultTrackball;
	bool restoreDefalut;

private slots:
	void on_Exporter_MultiFile_clicked();
	void on_Exporter_singleFile_clicked();

	
	void on_SliderPlaneDistance_sliderMoved(int);
	void on_Update_Val_clicked();
	void on_diasbledistance_toggled(bool);
	void on_ExportButton_clicked();
	void on_DefultButton_clicked();
	
	
	void on_spinBoxPlane_valueChanged(int);
	
	void on_on_slideTrackBall_clicked(bool);
	void on_DefaultTrackball_clicked(bool);


	

	
	
};

#endif

