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

#include "rfx_dialog.h"

RfxDialog::RfxDialog(RfxShader *s, QAction *a, QWidget *parent)
	: QDialog(parent)
{
	shader = s;
	mGLWin = (QGLContext*)parent;

	ui.setupUi(this);
	setWindowTitle("RenderRfx [" + a->text() + "]");

	/* Passes */
	QListIterator<RfxGLPass*> pit = s->PassesIterator();
	while (pit.hasNext()) {
		RfxGLPass *pass = pit.next();
		ui.comboPasses->addItem("Pass #" +
								QString().setNum(pass->GetPassIndex()) +
								" " + pass->GetPassName() + " ");
	}
	// start from first pass
	connect(ui.comboPasses, SIGNAL(currentIndexChanged(int)), this,
			SLOT(PassSelected(int)));
	selPass = 0;
	ui.comboPasses->setCurrentIndex(0);

	QFont fixedfont;
	fixedfont.setFamily("Courier");
	fixedfont.setFixedPitch(true);
	fixedfont.setPointSize(10);
	ui.textVert->setFont(fixedfont);
	ui.textFrag->setFont(fixedfont);

	vertHL = new GLSLSynHlighter(ui.textVert->document());
	fragHL = new GLSLSynHlighter(ui.textFrag->document());

	setupTabs();
}

void RfxDialog::setupTabs()
{
	/* Uniforms */
	ui.comboUniforms->clear();
	ui.comboUniforms->setEnabled(true);
	disconnect(ui.comboUniforms, 0, 0, 0);
	ui.BoxUnifProps->setTitle("");

	QListIterator<RfxUniform*> it = shader->GetPass(selPass)->UniformsIterator();
	int unifCount = -1; // keep track of uniform index
	while (it.hasNext()) {
		unifCount++;
		RfxUniform *uni = it.next();
		if (uni->isTexture())
			continue;
		ui.comboUniforms->addItem("[" +
								  RfxUniform::GetTypeString(uni->GetType()) +
								  "] " + uni->GetName(),
								  unifCount);
	}

	if (ui.comboUniforms->count() == 0) {
		ui.comboUniforms->addItem("No uniform variables");
		ui.comboUniforms->setDisabled(true);
	} else {
		ui.comboUniforms->insertItem(0, "Select...");
		ui.comboUniforms->setCurrentIndex(0);
		connect(ui.comboUniforms, SIGNAL(currentIndexChanged(int)), this,
				SLOT(UniformSelected(int)));
	}


	/* Textures */
	ui.comboTextures->clear();
	ui.comboTextures->setEnabled(true);
	disconnect(ui.comboTextures, 0, 0, 0);

	it = shader->GetPass(selPass)->UniformsIterator();
	unifCount = -1;
	while (it.hasNext()) {
		unifCount++;
		RfxUniform *uni = it.next();
		if (!uni->isTexture())
			continue;
		ui.comboTextures->addItem("[" +
								  RfxUniform::GetTypeString(uni->GetType()) +
								  "] " +
								  ((uni->isRenderable())? " (RT) " : "") +
								  uni->GetName(),
								  unifCount);
	}

	if (ui.comboTextures->count() == 0) {
		ui.comboTextures->addItem("No textures");
		ui.comboTextures->setDisabled(true);
	} else {
		ui.comboTextures->insertItem(0, "Select...");
		ui.comboTextures->setCurrentIndex(0);
		connect(ui.comboTextures, SIGNAL(currentIndexChanged(int)), this,
				SLOT(TextureSelected(int)));
	}

	disconnect(ui.btnChangeTexture, 0, 0, 0);
	ui.BoxTextureProps->setTitle("");
	ui.EditTexFile->clear();
	ui.TexStatesTable->clear();
	ui.TexStatesTable->setRowCount(0);
	ui.TexStatesTable->setColumnCount(2);
	ui.TexStatesTable->horizontalHeader()->setStretchLastSection(true);
	ui.TexStatesTable->horizontalHeader()->hide();
	ui.TexStatesTable->verticalHeader()->hide();
	ui.lblPreview->clear();


	/* States */
	ui.glStatesTable->clear();
	ui.glStatesTable->setRowCount(0);
	ui.glStatesTable->setColumnCount(2);
	ui.glStatesTable->horizontalHeader()->setStretchLastSection(true);
	ui.glStatesTable->horizontalHeader()->hide();
	ui.glStatesTable->verticalHeader()->hide();
	QListIterator<RfxState*> sit = shader->GetPass(selPass)->StatesIterator();
	int rowidx = 0;
	while (sit.hasNext()) {
		RfxState *r = sit.next();

		// QTableWidget will take ownership of objects, do not delete them.
		QTableWidgetItem *c0 = new QTableWidgetItem(r->GetRenderState());
		c0->setFlags(Qt::ItemIsSelectable);
		QTableWidgetItem *c1 = new QTableWidgetItem(r->GetRenderValue());
		c1->setFlags(Qt::ItemIsSelectable);

		ui.glStatesTable->insertRow(rowidx);
		ui.glStatesTable->setItem(rowidx, 0, c0);
		ui.glStatesTable->setItem(rowidx, 1, c1);
		ui.glStatesTable->resizeRowToContents(rowidx);
		++rowidx;
	}
	ui.glStatesTable->resizeColumnToContents(0);
	ui.glStatesTable->resizeColumnToContents(1);


	/* Vertex/Fragment source view */
	ui.textVert->setPlainText(shader->GetPass(selPass)->GetVertexSource());
	ui.textFrag->setPlainText(shader->GetPass(selPass)->GetFragmentSource());
}

RfxDialog::~RfxDialog()
{
	delete vertHL;
	delete fragHL;

	CleanTab(ALL_TABS);
}

void RfxDialog::PassSelected(int idx)
{
	selPass = idx;
	setupTabs();
	CleanTab(ALL_TABS);
}

void RfxDialog::UniformSelected(int idx)
{
	if (idx <= 0)
		return;

	int uniIndex = ui.comboUniforms->itemData(idx).toInt();
	RfxUniform *uni = shader->GetPass(selPass)->getUniform(uniIndex);
	assert(uni);

	CleanTab(UNIFORM_TAB);
	ui.BoxUnifProps->setTitle(uni->GetName() +
	                          ((uni->GetSemantic().isNull())? "" :
	                            " [Predefined: " + uni->GetSemantic() + "]" ));

	switch (uni->GetType()) {
	case RfxUniform::INT:
	case RfxUniform::FLOAT:
	case RfxUniform::BOOL:
		DrawIFace(uni, uniIndex, 1, 1);
		break;

	case RfxUniform::VEC2:
	case RfxUniform::IVEC2:
	case RfxUniform::BVEC2:
		DrawIFace(uni, uniIndex, 1, 2);
		break;

	case RfxUniform::VEC3:
	case RfxUniform::IVEC3:
	case RfxUniform::BVEC3:
		DrawIFace(uni, uniIndex, 1, 3);
		break;

	case RfxUniform::VEC4:
	case RfxUniform::IVEC4:
	case RfxUniform::BVEC4:
		DrawIFace(uni, uniIndex, 1, 4);
		break;

	case RfxUniform::MAT2:
		DrawIFace(uni, uniIndex, 2, 2);
		break;

	case RfxUniform::MAT3:
		DrawIFace(uni, uniIndex, 3, 3);
		break;

	case RfxUniform::MAT4:
		DrawIFace(uni, uniIndex, 4, 4);
		break;

	default:
		return;
	}
}

void RfxDialog::DrawIFace(RfxUniform *u, int uidx, int rows, int columns)
{
	enum controlType { INT_CTRL, FLOAT_CTRL, BOOL_CTRL };
	float *val = u->GetValue();
	controlType ctrl;
	QWidget *controls[rows * columns];
	QGridLayout *uniLayout = ((QGridLayout*)ui.BoxUnifProps->layout());
	QSignalMapper *valMapper = new QSignalMapper(this);

	switch (u->GetType()) {
	case RfxUniform::INT:
	case RfxUniform::IVEC2:
	case RfxUniform::IVEC3:
	case RfxUniform::IVEC4:
		ctrl = INT_CTRL;
		break;

	case RfxUniform::FLOAT:
	case RfxUniform::VEC2:
	case RfxUniform::VEC3:
	case RfxUniform::VEC4:
	case RfxUniform::MAT2:
	case RfxUniform::MAT3:
	case RfxUniform::MAT4:
		ctrl = FLOAT_CTRL;
		break;

	case RfxUniform::BOOL:
	case RfxUniform::BVEC2:
	case RfxUniform::BVEC3:
	case RfxUniform::BVEC4:
		ctrl = BOOL_CTRL;
		break;

	default:
		return;
	}

	// controls in a grid layout
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			int arrayIdx = j + (i * rows);
			switch (ctrl) {
			case INT_CTRL:
				controls[arrayIdx] = new QSpinBox();
				((QSpinBox*)controls[arrayIdx])->setRange(-99, 99);
				((QSpinBox*)controls[arrayIdx])->setValue((int)val[arrayIdx]);
				connect(controls[arrayIdx], SIGNAL(valueChanged(int)),
				        valMapper, SLOT(map()));
				connect(controls[arrayIdx], SIGNAL(valueChanged(int)), this,
				        SLOT(extendRange(int)));
				break;
			case FLOAT_CTRL:
				controls[arrayIdx] = new QDoubleSpinBox();
				((QDoubleSpinBox*)controls[arrayIdx])->setRange(-99.0, 99.0);
				((QDoubleSpinBox*)controls[arrayIdx])->setValue(val[arrayIdx]);
				((QDoubleSpinBox*)controls[arrayIdx])->setDecimals(4);
				connect(controls[arrayIdx], SIGNAL(valueChanged(double)),
				        valMapper, SLOT(map()));
				connect(controls[arrayIdx], SIGNAL(valueChanged(double)), this,
				        SLOT(extendRange(double)));
				break;
			case BOOL_CTRL:
				controls[arrayIdx] = new QComboBox();
				((QComboBox*)controls[arrayIdx])->addItem("FALSE");
				((QComboBox*)controls[arrayIdx])->addItem("TRUE");
				if (!val[arrayIdx])
					((QComboBox*)controls[arrayIdx])->setCurrentIndex(0);
				connect(controls[arrayIdx], SIGNAL(currentIndexChanged(int)),
						valMapper, SLOT(map()));
				break;
			}

			valMapper->setMapping(controls[arrayIdx],
			                      QString().setNum(uidx) + '-' +
			                      QString().setNum(arrayIdx));
			valMapper->setParent(controls[arrayIdx]);

			if (!u->GetSemantic().isNull())
				controls[arrayIdx]->setDisabled(true);

			uniLayout->addWidget(controls[arrayIdx], i, j);
			widgetsByTab.insert(UNIFORM_TAB, controls[arrayIdx]);
		}
	}
	connect(valMapper, SIGNAL(mapped(const QString&)), this,
	        SLOT(ChangeValue(const QString&)));
}

void RfxDialog::CleanTab(int tabIdx)
{
	// deletes all widgets from specified tab or, if tabIdx == -1
	// removes all widgets from all tabs
	// (this applies to *dynamically created* widgets only)

	if (tabIdx == ALL_TABS) {
		QMapIterator<int, QWidget*> it(widgetsByTab);
		while (it.hasNext()) {
			QWidget *toDelete = it.next().value();
			toDelete->close();
			delete toDelete;
		}
		widgetsByTab.clear();

	} else {
		QList<QWidget*> toDelete = widgetsByTab.values(tabIdx);
		for (int i = 0; i < toDelete.size(); ++i) {
			toDelete.at(i)->close();
			delete toDelete.at(i);
		}
		widgetsByTab.remove(tabIdx);
	}
}

void RfxDialog::extendRange(double newVal)
{
	QDoubleSpinBox *sender = (QDoubleSpinBox*)QObject::sender();
	if (newVal == sender->minimum() || newVal == sender->maximum()) {
		if (newVal == sender->minimum())
			sender->setMinimum(newVal - 50);
		else
			sender->setMaximum(newVal + 50);
	}
}

void RfxDialog::extendRange(int newVal)
{
	QSpinBox *sender = (QSpinBox*)QObject::sender();
	if (newVal == sender->minimum() || newVal == sender->maximum()) {
		if (newVal == sender->minimum())
			sender->setMinimum(newVal - 50);
		else
			sender->setMaximum(newVal + 50);
	}
}

void RfxDialog::ChangeTexture(int unifIdx)
{
	int uniIndex = ui.comboTextures->itemData(unifIdx).toInt();
	RfxUniform *uni = shader->GetPass(selPass)->getUniform(uniIndex);
	assert(uni);

	QString fname = QFileDialog::getOpenFileName(this,
	                                             tr("Choose Texture"),
	                                             uni->GetTextureFName());
	if (!fname.isEmpty()) {
		uni->SetValue(QDir::fromNativeSeparators(fname));
		uni->LoadTexture();
		uni->PassToShader();

		// generate a currentIndexChanged event
		ui.comboTextures->setCurrentIndex(0);
		ui.comboTextures->setCurrentIndex(unifIdx);
	}
}

void RfxDialog::ChangeValue(const QString& val)
{
	QStringList unif = val.split('-');
	RfxUniform *uni = shader->GetPass(selPass)->getUniform(unif[0].toInt());
	float *oldVal = uni->GetValue();
	float newVal = 0.0f;

	// parent of SignalMapper has been set to appropriate QWidget type
	QObject *sender = QObject::sender()->parent();
	assert(sender);

	QComboBox *cbox = dynamic_cast<QComboBox*>(sender);
	if (cbox != NULL) {
		newVal = cbox->currentIndex();
	} else {
		QSpinBox *sbox = dynamic_cast<QSpinBox*>(sender);
		if (sbox != NULL) {
			newVal = sbox->value();
		} else {
			QDoubleSpinBox *dsbox = dynamic_cast<QDoubleSpinBox*>(sender);
			if (dsbox != NULL)
				newVal = dsbox->value();
			else
				return;
		}
	}

	oldVal[unif[1].toInt()] = newVal;
	uni->PassToShader();
}

void RfxDialog::TextureSelected(int idx)
{
	disconnect(ui.btnChangeTexture, 0, 0, 0);

	if (idx <= 0)
		return;

	int uniIndex = ui.comboTextures->itemData(idx).toInt();
	RfxUniform *uni = shader->GetPass(selPass)->getUniform(uniIndex);
	assert(uni);

	// connect Change Texture button
	QSignalMapper *sigMap = new QSignalMapper(this);
	connect(ui.btnChangeTexture, SIGNAL(clicked()), sigMap, SLOT(map()));
	sigMap->setMapping(ui.btnChangeTexture, idx);
	connect(sigMap, SIGNAL(mapped(int)), this, SLOT(ChangeTexture(int)));

	ui.BoxTextureProps->setTitle(uni->GetName());

	// clean and initialize table
	ui.TexStatesTable->clear();
	ui.TexStatesTable->setRowCount(0);
	ui.TexStatesTable->setColumnCount(2);
	ui.TexStatesTable->horizontalHeader()->setStretchLastSection(true);
	ui.TexStatesTable->horizontalHeader()->hide();
	ui.TexStatesTable->verticalHeader()->hide();

	// delete any existing preview
	ui.lblPreview->clear();

	CleanTab(TEXTURE_TAB);

	if (uni->isRenderable())
		ui.EditTexFile->clear();
	else
		ui.EditTexFile->setText(uni->GetTextureFName());

	QLabel *fname = new QLabel();
	ui.infoTexLayout->addWidget(fname);
	widgetsByTab.insert(TEXTURE_TAB, fname);
	if (!uni->isTextureFound() || !uni->isTextureLoaded()) {
		if (!uni->isTextureFound())
			fname->setText("<span style=\"color:darkred;\">Texture file not found</span>");
		else
			fname->setText("<span style=\"color:darkred;\">Texture binding failed</span>");
	} else {
		fname->setText("<span style=\"color:darkgreen;\">Texture loaded</span>");

		ImageInfo ii;
		bool loaded;
		if (uni->isRenderable()) {
			ii.preview = uni->GetRTTexture();
			ii.texType = "Render Target";
			ii.width = (ii.preview.isNull())? 0 : ii.preview.width();
			ii.height = (ii.preview.isNull())? 0 : ii.preview.height();
			ii.depth = 1;
			ii.format = "";
		} else {
			ii = RfxTextureLoader::LoadAsQImage(uni->GetTextureFName());
		}
		loaded = !ii.preview.isNull();

		if (loaded) {
			QLabel *tSize = new QLabel();
			tSize->setText("Dimensions: " +
					       QString().setNum(ii.width) + " x " +
			               QString().setNum(ii.height) +
			               ((ii.depth > 1)? " x " + QString().setNum(ii.depth) : ""));
			ui.infoTexLayout->addWidget(tSize);
			widgetsByTab.insert(TEXTURE_TAB, tSize);

			QLabel *tType = new QLabel();
			tType->setText("Type: " + ii.texType);
			ui.infoTexLayout->addWidget(tType);
			widgetsByTab.insert(TEXTURE_TAB, tType);

			QLabel *tFormat = new QLabel();
			tFormat->setText("Format: " + ii.format);
			ui.infoTexLayout->addWidget(tFormat);
			widgetsByTab.insert(TEXTURE_TAB, tFormat);

			// try to get a preview
			QPixmap prvw = QPixmap::fromImage(ii.preview);
			if (!prvw.isNull()) {
				QSize scaledSize(120, 120);
				if (ii.texType == "Cubemap Texture")
					scaledSize.setWidth(200);

				ui.lblPreview->setPixmap(prvw.scaled(scaledSize,
				                                     Qt::KeepAspectRatio));
			}
		}

		QLabel *tunit = new QLabel();
		tunit->setText("Texture Unit: " + QString().setNum(uni->GetTU()));
		ui.infoTexLayout->addWidget(tunit);
		widgetsByTab.insert(TEXTURE_TAB, tunit);
	}

	QListIterator<RfxState*> it = uni->StatesIterator();
	int rowidx = 0;
	while (it.hasNext()) {
		RfxState *r = it.next();

		// QTableWidget will take ownership of objects, do not delete them.
		QTableWidgetItem *c0 = new QTableWidgetItem(r->GetTextureState());
		c0->setFlags(Qt::ItemIsSelectable);
		QTableWidgetItem *c1 = new QTableWidgetItem(r->GetTextureValue());
		c1->setFlags(Qt::ItemIsSelectable);

		ui.TexStatesTable->insertRow(rowidx);
		ui.TexStatesTable->setItem(rowidx, 0, c0);
		ui.TexStatesTable->setItem(rowidx, 1, c1);
		ui.TexStatesTable->resizeRowToContents(rowidx);
		++rowidx;
	}
	ui.TexStatesTable->resizeColumnToContents(0);
	ui.TexStatesTable->resizeColumnToContents(1);
}



/*********************************
 * GLSL Syntax Highlighter Class *
 *********************************/

GLSLSynHlighter::GLSLSynHlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	HighlightingRule rule;

	kwordsFormat.setForeground(Qt::blue);
	QStringList keywords;
	keywords << "uniform" << "int" << "float" << "bool" << "inout"
			 << "vec2" << "vec3" << "vec4" << "ivec2" << "out"
			 << "ivec3" << "ivec4" << "bvec2" << "bvec3" << "in"
			 << "bvec4" << "mat2" << "mat3" << "mat4" << "attribute"
			 << "sampler1D" << "sampler2D" << "sampler3D"
			 << "samplerCube" << "sampler1DShadow" << "void"
			 << "sampler2DShadow" << "varying" << "const";

	foreach (QString pattern, keywords) {
		rule.pattern = QRegExp("\\b" + pattern + "\\b");
		rule.format = kwordsFormat;
		highlightingRules.append(rule);
	}

	builtinsFormat.setForeground(Qt::magenta);
	QStringList builtins;
	builtins << "gl_Position" << "gl_ClipSize" << "gl_ClipVertex"
	         << "gl_Vertex" << "gl_Normal" << "gl_Color" << "gl_FragColor"
	         << "gl_SecondaryColor" << "gl_FogCoord" << "gl_MultiTexCoord[0-7]"
	         << "gl_FrontColor" << "gl_BackColor" << "gl_FrontSecondaryColor"
	         << "gl_BackSecondaryColor" << "gl_TexCoord" << "gl_FogFragCoord"
	         << "gl_FragData" << "gl_FrontFacing" << "gl_FragCoord"
	         << "gl_FragDepth" << "gl_ModelViewMatrix" << "gl_ProjectionMatrix"
	         << "gl_ModelViewProjectionMatrix" << "gl_ModelViewMatrixInverse"
	         << "gl_ModelViewProjectionMatrixInverse" << "gl_NormalMatrix"
	         << "gl_NormalScale" << "gl_DepthRange" << "gl_Point"
	         << "gl_ModelViewMatrixInverseTranspose" << "gl_LightSource"
	         << "gl_ModelViewMatrixTranspose" << "gl_ProjectionMatrixTranspose"
	         << "gl_ModelViewProjectionMatrixInverseTranspose" << "gl_Fog"
	         << "gl_ClipPlane" << "gl_(Eye|Object)Plane[STRQ]"
	         << "gl_(Front|Back)Material" << "gl_(Front|Back)LightProduct";

	foreach (QString pattern, builtins) {
			rule.pattern = QRegExp("\\b" + pattern + "\\b");
			rule.format = builtinsFormat;
			highlightingRules.append(rule);
	}

	functionFormat.setForeground(QColor::fromRgb(255, 0, 141));
	QStringList functions;
	functions << "sin" << "cos" << "tan" << "asin" << "acos" << "atan"
	          << "radians" << "degrees" << "pow" << "exp" << "log"
	          << "expr2" << "log2" << "sqrt" << "inversesqrt" << "abs"
	          << "ceil" << "clamp" << "floor" << "fract" << "min" << "mix"
	          << "max" << "mod" << "sign" << "smoothstep" << "step"
	          << "ftransform" << "cross" << "distance" << "dot"
	          << "faceforward" << "length" << "normalize" << "reflect"
	          << "dFdx" << "dFdy" << "fwidth" << "matrixCompMult" << "all"
	          << "any" << "equal" << "greaterThan" << "lessThan" << "notEqual"
	          << "texture1DProj" << "texture2DProj" << "texture3DProj"
	          << "textureCube" << "noise4" << "texture3D" << "not" << "noise3"
	          << "texture1D" << "texture2D" << "noise1" << "noise2";

	foreach (QString pattern, functions) {
		rule.pattern = QRegExp("\\b" + pattern + "+(?=\\()");
		rule.format = functionFormat;
		highlightingRules.append(rule);
	}

	singleLineCommentFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegExp("//[^\n]*");
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);

	multiLineCommentFormat = singleLineCommentFormat;
	commentStartExpression = QRegExp("/\\*");
	commentEndExpression = QRegExp("\\*/");
}

void GLSLSynHlighter::highlightBlock(const QString &text)
{
	foreach (HighlightingRule rule, highlightingRules) {
		QRegExp expression(rule.pattern);
		int index = text.indexOf(expression);
		while (index >= 0) {
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = text.indexOf(expression, index + length);
		}
	}

	/* multiline comment management */
	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
		startIndex = text.indexOf(commentStartExpression);

	 while (startIndex >= 0) {
		int endIndex = text.indexOf(commentEndExpression, startIndex);
		int commentLength;
		if (endIndex == -1) {
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex +
			                         commentEndExpression.matchedLength();
		}
		setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = text.indexOf(commentStartExpression,
		                                      startIndex + commentLength);
	}
}
