#include "decorate_plugin_interface.h"

QAction* DecoratePluginInterface::action(QString name) const
{
	QString n = name;
	foreach(QAction *tt, actions())
		if (name == this->decorationName(ID(tt))) return tt;
	n.replace("&","");
	foreach(QAction *tt, actions())
		if (n == this->decorationName(ID(tt))) return tt;

	qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(name));
	return 0;
}

PluginInterface::FilterIDType DecoratePluginInterface::ID(const QAction* a) const
{
	QString aa=a->text();
	foreach(FilterIDType tt, types())
		if (a->text() == this->decorationName(tt)) return tt;
	aa.replace("&","");
	foreach(FilterIDType tt, types())
		if (aa == this->decorationName(tt)) return tt;

	qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(a->text()));
	assert(0);
	return -1;
}

PluginInterface::FilterIDType DecoratePluginInterface::ID(QString name) const
{
	QString n = name;
	foreach(FilterIDType tt, types())
		if (name == this->decorationName(tt)) return tt;
	n.replace("&","");
	foreach(FilterIDType tt, types())
		if (n == this->decorationName(tt)) return tt;

	qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(name));
	assert(0);
	return -1;
}
