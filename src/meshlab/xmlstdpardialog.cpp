#include "xmlstdpardialog.h"

MeshLabXMLStdDialog::MeshLabXMLStdDialog( QWidget *p )
:QDockWidget(QString("Plugin"), p)
{
	curmask = 0;
	qf = NULL;
	stdParFrame=NULL;
	clearValues();
}

MeshLabXMLStdDialog::~MeshLabXMLStdDialog()
{

}

void MeshLabXMLStdDialog::clearValues()
{

}

void MeshLabXMLStdDialog::createFrame()
{

}

void MeshLabXMLStdDialog::loadFrameContent( MeshDocument *mdPt/*=0*/ )
{

}

bool MeshLabXMLStdDialog::showAutoDialog(MeshLabXMLFilterContainer *mfc,const XMLFilterInfo::MapList& mplist, MeshDocument * md, MainWindowInterface *mwi, QWidget *gla/*=0*/ )
{
	if (mfc == NULL) 
		return false;
	if (mfc->filterInterface == NULL)
		return false;
	if (mfc->xmlInfo == NULL)
		return false;
	if (mfc->act == NULL)
		return false;

	validcache = false;
	curAction=mfc->act;
	curmfc=mfc;
	curmwi=mwi;
	curParMap.clear();
	prevParMap.clear();
	curModel = md->mm();
	curMeshDoc = md;
	curgla=gla;

	//mfi->initParameterSet(action, *mdp, curParSet);
	curParMap = mplist;
	//curmask = mfc->xmlInfo->filterAttribute(mfc->act->text(),QString("postCond"));
	if(curParMap.isEmpty() && !isDynamic()) 
		return false;

	createFrame();
	loadFrameContent(md);
	QString postCond = mfc->xmlInfo->filterAttribute(mfc->act->text(),QString("postCond"));
	QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
	curmask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
	if(isDynamic())
	{
		meshState.create(curmask, curModel);
		connect(stdParFrame,SIGNAL(dynamicFloatChanged(int)), this, SLOT(applyDynamic()));
		connect(stdParFrame,SIGNAL(parameterChanged()), this, SLOT(applyDynamic()));
	}
	connect(curMeshDoc, SIGNAL(currentMeshChanged(int)),this, SLOT(changeCurrentMesh(int)));
	return true;
}

void MeshLabXMLStdDialog::applyClick()
{

}

void MeshLabXMLStdDialog::closeClick()
{

}

void MeshLabXMLStdDialog::resetValues()
{

}

void MeshLabXMLStdDialog::toggleHelp()
{

}

void MeshLabXMLStdDialog::togglePreview()
{

}

void MeshLabXMLStdDialog::applyDynamic()
{

}

void MeshLabXMLStdDialog::changeCurrentMesh( int meshInd )
{

}

//void MeshLabXMLStdDialog::closeEvent( QCloseEvent * event )
//{
//
//}

bool MeshLabXMLStdDialog::isDynamic() const
{
	 return ((curmask != MeshModel::MM_UNKNOWN) && (curmask != MeshModel::MM_NONE) && !(curmask & MeshModel::MM_VERTNUMBER) && !(curmask & MeshModel::MM_FACENUMBER));
}


XMLStdParFrame::XMLStdParFrame( QWidget *p, QWidget *gla/*=0*/ )
:QFrame(p)
{
	curr_gla=gla;
}

XMLStdParFrame::~XMLStdParFrame()
{

}