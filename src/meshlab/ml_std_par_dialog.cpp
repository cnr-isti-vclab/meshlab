#include "ml_std_par_dialog.h"
#include "mainwindow.h"

static void updateRenderingData(MainWindow* curmwi, MeshModel* curmodel)
{
	if ((curmwi != NULL) && (curmodel != NULL))
	{
		MultiViewer_Container* mvcont = curmwi->currentViewContainer();
		if (mvcont != NULL)
		{
			MLSceneGLSharedDataContext* shar = mvcont->sharedDataContext();
			if (shar != NULL)
			{
				shar->meshAttributesUpdated(curmodel->id(), true, MLRenderingData::RendAtts(true));
				shar->manageBuffers(curmodel->id());
			}
		}
	}
}

MeshlabStdDialog::MeshlabStdDialog(QWidget *p)
	:QDockWidget(QString("Plugin"), p), previewCB(NULL), curmask(MeshModel::MM_UNKNOWN)
{
	qf = NULL;
	stdParFrame = NULL;
	clearValues();
}

/* manages the setup of the standard parameter window, when the execution of a plugin filter is requested */
bool MeshlabStdDialog::showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, MeshDocument * mdp, QAction *action, MainWindow *mwi, QWidget *gla)
{
	validcache = false;
	curAction = action;
	curmfi = mfi;
	curmwi = mwi;
	curParSet.clear();
	prevParSet.clear();
	curModel = mm;
	curMeshDoc = mdp;
	curgla = gla;

	mfi->initParameterSet(action, *mdp, curParSet);
	curmask = mfi->postCondition(action);
	if (curParSet.isEmpty() && !isPreviewable()) return false;

	createFrame();
	loadFrameContent(mdp);
	if (isPreviewable())
	{
		meshState.create(curmask, curModel);
		connect(stdParFrame, SIGNAL(parameterChanged()), this, SLOT(applyDynamic()));
	}
	connect(curMeshDoc, SIGNAL(currentMeshChanged(int)), this, SLOT(changeCurrentMesh(int)));
	raise();
	activateWindow();
	return true;
}

void MeshlabStdDialog::changeCurrentMesh(int meshInd)
{
	if (isPreviewable())
	{
		meshState.apply(curModel);
		curModel = curMeshDoc->getMesh(meshInd);
		meshState.create(curmask, curModel);
		applyDynamic();
	}
}

bool MeshlabStdDialog::isPreviewable()
{
	if ((curAction == NULL) || (curmfi == NULL) || (curmfi->filterArity(curAction) != MeshFilterInterface::SINGLE_MESH))
		return false;

	if ((curmask == MeshModel::MM_UNKNOWN) || (curmask == MeshModel::MM_NONE))
		return false;

	if ((curmask & MeshModel::MM_VERTNUMBER) ||
		(curmask & MeshModel::MM_FACENUMBER))
		return false;

	return true;
}


void MeshlabStdDialog::clearValues()
{
	curAction = NULL;
	curModel = NULL;
	curmfi = NULL;
	curmwi = NULL;
}

void MeshlabStdDialog::createFrame()
{
	if (qf) delete qf;

	QFrame *newqf = new QFrame(this);
	setWidget(newqf);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	qf = newqf;
}

// update the values of the widgets with the values in the paramlist;
void MeshlabStdDialog::resetValues()
{
	curParSet.clear();
	curmfi->initParameterSet(curAction, *curMeshDoc, curParSet);

	assert(qf);
	assert(qf->isVisible());
	// assert(curParSet.paramList.count() == stdfieldwidgets.count());
	stdParFrame->resetValues(curParSet);
}

void MeshlabStdDialog::loadFrameContent(MeshDocument *mdPt)
{
	assert(qf);
	qf->hide();

	QGridLayout *gridLayout = new QGridLayout(qf);

	setWindowTitle(curmfi->filterName(curAction));
	QLabel *ql = new QLabel("<i>" + curmfi->filterInfo(curAction) + "</i>", qf);
	ql->setTextFormat(Qt::RichText);
	ql->setWordWrap(true);
  ql->setOpenExternalLinks(true);
	gridLayout->addWidget(ql, 0, 0, 1, 2, Qt::AlignTop); // this widgets spans over two columns.

	stdParFrame = new StdParFrame(this, curgla);
	stdParFrame->loadFrameContent(curParSet, mdPt);
	gridLayout->addWidget(stdParFrame, 1, 0, 1, 2);

	int buttonRow = 2;  // the row where the line of buttons start

	QPushButton *helpButton = new QPushButton("Help", qf);
	QPushButton *closeButton = new QPushButton("Close", qf);
	QPushButton *applyButton = new QPushButton("Apply", qf);
	QPushButton *defaultButton = new QPushButton("Default", qf);
	applyButton->setFocus();
#ifdef Q_OS_MAC
	// Hack needed on mac for correct sizes of button in the bottom of the dialog.
	helpButton->setMinimumSize(100, 25);
	closeButton->setMinimumSize(100, 25);
	applyButton->setMinimumSize(100, 25);
	defaultButton->setMinimumSize(100, 25);
#endif

	if (isPreviewable())
	{
		previewCB = new QCheckBox("Preview", qf);
		previewCB->setCheckState(Qt::Unchecked);
		gridLayout->addWidget(previewCB, buttonRow + 0, 0, Qt::AlignBottom);
		connect(previewCB, SIGNAL(toggled(bool)), this, SLOT(togglePreview()));
		buttonRow++;
	}

	gridLayout->addWidget(helpButton, buttonRow + 0, 1, Qt::AlignBottom);
	gridLayout->addWidget(defaultButton, buttonRow + 0, 0, Qt::AlignBottom);
	gridLayout->addWidget(closeButton, buttonRow + 1, 0, Qt::AlignBottom);
	gridLayout->addWidget(applyButton, buttonRow + 1, 1, Qt::AlignBottom);

	connect(helpButton, SIGNAL(clicked()), this, SLOT(toggleHelp()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(applyClick()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(closeClick()));
	connect(defaultButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	qf->showNormal();
	qf->adjustSize();

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	this->setMinimumSize(qf->sizeHint());
	this->showNormal();
	this->adjustSize();
}

void MeshlabStdDialog::toggleHelp()
{
	stdParFrame->toggleHelp();
	qf->updateGeometry();
	qf->adjustSize();
	this->updateGeometry();
	this->adjustSize();
}

void MeshlabStdDialog::applyClick()
{
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);

	// Note that curModel CAN BE NULL (for creation filters on empty docs...)
	if (curmask && curModel)
	{
		meshState.apply(curModel);
		updateRenderingData(curmwi, curModel);
	}

	//PreView Caching: if the apply parameters are the same to those used in the preview mode
	//we don't need to reapply the filter to the mesh
	if ((q != nullptr) && (curMeshDoc != nullptr) && (curMeshDoc->filterHistory != nullptr))
	{
		FilterNameParameterValuesPair* oldpair = new FilterNameParameterValuesPair();
		oldpair->pair = qMakePair(q->text(), curParSet);
		curMeshDoc->filterHistory->filtparlist.append(oldpair);
	}

	bool isEqual = (curParSet == prevParSet);
	if (curModel && (isEqual) && (validcache))
	{
		meshCacheState.apply(curModel);
		updateRenderingData(curmwi, curModel);
	}
	else
		curmwi->executeFilter(q, curParSet, false);

	if (curmask && curModel)
		meshState.create(curmask, curModel);
	if (this->curgla)
		this->curgla->update();

}

void MeshlabStdDialog::applyDynamic()
{
	if (!previewCB->isChecked())
		return;
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);
	//for cache mechanism
	//needed to allocate the required memory space in prevParSet
	//it called the operator=(RichParameterSet) function defined in RichParameterSet
	prevParSet = curParSet;
	stdParFrame->readValues(prevParSet);
	// Restore the
	meshState.apply(curModel);
	curmwi->executeFilter(q, curParSet, true);
	meshCacheState.create(curmask, curModel);
	validcache = true;


	if (this->curgla)
		this->curgla->update();
}

void MeshlabStdDialog::togglePreview()
{
	if (previewCB == NULL)
		return;

	GLArea* glarea = qobject_cast<GLArea*>(curgla);

	if (previewCB->isChecked())
	{
		stdParFrame->readValues(curParSet);
		if (!prevParSet.isEmpty() && (validcache) && (curParSet == prevParSet))
		{
			meshCacheState.apply(curModel);
			updateRenderingData(curmwi, curModel);
			if (glarea != NULL)
				glarea->updateAllDecorators();

		}
		else
		{
			applyDynamic();
			if (glarea != NULL)
				glarea->updateAllDecorators();
		}
	}
	else
	{
		meshState.apply(curModel);
		updateRenderingData(curmwi, curModel);
		if (glarea != NULL)
			glarea->updateAllDecorators();
	}
	curgla->update();
}

/* click event for the close button of the standard plugin window */
void MeshlabStdDialog::closeClick()
{
	//int mask = 0;//curParSet.getDynamicFloatMask();
	if (curmask != MeshModel::MM_UNKNOWN)
	{

		meshState.apply(curModel);
		if (isPreviewable() && (previewCB != NULL) && previewCB->isChecked())
		{
			updateRenderingData(curmwi, curModel);
		}

	}
	curmask = MeshModel::MM_UNKNOWN;
	// Perform the update only if there is Valid GLarea.
	if (this->curgla)
	{
		if ((previewCB != NULL) && (previewCB->isChecked()) && (curmwi != NULL) && (curmwi->GLA() != NULL))
			curmwi->GLA()->updateAllPerMeshDecorators();

		this->curgla->update();
	}
	close();
}


// click event for the standard red crossed close button in the upper right widget's corner
void MeshlabStdDialog::closeEvent(QCloseEvent * /*event*/)
{
	closeClick();
}

MeshlabStdDialog::~MeshlabStdDialog()
{
	delete stdParFrame;
	delete previewCB;
}
