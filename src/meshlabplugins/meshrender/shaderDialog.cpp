#include "shaderDialog.h"

#define DECFACTOR 100000.0f



ShaderDialog::ShaderDialog(ShaderInfo *sInfo, GLArea* gla, RenderMode &rm, QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	shaderInfo = sInfo;
	glarea = gla;
	rendMode = &rm;

	colorSignalMapper = new QSignalMapper(this);
	valueSignalMapper = new QSignalMapper(this);

	QGridLayout * qgrid = new QGridLayout(ui.uvTab);
	qgrid->setColumnMinimumWidth(0, 45);
	qgrid->setColumnMinimumWidth(1, 40);
	qgrid->setColumnMinimumWidth(2, 40);
	qgrid->setColumnMinimumWidth(3, 40);

	QLabel *perVertexColorLabel = new QLabel(this);
	perVertexColorLabel->setText("Use PerVertex Color");
	QCheckBox *perVertexColorCBox = new QCheckBox(this);
	rendMode->colorMode =  GLW::CMNone;
	connect(perVertexColorCBox, SIGNAL(stateChanged(int)), this, SLOT(setColorMode(int)));

	qgrid->addWidget(perVertexColorLabel, 0, 0);
	qgrid->addWidget(perVertexColorCBox, 0, 1);

	int row=1;
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
					qline->setAlignment(Qt::AlignRight);
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

	//OpenGL Status Tab Section
	
	QGridLayout * qgridGlStatus = new QGridLayout(ui.glTab);
	qgridGlStatus->setColumnMinimumWidth(0, 45);
	qgridGlStatus->setColumnMinimumWidth(1, 40);

	row = 0;
	std::map<int, QString>::iterator glIterator;
	for (glIterator = shaderInfo->glStatus.begin(); glIterator != shaderInfo->glStatus.end(); ++glIterator) {
		QLabel *glVarLabel = new QLabel(this);
		QLabel *glValueLabel = new QLabel(this);
	
	
		switch (glIterator->first) {
			case SHADE:  glVarLabel->setText("glShadeModel"); glValueLabel->setText(glIterator->second); ++row; break;
			case ALPHA_TEST:  glVarLabel->setText("GL_ALPHA_TEST"); glValueLabel->setText(glIterator->second); ++row; break;
			case ALPHA_FUNC:  glVarLabel->setText("glAlphaFunc"); glValueLabel->setText(glIterator->second + ", " + shaderInfo->glStatus[ALPHA_CLAMP]); ++row; break;
			//case ALPHA_CLAMP: used in ALPHA_FUNC
			case BLENDING:  glVarLabel->setText("GL_BLEND"); glValueLabel->setText(glIterator->second); ++row; break;
			case BLEND_FUNC_SRC:  glVarLabel->setText("glBlendFunc"); glValueLabel->setText(glIterator->second + ", " + shaderInfo->glStatus[BLEND_FUNC_SRC]); ++row; break;
			//case BLEND_FUNC_DST: used in BLEND_FUNC_SRC
			case BLEND_EQUATION: glVarLabel->setText("glBlendEquation"); glValueLabel->setText(glIterator->second); ++row; break;
			case DEPTH_TEST: glVarLabel->setText("GL_DEPTH_TEST"); glValueLabel->setText(glIterator->second); ++row; break;
			case DEPTH_FUNC: glVarLabel->setText("glDepthFunc"); glValueLabel->setText(glIterator->second); ++row; break;
			//case CLAMP_NEAR:
			//case CLAMP_FAR:
			case CLEAR_COLOR_R: glVarLabel->setText("glClearColor"); glValueLabel->setText(glIterator->second + ", " +
																																										 shaderInfo->glStatus[CLEAR_COLOR_G] + ", " +
																																										 shaderInfo->glStatus[CLEAR_COLOR_B] + ", " +
																																										 shaderInfo->glStatus[CLEAR_COLOR_A]); ++row; break;
			//case CLEAR_COLOR_G: used in CLEAR_COLOR_R
			//case CLEAR_COLOR_B: used in CLEAR_COLOR_R
			//case CLEAR_COLOR_A: used in CLEAR_COLOR_R
		}

		qgridGlStatus->addWidget(glVarLabel, row, 0);
		qgridGlStatus->addWidget(glValueLabel, row, 1);
		
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


void ShaderDialog::setColorMode(int state) {
	if (state == Qt::Checked) {
		rendMode->colorMode = GLW::CMPerVert;
	} else {
		rendMode->colorMode = GLW::CMNone;
	}
	glarea->updateGL();
}