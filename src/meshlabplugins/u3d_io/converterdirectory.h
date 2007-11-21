#ifndef CONVERTERDIRECTORY_H
#define CONVERTERDIRECTORY_H

#include <QFrame>
#include "ui_converterdirectory.h"

class ConverterDirectory : public QFrame
{
	Q_OBJECT

public:
	ConverterDirectory(QWidget *parent = 0);
	~ConverterDirectory();

private:
	Ui::ConverterDirectoryClass ui;

private slots:
	void on_browse_button_clicked();
};

#endif // CONVERTERDIRECTORY_H
