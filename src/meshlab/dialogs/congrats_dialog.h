#ifndef CONGRATS_DIALOG_H
#define CONGRATS_DIALOG_H

#include <QDialog>

namespace Ui {
class CongratsDialog;
}

class CongratsDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit CongratsDialog(QWidget *parent = nullptr);
	~CongratsDialog();
	
private slots:
	void on_githubButton_clicked();
	
	void on_emailButton_clicked();
	
	void on_closeButton_clicked();
	
private:
	void updateDontShowSetting();
	Ui::CongratsDialog *ui;
};

#endif // CONGRATS_DIALOG_H
