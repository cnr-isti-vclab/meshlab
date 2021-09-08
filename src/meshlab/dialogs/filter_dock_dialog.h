#ifndef FILTER_DOCK_DIALOG_H
#define FILTER_DOCK_DIALOG_H

#include <QDockWidget>

#include <common/plugins/interfaces/filter_plugin.h>

class GLArea;

namespace Ui {
class FilterDockDialog;
}

class FilterDockDialog : public QDockWidget
{
	Q_OBJECT

public:
	explicit FilterDockDialog(
		const RichParameterList& rpl,
		FilterPlugin*            plugin,
		const QAction*           filter,
		QWidget*                 parent = nullptr,
		GLArea*                  glArea = nullptr);
	~FilterDockDialog();

signals:
	void applyButtonClicked(const QAction*, RichParameterList);

private slots:
	void on_applyPushButton_clicked();

	void on_helpPushButton_clicked();

	void on_closePushButton_clicked();

	void on_defaultPushButton_clicked();

private:
	bool isPreviewable() const;

	Ui::FilterDockDialog* ui;

	FilterPlugin*     plugin;
	const QAction*    filter;
	RichParameterList parameters;
	unsigned int      mask;
	GLArea*           currentGLArea;
};

#endif // FILTER_DOCK_DIALOG_H
