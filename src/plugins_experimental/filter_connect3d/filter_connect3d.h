#pragma once

#include <common/interfaces.h>

class Connect3dFilterPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
		MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

public:

	enum { 
		FP_CONNECT3D, 
	} ;

	Connect3dFilterPlugin();
	~Connect3dFilterPlugin();
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;

	FilterClass getClass(QAction *);
	void initParameterSet(QAction *, MeshModel &, RichParameterSet &);
	bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet &, vcg::CallBackPos * cb) ;
	int postCondition(QAction *filter) const;
	int getPreCondition(QAction *filter) const;

};

