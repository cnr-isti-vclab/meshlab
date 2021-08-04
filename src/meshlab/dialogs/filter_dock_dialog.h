#ifndef FILTER_DOCK_DIALOG_H
#define FILTER_DOCK_DIALOG_H

#include <QDockWidget>

namespace Ui {
class FilterDockDialog;
}

class FilterDockDialog : public QDockWidget
{
	Q_OBJECT

public:
	explicit FilterDockDialog(QWidget *parent = nullptr);
	~FilterDockDialog();

private:
	Ui::FilterDockDialog *ui;
};

#endif // FILTER_DOCK_DIALOG_H
