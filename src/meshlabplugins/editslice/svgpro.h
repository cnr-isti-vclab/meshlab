#ifndef SVGPRO_H
#define SVGPRO_H

#include <QDialog>
#include "ui_svgpro.h"

class SVGPro : public QDialog
{
	Q_OBJECT

public:
	SVGPro(QWidget *parent, int numPlans, bool ExportMode);
	~SVGPro();
	void Init(int image_width, int image_height, int viewBox_width, int viewBox_height, float scale);
	inline int getImageWidth(){ bool ok; return ui.ImageWidth->text().toInt(&ok, 10);}
	inline int getImageHeight(){ bool ok; return ui.ImageHeight->text().toInt(&ok, 10);}
	inline int getViewBoxWidth(){ bool ok; return ui.viewBox_Width->text().toInt(&ok, 10);}
	inline int getViewBoxHeight(){ bool ok; return ui.viewBox_Height->text().toInt(&ok, 10);}
	bool showText;
	int numCol;
	int numRow;
bool ExportMode;
private:
	Ui::SVGProClass ui;
	
	QString mesureUnit;

private slots:
	void on_view_text_stateChanged(int);
	
	void on_buttonBox_rejected();
	void on_buttonBox_accepted();
	void on_viewBox_Height_textChanged(const QString c);
	void on_viewBox_Width_textChanged(const QString c);
	void on_pixel_choose_clicked();
	void on_cm_choose_clicked();
	
	void on_ImageHeight_textChanged(const QString c);
	void on_ImageWidth_textChanged(const QString c);
	
};

#endif // SVGPRO_H
