#ifndef PLUGIN_INFO_DIALOG_H
#define PLUGIN_INFO_DIALOG_H

#include <QDialog>
#include <QIcon>

namespace Ui {
class PluginInfoDialog;
}

class QTreeWidgetItem;

class PluginInfoDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit PluginInfoDialog(QWidget *parent = nullptr);
	explicit PluginInfoDialog(const QString &path, const QStringList &fileNames,QWidget *parent);
	~PluginInfoDialog();

private slots:
	void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
	
private:
	void populateTreeWidget(const QString &path, const QStringList &fileNames);
	void addItems(QTreeWidgetItem *, const QStringList &features);
	
	Ui::PluginInfoDialog *ui;
	QIcon interfaceIcon;
	QIcon featureIcon;
	QString pathDirectory;
};

#endif // PLUGIN_INFO_DIALOG_H
