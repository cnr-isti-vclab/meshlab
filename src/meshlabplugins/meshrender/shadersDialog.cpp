#include "shadersDialog.h"

ShadersDialog::ShadersDialog(ShaderInfo* sInfo, QWidget *parent)
: QDialog(parent)
{

	ui.setupUi(this);
	
	signalMapper = new QSignalMapper(this);
	shaderInfo = sInfo;
	
	ui.gridLayout1->setColumnStretch(3, 1);
	ui.gridLayout1->setColumnMinimumWidth(0,80);
	//ui.gridLayout1->setColumnMinimumWidth(1,60);
	ui.gridLayout1->setColumnMinimumWidth(2,60);
	
	int row = 0;
	std::map<QString, UniformVariable>::iterator i;
	for (i = shaderInfo->uniformVars.begin(); i != shaderInfo->uniformVars.end(); ++i,++row) {
	

		//variable's name label
		QLabel *qlab = new QLabel(i->first,this);
		qlab->setText(i->first);
		ui.gridLayout1->addWidget(qlab,row,0);

		//variable's value label
		QLabel *qlab_val = new QLabel(i->first+"val",this);

		labels[i->first] = qlab_val;

		switch (shaderInfo->uniformVars[i->first].type) {
			case SINGLE_INT: 
				{
					
					qlab_val->setText(tr("%1").arg(shaderInfo->uniformVars[i->first].ival));

				} break;
			case SINGLE_FLOAT: 
				{ 
					qlab_val->setText(tr("%1").arg(shaderInfo->uniformVars[i->first].fval));
				
				} break; 
			case ARRAY_2_FLOAT: 
				{ 
					qlab_val->setText(tr("%1, %2").arg(shaderInfo->uniformVars[i->first].val2[0]).arg(
																							shaderInfo->uniformVars[i->first].val2[1]));
					
				} break; 
			case ARRAY_3_FLOAT: 
				{ 
					qlab_val->setText(tr("%1, %2, %3").arg(shaderInfo->uniformVars[i->first].val3[0]).arg(
																									 shaderInfo->uniformVars[i->first].val3[1]).arg(
																									 shaderInfo->uniformVars[i->first].val3[2]));
					
				} break; 
			case ARRAY_4_FLOAT: 
				{ 
					qlab_val->setText(tr("%1, %2, %3, %4").arg(shaderInfo->uniformVars[i->first].val4[0]).arg(
																											 shaderInfo->uniformVars[i->first].val4[1]).arg( 
																											 shaderInfo->uniformVars[i->first].val4[2]).arg(
																											 shaderInfo->uniformVars[i->first].val4[3]));
					
				} break; 
			default: 
				{ 

				} break; 
		}

		ui.gridLayout1->addWidget(qlab_val,row,1);
		
		QPushButton *changeButton = new QPushButton(i->first,this);
		changeButton->setText("Change");
		connect(changeButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
		signalMapper->setMapping(changeButton,i->first);
		ui.gridLayout1->addWidget(changeButton,row,2);
		
	}		

	connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(changeClick(const QString &)));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	

}


ShadersDialog::~ShadersDialog()
{

}

void ShadersDialog::changeClick(const QString &text) 
{
	switch (shaderInfo->uniformVars[text].widget) {
		case WIDGET_NONE:
			{
				setSimpleValue(text);
			} break;
		case WIDGET_COLOR:
			{
				setColorValue(text);
			} break;
		case WIDGET_SLIDER:
			{
				setSimpleValue(text);
			} break;
	}
}
		
void ShadersDialog::setSimpleValue(QString varName) {
	switch (shaderInfo->uniformVars[varName].type) {
		case SINGLE_INT: 
				{
					shaderInfo->uniformVars[varName].ival = QInputDialog::getInteger(this,
																																						tr("Change uniform variable"),
																																						varName,
																																						shaderInfo->uniformVars[varName].ival,
																																						shaderInfo->uniformVars[varName].min,
																																						shaderInfo->uniformVars[varName].max,
																																						shaderInfo->uniformVars[varName].step);


					labels[varName]->setText(tr("%1").arg(shaderInfo->uniformVars[varName].ival));
				} break;
			case SINGLE_FLOAT: 
				{ 
					shaderInfo->uniformVars[varName].fval = QInputDialog::getDouble(this,
																																						tr("Change uniform variable"),
																																						varName,
																																						shaderInfo->uniformVars[varName].fval,
																																						shaderInfo->uniformVars[varName].min,
																																						shaderInfo->uniformVars[varName].max,
																																						7);


					labels[varName]->setText(tr("%1").arg(shaderInfo->uniformVars[varName].fval));
				
				} break; 
			case ARRAY_2_FLOAT: 
				{ 
					//qlab_val->setText(tr("%1, %2").arg(shaderInfo->uniformVars[i->first].val2[0]).arg(
				//																			shaderInfo->uniformVars[i->first].val2[1]));
					
				} break; 
			case ARRAY_3_FLOAT: 
				{ 
					//qlab_val->setText(tr("%1, %2, %3").arg(shaderInfo->uniformVars[i->first].val3[0]).arg(
					//																				 shaderInfo->uniformVars[i->first].val3[1]).arg(
					//																				 shaderInfo->uniformVars[i->first].val3[2]));
					
				} break; 
			case ARRAY_4_FLOAT: 
				{ 
					//qlab_val->setText(tr("%1, %2, %3, %4").arg(shaderInfo->uniformVars[i->first].val4[0]).arg(
						//																					 shaderInfo->uniformVars[i->first].val4[1]).arg( 
							//																				 shaderInfo->uniformVars[i->first].val4[2]).arg(
								//																			 shaderInfo->uniformVars[i->first].val4[3]));
					
				} break; 
			default: 
				{ 

				} break;  
	}
}



void ShadersDialog::setColorValue(QString varName)
{
	  
		QColor old;
		if ( shaderInfo->uniformVars[varName].type == ARRAY_3_FLOAT) {
			old.setRgbF(shaderInfo->uniformVars[varName].val3[0], shaderInfo->uniformVars[varName].val3[1], shaderInfo->uniformVars[varName].val3[2]);
		} else if (shaderInfo->uniformVars[varName].type == ARRAY_4_FLOAT) {
			old.setRgbF(shaderInfo->uniformVars[varName].val4[0], shaderInfo->uniformVars[varName].val4[1], shaderInfo->uniformVars[varName].val4[2], shaderInfo->uniformVars[varName].val4[3]);
		}

    QColor newColor = QColorDialog::getColor(old, this);
    if (newColor.isValid()) {
        
					QLabel *label = labels[varName];
					
					if (shaderInfo->uniformVars[varName].type == ARRAY_3_FLOAT) {
							shaderInfo->uniformVars[varName].val3[0] = newColor.redF(); 
							shaderInfo->uniformVars[varName].val3[1] = newColor.greenF();
              shaderInfo->uniformVars[varName].val3[2] = newColor.blueF();

							label->setText(tr("%1, %2, %3").arg(shaderInfo->uniformVars[varName].val3[0]).arg(
																									shaderInfo->uniformVars[varName].val3[1]).arg(
																									shaderInfo->uniformVars[varName].val3[2]));

					} else if (shaderInfo->uniformVars[varName].type == ARRAY_4_FLOAT) {
							shaderInfo->uniformVars[varName].val4[0] = newColor.redF(); 
							shaderInfo->uniformVars[varName].val4[1] = newColor.greenF();
							shaderInfo->uniformVars[varName].val4[2] = newColor.blueF();
							shaderInfo->uniformVars[varName].val4[3] = newColor.alphaF();

							label->setText(tr("%1, %2, %3, %4").arg(shaderInfo->uniformVars[varName].val4[0]).arg(
																									shaderInfo->uniformVars[varName].val4[1]).arg(
																									shaderInfo->uniformVars[varName].val4[2]).arg(
																									shaderInfo->uniformVars[varName].val4[3]));
					}
    }
}

void ShadersDialog::setSliderValue(QString varName) {
	return;
}