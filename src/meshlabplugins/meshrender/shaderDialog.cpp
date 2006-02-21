#include "shaderDialog.h"

#define DECFACTOR 100000.0f



ShaderDialog::ShaderDialog(ShaderInfo *sInfo, GLArea* gla, QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	shaderInfo = sInfo;
	glarea = gla;
	colorSignalMapper = new QSignalMapper(this);
	valueSignalMapper = new QSignalMapper(this);

	QGridLayout * qgrid = new QGridLayout(ui.uvTab);
	qgrid->setColumnMinimumWidth(0, 45);
	qgrid->setColumnMinimumWidth(1, 40);
	qgrid->setColumnMinimumWidth(2, 40);
	qgrid->setColumnMinimumWidth(3, 40);

	int row=0;
	std::map<QString, UniformVariable>::iterator i;
	for (i = shaderInfo->uniformVars.begin(); i != shaderInfo->uniformVars.end(); ++i) {

		QLabel *varNameLabel = new QLabel(this);
		varNameLabel->setObjectName(i->first+"_name");
		varNameLabel->setText(i->first);

		qgrid->addWidget(varNameLabel, row, 0);

		int varNum = getVarsNumber(i->second.type);

		switch (i->second.widget) {

			case WIDGET_NONE: {
				for (int j=0;j<varNum;++j) {
					QLineEdit *qline = new QLineEdit(this);
					qline->setObjectName(tr("%1%2").arg(i->first).arg(j));
					if (i->second.type == SINGLE_INT) {
						qline->setText(tr("%1").arg(i->second.ival[j]));

					} else {
						qline->setText(tr("%1").arg(i->second.fval[j]));

					}

					connect(qline, SIGNAL(textChanged(QString)), valueSignalMapper, SLOT(map()));
					valueSignalMapper->setMapping(qline,tr("%1%2").arg(i->first).arg(j));
					lineEdits[tr("%1%2").arg(i->first).arg(j)]=qline;

					qgrid->addWidget(qline, row, j+1);
				}
												} break;
			case WIDGET_COLOR : {

				QPushButton * colorButton = new QPushButton(this);
				colorButton->setText("Change");
				connect(colorButton, SIGNAL(clicked()), colorSignalMapper, SLOT(map()));
				colorSignalMapper->setMapping(colorButton,i->first);					

				qgrid->addWidget(colorButton, row, 1);


													} break;
			case WIDGET_SLIDER : {
				for (int j=0;j<varNum;++j) {
					QSlider *qslider = new QSlider(this);
					qslider->setTickPosition(QSlider::NoTicks);
					qslider->setOrientation(Qt::Horizontal);
					qslider->setObjectName(tr("%1%2").arg(i->first).arg(j));
					if (i->second.type == SINGLE_INT) {
						qslider->setTickInterval(i->second.step);
						qslider->setRange(i->second.min, i->second.max);
						qslider->setValue(i->second.ival[j]);
					} else {
						qslider->setTickInterval(i->second.step*DECFACTOR);
						qslider->setRange(i->second.min*DECFACTOR, i->second.max*DECFACTOR);
						qslider->setValue(i->second.fval[j]*DECFACTOR);						
					}
					connect(qslider, SIGNAL(valueChanged(int)), valueSignalMapper, SLOT(map()));
					valueSignalMapper->setMapping(qslider,tr("%1%2").arg(i->first).arg(j));

					sliders[tr("%1%2").arg(i->first).arg(j)]=qslider;

					qgrid->addWidget(qslider, row, j+1);

				}			 
													 } break;
		}	
		++row;
	}


	connect(colorSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(setColorValue(const QString &)));
	connect(valueSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(valuesChanged(const QString &)));



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
	ui.fpTextBrowser->setSource(QUrl(shaderInfo->fpFile));
	ui.fpTextBrowser->setAutoFormatting(QTextBrowser::AutoNone);
	ui.fpTextBrowser->setLineWrapMode(QTextBrowser::WidgetWidth);



	ui.vpTextBrowser->setSearchPaths(QStringList(shadersDir.absolutePath()));
	ui.vpTextBrowser->setSource(QUrl(shaderInfo->vpFile));
	ui.vpTextBrowser->setAutoFormatting(QTextBrowser::AutoNone);
	//End of Vertex and Fragment Program Tabs Section


	this->setWindowFlags(Qt::WindowStaysOnTopHint);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));




}

ShaderDialog::~ShaderDialog()
{

}

void ShaderDialog::setColorValue(const QString &varName)
{
	QColor old;
	if ( shaderInfo->uniformVars[varName].type == ARRAY_3_FLOAT) {
		old.setRgbF(shaderInfo->uniformVars[varName].fval[0], shaderInfo->uniformVars[varName].fval[1], shaderInfo->uniformVars[varName].fval[2]);
	} else if (shaderInfo->uniformVars[varName].type == ARRAY_4_FLOAT) {
		old.setRgbF(shaderInfo->uniformVars[varName].fval[0], shaderInfo->uniformVars[varName].fval[1], shaderInfo->uniformVars[varName].fval[2], shaderInfo->uniformVars[varName].fval[3]);
	}

	QColor newColor = QColorDialog::getColor(old, this);
	if (newColor.isValid()) {

		QLabel *label = labels[varName];

		shaderInfo->uniformVars[varName].fval[0] = newColor.redF(); 
		shaderInfo->uniformVars[varName].fval[1] = newColor.greenF();
		shaderInfo->uniformVars[varName].fval[2] = newColor.blueF();

		if (shaderInfo->uniformVars[varName].type == ARRAY_4_FLOAT) {

			shaderInfo->uniformVars[varName].fval[3] = newColor.alphaF();

		}
	}
	glarea->updateGL();
}


void ShaderDialog::valuesChanged(const QString &varNameAndIndex) {


	int varIndex = varNameAndIndex[varNameAndIndex.length()-1].digitValue();
	QString varName = varNameAndIndex;
	varName.chop(1);

	int varWidget = shaderInfo->uniformVars[varName].widget;
	int varType = shaderInfo->uniformVars[varName].type;
	switch (varWidget) {
		case WIDGET_NONE: {
			QLineEdit * qline = lineEdits[varNameAndIndex];
			if (varType == SINGLE_INT) {
				shaderInfo->uniformVars[varName].ival[varIndex] = qline->text().toInt();
			} else {
				shaderInfo->uniformVars[varName].fval[varIndex] = qline->text().toFloat();
			}
											} break;
		case WIDGET_SLIDER: {
			QSlider * qslider = sliders[varNameAndIndex];
			if (varType == SINGLE_INT) {
				shaderInfo->uniformVars[varName].ival[varIndex] = qslider->value();
			} else {
				shaderInfo->uniformVars[varName].fval[varIndex] = qslider->value()/DECFACTOR; 
			}
												} break;
	}
	glarea->updateGL();

}

