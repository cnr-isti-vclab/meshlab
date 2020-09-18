#include "filter_plugin_interface.h"

bool FilterPluginInterface::isFilterApplicable(const QAction* act, const MeshModel& m, QStringList &MissingItems) const
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

int FilterPluginInterface::previewOnCreatedAttributes(const QAction* act, const MeshModel& mm ) const
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

	if ((getClass(act) == FilterPluginInterface::MeshCreation) && (mm.cm.vn == 0))
		createdIfCalled = createdIfCalled | MeshModel::MM_VERTCOORD;

	return createdIfCalled;
}
