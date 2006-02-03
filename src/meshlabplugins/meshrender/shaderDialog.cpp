#include "shaderDialog.h"

ShaderDialog::ShaderDialog(ShaderInfo *sInfo, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	shaderInfo = sInfo;

	std::map<QString, UniformVariable>::iterator i;
	for (i = shaderInfo->uniformVars.begin(); i != shaderInfo->uniformVars.end(); ++i) {
		ui.varList->addItem(i->first);
	}
	

	//Vertex and Fragment Program Tabs Section
	QDir shadersDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
				if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release")
					shadersDir.cdUp();
#elif defined(Q_OS_MAC)
				if (shadersDir.dirName() == "MacOS") {
					shadersDir.cdUp();
					shadersDir.cdUp();
					shadersDir.cdUp();
				}
#endif
	shadersDir.cd("shaders");

	ui.fpTextBrowser->setSearchPaths(QStringList(shadersDir.absolutePath()));
	ui.vpTextBrowser->setSearchPaths(QStringList(shadersDir.absolutePath()));

	ui.fpTextBrowser->setSource(QUrl(shaderInfo->fpFile));
	ui.vpTextBrowser->setSource(QUrl(shaderInfo->vpFile));
	
	ui.fpTextBrowser->reload();
	ui.vpTextBrowser->reload();
	
	connect(ui.varList, SIGNAL(activated(QString)), this, SLOT(onVarList_change(QString)));


	
	
}

ShaderDialog::~ShaderDialog()
{

}

void ShaderDialog::onVarList_change(QString varName) 
{
	
	ui.varRes->setText(varName);

}
