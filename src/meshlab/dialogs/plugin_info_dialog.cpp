#include "plugin_info_dialog.h"
#include "ui_plugin_info_dialog.h"

#include <QDir>
#include <QPluginLoader>

#include <common/interfaces/filter_plugin_interface.h>
#include <common/interfaces/iomesh_plugin_interface.h>
#include <common/interfaces/decorate_plugin_interface.h>
#include <common/interfaces/render_plugin_interface.h>
#include <common/interfaces/edit_plugin_interface.h>

PluginInfoDialog::PluginInfoDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PluginInfoDialog)
{
	ui->setupUi(this);
}

PluginInfoDialog::PluginInfoDialog(const QString& path, const QStringList& fileNames, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PluginInfoDialog),
	pathDirectory(path)
{
	ui->setupUi(this);
	
	interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),QIcon::Normal, QIcon::On);
	interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),QIcon::Normal, QIcon::Off);
	featureIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));
	
	populateTreeWidget(path, fileNames);
}

PluginInfoDialog::~PluginInfoDialog()
{
	delete ui;
}

void PluginInfoDialog::populateTreeWidget(const QString& path, const QStringList& fileNames)
{
	if (fileNames.isEmpty()) {
		ui->label->setText(tr("Can't find any plugins in the %1 " "directory.").arg(QDir::toNativeSeparators(path)));
		ui->treeWidget->hide();
	}
	else {
		int nPlugins = 0;
		QDir dir(path);
		for (const QString& fileName : fileNames) {
			QPluginLoader loader(dir.absoluteFilePath(fileName));
			QObject *plugin = loader.instance();
			
			QTreeWidgetItem *pluginItem = new QTreeWidgetItem(ui->treeWidget);
			pluginItem->setText(0, fileName);
			pluginItem->setIcon(0, interfaceIcon);
			ui->treeWidget->setItemExpanded(pluginItem, false);
			
			QFont boldFont = pluginItem->font(0);
			boldFont.setBold(true);
			pluginItem->setFont(0, boldFont);
			
			if (plugin) {
				IOMeshPluginInterface *iMeshIO = qobject_cast<IOMeshPluginInterface *>(plugin);
				if (iMeshIO){
					nPlugins++;
					QStringList Templist;
					for(const FileFormat& f: iMeshIO->importFormats()){
						QString formats;
						for(const QString& s : f.extensions)
							formats+="Importer_"+s+" ";
						Templist.push_back(formats);
					}
					for(const FileFormat& f: iMeshIO->exportFormats()){
						QString formats;
						for(const QString& s: f.extensions)
							formats+="Exporter_"+s+" ";
						Templist.push_back(formats);
					}
					addItems(pluginItem,Templist);
				}
				DecoratePluginInterface *iDecorate = qobject_cast<DecoratePluginInterface *>(plugin);
				if (iDecorate){
					nPlugins++;
					QStringList Templist;
					for(QAction *a: iDecorate->actions())
						Templist.push_back(a->text());
					addItems(pluginItem,Templist);
				}
				FilterPluginInterface *iFilter = qobject_cast<FilterPluginInterface *>(plugin);
				if (iFilter){
					nPlugins++;
					QStringList Templist;
					for(QAction *a: iFilter->actions())
						Templist.push_back(a->text());
					addItems(pluginItem,Templist);
				}
				RenderPluginInterface *iRender = qobject_cast<RenderPluginInterface *>(plugin);
				if (iRender){
					nPlugins++;
					QStringList Templist;
					for(QAction *a: iRender->actions())
						Templist.push_back(a->text());
					addItems(pluginItem,Templist);
				}
				EditPluginInterfaceFactory *iEdit = qobject_cast<EditPluginInterfaceFactory *>(plugin);
				if (iEdit){
					nPlugins++;
					QStringList Templist;
					for(QAction *a: iEdit->actions())
						Templist.push_back(a->text());
					addItems(pluginItem,Templist);
				}
			}
		}
		std::string lbl = "Found the following " + std::to_string(nPlugins) + " plugins in the %1 " "directory:";
		ui->label->setText(tr(lbl.c_str()).arg(QDir::toNativeSeparators(path)));
	}
}

void PluginInfoDialog::addItems(QTreeWidgetItem* pluginItem, const QStringList& features)
{
	for (const QString& feature: features) {
		QTreeWidgetItem *featureItem = new QTreeWidgetItem(pluginItem);
		featureItem->setText(0, feature);
		featureItem->setIcon(0, featureIcon);
	}
}

void PluginInfoDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int)
{
	QString parent;
	QString actionName;
	if(item==NULL) return;
	if (item->parent()!=NULL){
		parent=item->parent()->text(0);
		actionName=item->text(0);
	}
	else parent=item->text(0);
	QString fileName=pathDirectory+"/"+parent;
	QDir dir(pathDirectory);
	QPluginLoader loader(fileName);
	qDebug("Trying to load the plugin '%s'", qUtf8Printable(fileName));
	QObject *plugin = loader.instance();
	if (plugin) {
		IOMeshPluginInterface *iMeshIO = qobject_cast<IOMeshPluginInterface *>(plugin);
		if (iMeshIO){
			for(const FileFormat& f: iMeshIO->importFormats()){
				QString formats;
				for(const QString& s: f.extensions)
					formats+="Importer_"+s+" ";
				if (actionName==formats) ui->labelInfo->setText(f.description);
			}
			for(const FileFormat& f: iMeshIO->exportFormats()){
				QString formats;
				for(const QString& s: f.extensions)
					formats+="Exporter_"+s+" ";
				if (actionName==formats) ui->labelInfo->setText(f.description);
			}
		}
		DecoratePluginInterface *iDecorate = qobject_cast<DecoratePluginInterface *>(plugin);
		if (iDecorate) {
			for(QAction *a: iDecorate->actions())
				if (actionName==a->text())
					ui->labelInfo->setText(iDecorate->decorationInfo(a));
		}
		FilterPluginInterface *iFilter = qobject_cast<FilterPluginInterface *>(plugin);
		if (iFilter) {
			for(QAction *a: iFilter->actions())
				if (actionName==a->text()) 
					ui->labelInfo->setText(iFilter->filterInfo(iFilter->ID(a)));
		}
//		RenderPluginInterface *iRender = qobject_cast<RenderPluginInterface *>(plugin);
//		if (iRender){
//		}
		EditPluginInterfaceFactory *iEditFactory = qobject_cast<EditPluginInterfaceFactory *>(plugin);
		if (iEditFactory) {
			for(QAction *a: iEditFactory->actions())
				if(iEditFactory)
					ui->labelInfo->setText(iEditFactory->getEditToolDescription(a));
		}
	}
}
