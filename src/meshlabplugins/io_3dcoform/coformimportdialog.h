#ifndef COFORMIMPORTDIALOG_H
#define COFORMIMPORTDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui
{
	class CoformImportDialogClass;
}

class CoformImportDialog : public QDialog
{
	Q_OBJECT

public:
	CoformImportDialog(QStandardItemModel* model,QWidget *parent = 0);
	~CoformImportDialog();

	void addExtensionFilters(const QStringList& ext);

private:

	Ui::CoformImportDialogClass* ui;
};

#endif // COFORMIMPORTDIALOG_H
