/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
History
$Log$
Revision 1.10  2007/12/11 16:21:06  corsini
improve textures parameters description

Revision 1.9  2007/12/10 14:22:06  corsini
new version with layout correct

Revision 1.7  2007/12/06 14:47:35  corsini
remove model reference

Revision 1.6  2007/12/03 11:56:10  corsini
code restyling


****************************************************************************/

// Local headers
#include "rmshaderdialog.h"


// we create the dialog with all the proper contents
RmShaderDialog::RmShaderDialog(GLStateHolder * _holder, RmXmlParser * _parser,
                               QGLWidget* gla, RenderMode &rm, QWidget *parent)
                        	       : QDialog(parent)
{
	ui.setupUi(this);

	// make this dialog always visible
	this->setWindowFlags(Qt::WindowStaysOnTopHint);

	// customize the layout
	layoutUniform = dynamic_cast<QGridLayout *>(ui.frameUniform->layout());
	layoutTextures = dynamic_cast<QGridLayout *>(ui.frameTextures->layout());
	layoutOpengl = dynamic_cast<QGridLayout *>(ui.frameOpenGL->layout());

	parser = _parser;
	holder = _holder;
	glarea = gla;
	rendMode = &rm;

	eff_selected = NULL;
	pass_selected = NULL;

	if(parser->size() == 0) {
		QMessageBox::critical(0, "Meshlab",
		                      QString("This RmShader seems to have no suitable effects"));
		return;
	}

	// fill the effect combo
	for (int i = 0; i < parser->size(); i++)
		ui.cmbEffectSelection->addItem(parser->at(i).getName());

	// signal-slot connections
	connect(ui.cmbEffectSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(fillDialogWithEffect(int)));
	connect(ui.cmbPassSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(fillTabsWithPass(int)));
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(accept()));

	signaler = NULL;

	fillDialogWithEffect(0);
}

RmShaderDialog::~RmShaderDialog()
{
	for (int i = 0; i < shown.size(); i++)
		delete shown[i];

	delete signaler;
}

void RmShaderDialog::fillDialogWithEffect(int index)
{
	if (index < 0 || index >= parser -> size())
		return;

	eff_selected = &(parser->at(index));
	ui.cmbPassSelection->clear();

	for (int i = 0; i < eff_selected->size(); i++)
		ui.cmbPassSelection->addItem(eff_selected->at(i).getName());

	holder->setPasses(eff_selected->getPassList());
	fillTabsWithPass(0);

	if (!holder->compile()) {
		QMessageBox::critical(0, "Meshlab",
		                         "An error occurred during shader compiling.\n" + holder->getLastError());
	} else if (!holder->link()) {
		QMessageBox::critical(0, "Meshlab",
		                         "An error occurred during shader linking.\n" + holder->getLastError());
	}
}

void RmShaderDialog::fillTabsWithPass(int index)
{
	clearTabs();
	if (index < 0 || eff_selected == NULL || index >= eff_selected->size())
		return;

	pass_selected = &(eff_selected->at(index));

	// Set the source code of vertex shader
	ui.textVertex->setText(pass_selected->getVertex());

	// Set the source code of fragment shader
	ui.textFragment->setText(pass_selected->getFragment());

	// General Info in the first tab
	QString info;
	if (pass_selected->hasRenderTarget())
		info += "Render Target: " + pass_selected->getRenderTarget().name + "\n";

	for (int i = 0; i < 2; i++)
		for (int j = 0;
		     j < (i == 0 ? pass_selected->vertexUniformVariableSize() : pass_selected->fragmentUniformVariableSize());
		     j++) {
			UniformVar v = pass_selected->getUniform(j, i == 0 ? RmPass::VERTEX : RmPass::FRAGMENT);

			if(v.representerTagName == "RmRenderTarget") {
				if (i == 0)
					info += "Vertex";
				else
					info += "Fragment";

				info += " render Input: " + v.name;

				for (int k = 0; k < eff_selected -> size(); k++)
					if (eff_selected->at(k).getRenderTarget().name == v.textureName) {
						info += " (from pass: " + eff_selected->at(k).getName() + ")";
						break;
					}

				info += "\n";
			}
		}

	if (!info.isEmpty()) {
		QLabel *lblinfo = new QLabel(info);
		layoutUniform->addWidget(lblinfo, 0, 0, 1, 5);
		shown.append(lblinfo);
	}

	// any value change is sent to the state holder with this mapper
	// Signal are send from signaler in the form "varnameNM" where
	// NM is the index of row and column in case of matrix. (00 if
	// it is a simple variable).
	delete signaler;
	signaler = new QSignalMapper();
	connect(signaler, SIGNAL(mapped(const QString &)), this, SLOT(valuesChanged(const QString &)));


	// Uniform Variables in the first Tab
	QList<QString> usedVarables; // parser can give same variable twice in the vertex and fragment
	int row = 1;
	for (int ii = 0; ii < 2; ii++)
		for (int jj = 0;
		     jj < (ii == 0 ? pass_selected->vertexUniformVariableSize() : pass_selected->fragmentUniformVariableSize());
		     jj++) {
			UniformVar v = pass_selected->getUniform(jj, ii == 0 ? RmPass::VERTEX : RmPass::FRAGMENT);
			if (v.representerTagName == "RmRenderTarget" || usedVarables.contains(v.name))
				continue;
			usedVarables.append(v.name);

			QString varname = (ii == 0 ? "Vertex: " : "Fragment: ");
			varname += UniformVar::getStringFromUniformType(v.type) +
			           " " + v.name + (v.minSet || v.maxSet ? "\n" : "");

			switch (v.type) {
			case UniformVar::INT:
			case UniformVar::IVEC2:
			case UniformVar::IVEC3:
			case UniformVar::IVEC4: {
				int n = v.type == UniformVar::INT ? 1 : (v.type == UniformVar::IVEC2 ? 2 : (v.type == UniformVar::IVEC3 ? 3 : 4 ));
				for (int i = 0; i < n; i++) {
					QSpinBox *input = new QSpinBox();
					input->setObjectName(v.name + "0" + QString().setNum(i));
					if (v.minSet)
						input->setMinimum(v.fmin);
					else
						input -> setMinimum(-1000);

					if (v.maxSet)
						input->setMaximum(v.fmax);
					else
						input->setMaximum(1000);

					input->setSingleStep((v.minSet && v.maxSet )? std::max(( v.imax - v.imin )/10, 1) : 1 );
					input->setValue(v.ivec4[i]);
					layoutUniform->addWidget(input, row, 1 + i, 1,
					                         ((i + 1)==n ? 5-n : 1));
					shown.append(input);

					connect(input, SIGNAL(valueChanged(int)), signaler, SLOT(map()));
					signaler->setMapping(input, v.name + "0" + QString().setNum(i));
				}
				if (v.minSet) {
					varname += "min: " + QString().setNum(v.imin) + " ";
				}
				if (v.maxSet) {
					varname += " max: " + QString().setNum(v.imax);
				}
				break;
			}
			case UniformVar::BOOL:
			case UniformVar::BVEC2:
			case UniformVar::BVEC3:
			case UniformVar::BVEC4:
			{
				int n = v.type == UniformVar::BOOL ? 1 : (v.type == UniformVar::BVEC2 ? 2 : (v.type == UniformVar::BVEC3 ? 3 : 4 ));
				for( int i = 0; i < n; i++ ) {
					QCheckBox * input = new QCheckBox();
					input -> setObjectName( v.name + "0" + QString().setNum(i) );
					input -> setCheckState( v.bvec4[i] ? Qt::Checked : Qt::Unchecked );
					layoutUniform->addWidget(input, row, 1+i, 1, ((i+1)==n ? 5-n : 1));
					shown.append(input);

					connect(input, SIGNAL(stateChanged(int)), signaler, SLOT(map()));
					signaler->setMapping(input, v.name + "0" + QString().setNum(i) );
				}
				break;
			}
			case UniformVar::FLOAT:
			case UniformVar::VEC2:
			case UniformVar::VEC3:
			case UniformVar::VEC4:
			{
				int n = v.type == UniformVar::FLOAT ? 1 : (v.type == UniformVar::VEC2 ? 2 : (v.type == UniformVar::VEC3 ? 3 : 4 ));
				for( int i = 0; i < n; i++ )
				{
					QDoubleSpinBox * input = new QDoubleSpinBox();
					input -> setObjectName( v.name + "0" + QString().setNum(i) );
					input -> setDecimals(4);
					if( v.minSet ) input -> setMinimum( v.fmin ); else input -> setMinimum( -1000 );
					if( v.maxSet ) input -> setMaximum( v.fmax ); else input -> setMaximum( 1000 );
					input -> setSingleStep( (v.minSet && v.maxSet ) ? std::max(( v.fmax - v.fmin )/10., 0.0001) : 0.0001 );
					input -> setValue( v.vec4[i] );
					layoutUniform->addWidget( input, row, 1+i, 1, ((i+1)==n ? 5-n : 1) );
					shown.append(input);

					connect(input, SIGNAL(valueChanged(double)), signaler, SLOT(map()));
					signaler->setMapping(input, v.name + "0" + QString().setNum(i) );
				}
				if( v.minSet ) { varname += "min: " + QString().setNum(v.fmin) + " "; }
				if( v.maxSet ) { varname += " max: " + QString().setNum(v.fmax); }
				break;
			}
			case UniformVar::MAT2:
			case UniformVar::MAT3:
			case UniformVar::MAT4:
			{
				int n = v.type == UniformVar::MAT2 ? 2 : (v.type == UniformVar::MAT3 ? 3 : 4 );
				for( int i = 0; i < n; i++ ) {
					for( int j = 0; j < n; j++ ) {
						QDoubleSpinBox * input = new QDoubleSpinBox();
						input -> setObjectName( v.name + QString().setNum(i) + QString().setNum(j));
						input -> setDecimals(4);
						if( v.minSet ) input -> setMinimum( v.fmin ); else input -> setMinimum( -1000 );
						if( v.maxSet ) input -> setMaximum( v.fmax ); else input -> setMaximum( 1000 );
						input -> setSingleStep( (v.minSet && v.maxSet ) ? std::max(( v.fmax - v.fmin )/10., 0.0001) : 0.0001 );
						input -> setValue( v.vec4[(i*n)+j] );
						layoutUniform->addWidget(input, row, 1+j, 1, ((j+1)==n ? 5-n : 1));
						shown.append(input);

						connect(input, SIGNAL(valueChanged(double)), signaler, SLOT(map()));
						signaler->setMapping(input, v.name + QString().setNum(i) + QString().setNum(j));
					}
					if( (i+1) < n ) row += 1;
				}
				if( v.minSet ) { varname += "min: " + QString().setNum(v.fmin) + " "; }
				if( v.maxSet ) { varname += " max: " + QString().setNum(v.fmax); }
				break;
			}
			case UniformVar::SAMPLER1D:
			case UniformVar::SAMPLER2D:
			case UniformVar::SAMPLER3D:
			case UniformVar::SAMPLERCUBE:
			case UniformVar::SAMPLER1DSHADOW:
			case UniformVar::SAMPLER2DSHADOW:
			{
				QLabel * link = new QLabel( "<font color=\"blue\">See texture tab</font>" );
				layoutUniform->addWidget(link, row, 1, 1, 4);
				shown.append(link);
				break;
			}
			case UniformVar::OTHER:
			{
				QLabel * unimpl = new QLabel( "[Unimplemented mask]" );
				layoutUniform->addWidget( unimpl, row, 1, 1, 4);
				shown.append(unimpl);
			}
			}

			QLabel * lblvar = new QLabel(varname);
			layoutUniform->addWidget( lblvar, row, 0 );
			shown.append(lblvar);

			row += 1;
		}


	// Texture in the second tab
	for( int ii = 0, row = 0; ii < 2; ii++ )
		for( int jj = 0; jj < ( ii == 0 ? pass_selected -> vertexUniformVariableSize() : pass_selected -> fragmentUniformVariableSize()); jj++ )
		{
			UniformVar v = pass_selected -> getUniform( jj, ii == 0 ? RmPass::VERTEX : RmPass::FRAGMENT );
			if( v.textureFilename.isNull() ) continue;

			QFileInfo finfo(v.textureFilename);

			QDir textureDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
			if (textureDir.dirName() == "debug" || textureDir.dirName() == "release" || textureDir.dirName() == "plugins"  ) textureDir.cdUp();
#elif defined(Q_OS_MAC)
			if (textureDir.dirName() == "MacOS") { for(int i=0;i<4;++i){ textureDir.cdUp(); if(textureDir.exists("textures")) break; } }
#endif
			textureDir.cd("textures");
			QFile f( textureDir.absoluteFilePath(finfo.fileName()));

			QString varname = ( ii == 0 ? "Vertex texture: " : "Fragment texture: ");
			varname += UniformVar::getStringFromUniformType(v.type) + " " + v.name + "<br>";
			varname += "Filename: " + finfo.fileName() + (f.exists() ? "" : " [<font color=\"red\">not found</font>]");

			for( int k = 0; k < v.textureGLStates.size(); k++ )
			{
				varname += "<br>OpenGL state: " + v.textureGLStates[k].getName() + ": " +
					parser->convertGlStateToString(v.textureGLStates[k]);
			}

			QLabel * lblvar = new QLabel(varname);
			lblvar -> setTextFormat( Qt::RichText );
			lblvar -> setObjectName( v.name + "00" );
			layoutTextures->addWidget( lblvar, row++, 0, 1, 2 );
			shown.append(lblvar);

			QLineEdit * txtChoose = new QLineEdit( textureDir.absoluteFilePath(finfo.fileName()) );
			txtChoose -> setObjectName( v.name + "11" );
			layoutTextures->addWidget( txtChoose, row, 0 );
			shown.append(txtChoose);

			connect(txtChoose, SIGNAL(editingFinished()), signaler, SLOT(map()));
			signaler->setMapping(txtChoose, v.name + "11");

			QPushButton * btnChoose = new QPushButton( "Browse" );
			btnChoose -> setObjectName( v.name + "22" );
			layoutTextures->addWidget( btnChoose, row, 1 );
			shown.append(btnChoose);

			connect(btnChoose, SIGNAL(clicked()), signaler, SLOT(map()));
			signaler->setMapping(btnChoose, v.name + "22");

			row++;
		}

	// OpenGL Status
	if( pass_selected -> openGLStatesSize() == 0 )
	{
		QLabel * lblgl = new QLabel( "No openGL states set" );
		layoutOpengl->addWidget( lblgl, row, 0 );
		shown.append(lblgl);
	}
	else
	{
		for( int i = 0, row = 0; i < pass_selected -> openGLStatesSize(); i++ )
		{
			QString str = "OpenGL state: " + pass_selected -> getOpenGLState(i).getName();
			str += " (" + QString().setNum(pass_selected -> getOpenGLState(i).getState()) + "): " +
				QString().setNum(pass_selected -> getOpenGLState(i).getValue());
			QLabel * lblgl = new QLabel(str);
			layoutOpengl->addWidget( lblgl, row++, 0 );
			shown.append(lblgl);
		}
	}
}

void RmShaderDialog::clearTabs()
{
	for( int i = 0; i < shown.size(); i++ )
	{
		shown[i] -> hide();
		shown[i] -> close();
		delete shown[i];
	}

	shown.clear();

	ui.textVertex -> clear();
	ui.textFragment -> clear();
}

void RmShaderDialog::valuesChanged(const QString & varNameAndIndex )
{
	int len = varNameAndIndex.length();
	int colIdx = QString(varNameAndIndex[ len - 1 ]).toInt();
	int rowIdx = QString(varNameAndIndex[ len - 2 ]).toInt();
	QString varname = varNameAndIndex.left(len-2);

	QLabel * lbl = NULL;
	QLineEdit * txt = NULL;
	bool isTextureFileEdit = false;


	QVariant val;
	for( int i = 0; val.isNull() && !isTextureFileEdit && i < shown.size(); i++ )
	{
		if( shown[i] -> objectName() == varNameAndIndex )
		{
			QDoubleSpinBox * dinp = dynamic_cast<QDoubleSpinBox*>(shown[i]);
			if( dinp ) { val = QVariant(dinp -> value()); }
			QSpinBox * sinp = dynamic_cast<QSpinBox*>(shown[i]);
			if( sinp ) { val = QVariant(sinp -> value()); }
			QCheckBox * cinp = dynamic_cast<QCheckBox*>(shown[i]);
			if( cinp ) { val = QVariant(cinp -> checkState() == Qt::Checked); }
			QLineEdit * linp = dynamic_cast<QLineEdit*>(shown[i]);
			if( linp ) { val = QVariant(linp -> text()); isTextureFileEdit = true; }
			QPushButton * binp = dynamic_cast<QPushButton*>(shown[i]);
			if( binp ) {
				isTextureFileEdit = true;

				// choose the filename with a dialog
				QFileDialog fd(0,"Choose new texture");

				QDir texturesDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
				if (texturesDir.dirName() == "debug" || texturesDir.dirName() == "release") texturesDir.cdUp();
#elif defined(Q_OS_MAC)
				if (texturesDir.dirName() == "MacOS") { for(int i=0;i<4;++i){ texturesDir.cdUp(); if(texturesDir.exists("textures")) break; } }
#endif
				texturesDir.cd("textures");

				fd.setDirectory(texturesDir);
				fd.move(500, 100);
				if (fd.exec())
					val = fd.selectedFiles().at(0);
				else
					return;
			}
		}
		if( !lbl && (lbl = dynamic_cast<QLabel*>(shown[i])) && lbl -> objectName().left(len-2) != varname ) lbl = NULL;
		if( !txt && (txt = dynamic_cast<QLineEdit*>(shown[i])) && txt -> objectName().left(len-2) != varname ) txt = NULL;
	}

	if( val.isNull() )
	{
		qWarning( "Uniform Variable changed in the dialog, but no valid input found.. fix me! (no change done)");
		return;
	}

	// if it's a texture file update the info shown in the dialog
	if( isTextureFileEdit )
	{
		txt -> setText( val.toString() );
		QString label = lbl -> text();
		int statusStart = label.indexOf("Filename: ");
		int statusEnd = label.indexOf(']');
		QFileInfo finfo(val.toString());
		if( finfo.exists() ) lbl -> setText(  label.mid(0,statusStart) + "Filename: " + finfo.fileName() + " [<font color=\"blue\">ok</font>" + label.mid(statusEnd) );
		else lbl -> setText(  label.mid(0,statusStart) + "Filename: " + finfo.fileName() + " [<font color=\"red\">not found</font>" + label.mid(statusEnd) );
	}

	holder -> updateUniformVariableValuesFromDialog( pass_selected -> getName(), varname, rowIdx, colIdx, val );

	glarea -> updateGL();
}

