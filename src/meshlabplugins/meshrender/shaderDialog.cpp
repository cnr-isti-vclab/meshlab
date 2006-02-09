#include "shaderDialog.h"

ShaderDialog::ShaderDialog(ShaderInfo *sInfo, GLArea* gla, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	shaderInfo = sInfo;
	glarea = gla;
	colorSignalMapper = new QSignalMapper(this);
	valueSignalMapper = new QSignalMapper(this);

	QVBoxLayout *verticalLayout = new QVBoxLayout(ui.uvTab);
	verticalLayout->setSpacing(6);
	verticalLayout->setMargin(0);
	verticalLayout->setObjectName("verticalLayout");
	verticalLayout->setGeometry(QRect(20, 40, 400, 280));

  std::map<QString, UniformVariable>::iterator i;
	for (i = shaderInfo->uniformVars.begin(); i != shaderInfo->uniformVars.end(); ++i) {
		QHBoxLayout *horLayout = new QHBoxLayout();
		horLayout->setSpacing(2);
		horLayout->setMargin(0);
		horLayout->setObjectName(i->first+"_horLayout");
    		
		QLabel *varNameLabel = new QLabel(this);
		varNameLabel->setObjectName(i->first+"_name");
		varNameLabel->setText(i->first);
		
		horLayout->addWidget(varNameLabel);

		QLabel *varValueLabel = new QLabel(this);
		varValueLabel->setObjectName(i->first+"_value");

		labels[i->first] = varValueLabel;

		int varNum = getVarsNumber(i->second.type);
		bool varIsInt = 0;

		switch (i->second.type) {
			case SINGLE_INT: {
				
				varIsInt = 1;
				varValueLabel->setText(tr("%1").arg(i->second.ival[0]));
											 } break;
			case SINGLE_FLOAT: {
			
        varValueLabel->setText(tr("%1").arg(i->second.fval[0]));
												 } break;
			case ARRAY_2_FLOAT : {
		
        varValueLabel->setText(tr("%1, %2").arg(i->second.fval[0]).arg(i->second.fval[1]));
													 } break;
			case ARRAY_3_FLOAT : {
			
        varValueLabel->setText(tr("%1, %2, %3").arg(i->second.fval[0]).arg(i->second.fval[1]).arg(i->second.fval[2]));
													 } break;
			case ARRAY_4_FLOAT : {
			
        varValueLabel->setText(tr("%1, %2, %3, %4").arg(i->second.fval[0]).arg(i->second.fval[1]).arg(i->second.fval[2]).arg(i->second.fval[3]));
													 } break;
		}
		horLayout->addWidget(varValueLabel);

		switch (i->second.widget) {
			
			case WIDGET_NONE: {
				for (int j=0;j<varNum;++j) {
					QLineEdit *qline = new QLineEdit(this);
					qline->setObjectName(tr("%1%2").arg(i->first).arg(j));
					if (varIsInt) {
            qline->setText(tr("%1").arg(i->second.ival[j]));
						
					} else {
            qline->setText(tr("%1").arg(i->second.fval[j]));
						
					}
					
					connect(qline, SIGNAL(textChanged(QString)), valueSignalMapper, SLOT(map()));
					valueSignalMapper->setMapping(qline,tr("%1%2").arg(i->first).arg(j));
					horLayout->addWidget(qline);
					lineEdits[tr("%1%2").arg(i->first).arg(j)]=qline;
				}
												} break;
			case WIDGET_COLOR : {
						
						QPushButton * colorButton = new QPushButton(this);
						colorButton->setText("Change");
						connect(colorButton, SIGNAL(clicked()), colorSignalMapper, SLOT(map()));
						colorSignalMapper->setMapping(colorButton,i->first);					
						horLayout->addWidget(colorButton);
					
													} break;
			case WIDGET_SLIDER : {
				for (int j=0;j<varNum;++j) {
					QSlider *qslider = new QSlider(this);
					qslider->setTickPosition(QSlider::NoTicks);
					qslider->setOrientation(Qt::Horizontal);
					qslider->setObjectName(tr("%1%2").arg(i->first).arg(j));
					if (varIsInt) {
            qslider->setTickInterval(i->second.step);
						qslider->setRange(i->second.min, i->second.max);
						qslider->setValue(i->second.ival[j]);
					} else {
            qslider->setTickInterval(i->second.step*100000);
						qslider->setRange(i->second.min*100000, i->second.max*100000);
						qslider->setValue(i->second.fval[j]*100000);						
					}
					connect(qslider, SIGNAL(valueChanged(int)), valueSignalMapper, SLOT(map()));
					valueSignalMapper->setMapping(qslider,tr("%1%2").arg(i->first).arg(j));
					horLayout->addWidget(qslider);
					sliders[tr("%1%2").arg(i->first).arg(j)]=qslider;

				}			 
													 } break;
		}

		verticalLayout->addLayout(horLayout);
	

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

	ui.vpTextBrowser->setSearchPaths(QStringList(shadersDir.absolutePath()));
	ui.vpTextBrowser->setSource(QUrl(shaderInfo->vpFile));
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


					if (shaderInfo->uniformVars[varName].type == ARRAY_3_FLOAT) {
							

							label->setText(tr("%1, %2, %3").arg(shaderInfo->uniformVars[varName].fval[0]).arg(
																									shaderInfo->uniformVars[varName].fval[1]).arg(
																									shaderInfo->uniformVars[varName].fval[2]));

					} else if (shaderInfo->uniformVars[varName].type == ARRAY_4_FLOAT) {
							
							shaderInfo->uniformVars[varName].fval[3] = newColor.alphaF();

							label->setText(tr("%1, %2, %3, %4").arg(shaderInfo->uniformVars[varName].fval[0]).arg(
																											shaderInfo->uniformVars[varName].fval[1]).arg(
																											shaderInfo->uniformVars[varName].fval[2]).arg(
																											shaderInfo->uniformVars[varName].fval[3]));
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
				float c = shaderInfo->uniformVars[varName].fval[varIndex];
				float d = qslider->value();
				float g = d/100000.0f;
				shaderInfo->uniformVars[varName].fval[varIndex] = qslider->value()/100000.0f; 
				c = shaderInfo->uniformVars[varName].fval[varIndex];
				c++;
			}
											} break;
	}
	glarea->updateGL();
  	
}

