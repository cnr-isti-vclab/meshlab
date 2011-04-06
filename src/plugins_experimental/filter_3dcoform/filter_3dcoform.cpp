#include "filter_3dcoform.h"
#include <vcg/complex/algorithms/create/platonic.h>
Filter3DCoformPlugin::Filter3DCoformPlugin() 
:cofGui(NULL)
{ 
	typeList << F3DC_RI_INTEGRATION;

	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

QString Filter3DCoformPlugin::filterName( FilterIDType filterId ) const
{
	switch(filterId) 
	{
		case F3DC_RI_INTEGRATION :
		{
			return QString("3D-COFORM: RI Integration");
			break;
		}
	}
	return QString("Error: Unknown Filter"); 
}

QString Filter3DCoformPlugin::filterInfo( FilterIDType filterId ) const
{
	switch(filterId)
	{
	case F3DC_RI_INTEGRATION :
		{	
			return QString("The 3D-COFORM plugin allows to retrieve and ingest meshes and Arc3d files from/in the 3D-COFORM Repository Infrastructure.");
			break;
		}
	}
	return QString("Error: Unknown Filter");
}

MeshFilterInterface::FilterClass Filter3DCoformPlugin::getClass( QAction * act)
{
	switch(ID(act))
	{
		case F3DC_RI_INTEGRATION :
		{
			return FilterClass (MeshFilterInterface::MeshCreation);
			break;
		}
	}
	return FilterClass(0);
}

void Filter3DCoformPlugin::initParameterSet( QAction * act,MeshDocument & m, RichParameterSet & par)
{
	switch(ID(act))
	{
		case F3DC_RI_INTEGRATION :
		{	
			break;
		}
	}
}

bool Filter3DCoformPlugin::applyFilter( QAction *act, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb )
{

	switch(ID(act))
	{
	case F3DC_RI_INTEGRATION :
		{	
			//MeshModel* mm=md.addNewMesh("","PUPPA!");
			qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
			//vcg::tri::Icosahedron<CMeshO>(mm->cm);
			QString defaultMeshPath;
			cofGui = new CoformGUI(&md);
			if (!cofGui->exec())
			{
				qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
				return true;
			}

			//Otherwise there is some strange problem...
			qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
			/////////////////////////////////////////////////
			return true;
			break;
		}
	}
	return false;
}

Q_EXPORT_PLUGIN(Filter3DCoformPlugin)