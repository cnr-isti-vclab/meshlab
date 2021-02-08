#include "plugin_interface.h"

PluginInterface::PluginInterface() :
    logstream(nullptr), enabled(true)
{
}

void PluginInterface::setLog(GLLogStream* log)
{
	this->logstream = log;
}

bool PluginInterface::isEnabled() const
{
	return enabled;
}

void PluginInterface::enable()
{
	enabled = true;
}

void PluginInterface::disable()
{
	enabled = false;
}

QFileInfo PluginInterface::pluginFileInfo() const
{
	return plugFileInfo;
}

void PluginInterface::log(const char* s)
{
	if(logstream != nullptr) {
		logstream->Log(GLLogStream::FILTER, s);
	}
}

void PluginInterface::log(const std::string& s)
{
	if(logstream != nullptr) {
		logstream->Log(GLLogStream::FILTER, s);
	}
}

void PluginInterface::log(GLLogStream::Levels level, const char* s)
{
	if(logstream != nullptr) {
		logstream->Log(level, s);
	}
}

void PluginInterface::log(GLLogStream::Levels level, const std::string& s)
{
	if(logstream != nullptr) {
		logstream->Log(level, s);
	}
}

void PluginInterface::realTimeLog(QString Id, const QString& meshName, const char* f)
{
	if(logstream != nullptr) {
		logstream->RealTimeLog(Id, meshName, f);
	}
}
