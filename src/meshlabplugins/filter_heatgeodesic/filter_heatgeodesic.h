#ifndef FILTERSAMPLE_PLUGIN_H
#define FILTERSAMPLE_PLUGIN_H

#include <common/plugins/interfaces/filter_plugin.h>

class FilterHeatGeodesicPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum { FP_COMPUTE_HEATGEODESIC_FROM_SELECTION  } ;

	FilterHeatGeodesicPlugin();
	virtual ~FilterHeatGeodesicPlugin();

	QString pluginName() const;

	QString filterName(ActionIDType filter) const;
	QString pythonFilterName(ActionIDType f) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FilterArity filterArity(const QAction*) const;
	int getPreConditions(const QAction *) const;
	int postCondition(const QAction* ) const;
	RichParameterList initParameterList(const QAction*, const MeshModel &/*m*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);

private:
    void computeHeatGeodesicFromSelection(MeshDocument& mesh, vcg::CallBackPos* cb, float m);
};

#endif
