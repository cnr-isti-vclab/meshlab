#include "filter_plugin.h"
#include "../../python/python_utils.h"

#include <QtGlobal>

QString FilterPlugin::pythonFilterName(MeshLabPlugin::ActionIDType f) const
{
	return pymeshlab::computePythonName(filterName(f));
}

bool FilterPlugin::isFilterApplicable(const QAction* act, const MeshModel& m, QStringList &MissingItems) const
{
	int preMask = getPreConditions(act);
	MissingItems.clear();

	if (preMask == MeshModel::MM_NONE) // no precondition specified.
		return true;

	if (preMask & MeshModel::MM_VERTCOLOR && !m.hasDataMask(MeshModel::MM_VERTCOLOR))
		MissingItems.push_back("Vertex Color");

	if (preMask & MeshModel::MM_FACECOLOR && !m.hasDataMask(MeshModel::MM_FACECOLOR))
		MissingItems.push_back("Face Color");

	if (preMask & MeshModel::MM_VERTQUALITY && !m.hasDataMask(MeshModel::MM_VERTQUALITY))
		MissingItems.push_back("Vertex Quality");

	if (preMask & MeshModel::MM_FACEQUALITY && !m.hasDataMask(MeshModel::MM_FACEQUALITY))
		MissingItems.push_back("Face Quality");

	if (preMask & MeshModel::MM_WEDGTEXCOORD && !m.hasDataMask(MeshModel::MM_WEDGTEXCOORD))
		MissingItems.push_back("Per Wedge Texture Coords");

	if (preMask & MeshModel::MM_VERTTEXCOORD && !m.hasDataMask(MeshModel::MM_VERTTEXCOORD))
		MissingItems.push_back("Per Vertex Texture Coords");

	if (preMask & MeshModel::MM_VERTRADIUS && !m.hasDataMask(MeshModel::MM_VERTRADIUS))
		MissingItems.push_back("Vertex Radius");

	if (preMask & MeshModel::MM_CAMERA && !m.hasDataMask(MeshModel::MM_CAMERA))
		MissingItems.push_back("Camera");

	if (preMask & MeshModel::MM_FACENUMBER && (m.cm.fn==0))
		MissingItems.push_back("Any Faces");


	return MissingItems.isEmpty();
}

MeshLabPlugin::ActionIDType FilterPlugin::ID(const QAction* a) const
{
	QString aa=a->text();
	for(ActionIDType tt : qAsConst(typeList))
		if (a->text() == this->filterName(tt)) return tt;
	aa.replace("&","");
	for(ActionIDType tt : qAsConst(typeList))
		if (aa == this->filterName(tt)) return tt;

	qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(a->text()));
	assert(0);
	return -1;
}

QAction* FilterPlugin::getFilterAction(MeshLabPlugin::ActionIDType filterID)
{
	QString idName = this->filterName(filterID);
	return getFilterAction(idName);
}

QAction* FilterPlugin::getFilterAction(const QString& idName)
{
	QString i=idName;
	for(QAction *tt : qAsConst(actionList))
		if (idName == tt->text()) return tt;
	i.replace("&","");
	for(QAction *tt : qAsConst(actionList))
		if (i == tt->text()) return tt;

	qDebug("unable to find the action corresponding to action  '%s'", qUtf8Printable(idName));
	assert(0);
	return 0;
}

void FilterPlugin::wrongActionCalled(const QAction* action)
{
	throw MLException("Internal error: unknown action filter " + action->text() + " to this plugin.");
}

int FilterPlugin::previewOnCreatedAttributes(const QAction* act, const MeshModel& mm ) const
{
	int changedIfCalled = postCondition(act);
	int createdIfCalled = MeshModel::MM_NONE;
	if ((changedIfCalled & MeshModel::MM_VERTCOLOR) && !mm.hasDataMask(MeshModel::MM_VERTCOLOR))
		createdIfCalled = createdIfCalled | MeshModel::MM_VERTCOLOR;

	if ((changedIfCalled & MeshModel::MM_FACECOLOR) && !mm.hasDataMask(MeshModel::MM_FACECOLOR))
		createdIfCalled = createdIfCalled | MeshModel::MM_FACECOLOR;

	if ((changedIfCalled & MeshModel::MM_VERTQUALITY) && !mm.hasDataMask(MeshModel::MM_VERTQUALITY))
		createdIfCalled = createdIfCalled | MeshModel::MM_VERTQUALITY;

	if ((changedIfCalled & MeshModel::MM_FACEQUALITY) && !mm.hasDataMask(MeshModel::MM_FACEQUALITY))
		createdIfCalled = createdIfCalled | MeshModel::MM_FACEQUALITY;

	if ((changedIfCalled & MeshModel::MM_WEDGTEXCOORD) && !mm.hasDataMask(MeshModel::MM_WEDGTEXCOORD))
		createdIfCalled = createdIfCalled | MeshModel::MM_WEDGTEXCOORD;

	if ((changedIfCalled & MeshModel::MM_VERTTEXCOORD) && !mm.hasDataMask(MeshModel::MM_VERTTEXCOORD))
		createdIfCalled = createdIfCalled | MeshModel::MM_VERTTEXCOORD;

	if ((changedIfCalled & MeshModel::MM_VERTRADIUS) && !mm.hasDataMask(MeshModel::MM_VERTRADIUS))
		createdIfCalled = createdIfCalled | MeshModel::MM_VERTRADIUS;

	if ((getClass(act) == FilterPlugin::MeshCreation) && (mm.cm.vn == 0))
		createdIfCalled = createdIfCalled | MeshModel::MM_VERTCOORD;

	return createdIfCalled;
}
