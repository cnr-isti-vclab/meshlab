#ifndef SLICEDIALOG_H
#define SLICEDIALOG_H

#include <QDialog>
#include "ui_slicedialog.h"

class Slicedialog : public QDialog
{
	Q_OBJECT

public:
	Slicedialog(QWidget *parent = 0);
	~Slicedialog();
	inline int getPlaneNumber() {return numPlane;}
	inline int getDistance() {return distance;}
	inline bool getDefaultTrackball(){return defaultTrackball;}
	inline bool getdistanceDefault(){return distanceDefault;}
	inline bool getRestoreDefalut(){return restoreDefalut;}
	inline void setRestoreDefalut(bool in){ restoreDefalut=in;}
Q_SIGNALS:
	void exportMesh(); 
	void Update_glArea();

private:
	
	Ui::SlicedialogClass ui;
	int numPlane; //numeber of plane
	int distance; //distance of plane

	bool distanceDefault; // enable/disable distance 
	bool  defaultTrackball;
	bool restoreDefalut;

private slots:
	
	
	void on_ExportButton_clicked();
	void on_radioButton_toggled(bool);
	void on_DefultButton_clicked();
	
	
	void on_SliderPlaneDistance_valueChanged(int);
	void on_spinBoxDistance_valueChanged(int);
	void on_spinBoxPlane_valueChanged(int);
	void on_SliderPlane_valueChanged(int);
	void on_on_slideTrackBall_clicked(bool);
	void on_DefaultTrackball_clicked(bool);


	

	
	
};

#endif // Slicedialog_H
