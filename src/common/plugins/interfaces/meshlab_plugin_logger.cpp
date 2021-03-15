#include "meshlab_plugin_logger.h"

MeshLabPluginLogger::MeshLabPluginLogger() :
    logstream(nullptr)
{
}

void MeshLabPluginLogger::setLog(GLLogStream* log)
{
	this->logstream = log;
}

void MeshLabPluginLogger::log(const char* s)
{
	if(logstream != nullptr) {
		logstream->log(GLLogStream::FILTER, s);
	}
}

void MeshLabPluginLogger::log(const std::string& s)
{
	if(logstream != nullptr) {
		logstream->log(GLLogStream::FILTER, s);
	}
}

void MeshLabPluginLogger::log(GLLogStream::Levels level, const char* s)
{
	if(logstream != nullptr) {
		logstream->log(level, s);
	}
}

void MeshLabPluginLogger::log(GLLogStream::Levels level, const std::string& s)
{
	if(logstream != nullptr) {
		logstream->log(level, s);
	}
}

void MeshLabPluginLogger::realTimeLog(QString id, const QString& meshName, const char* f)
{
	if(logstream != nullptr) {
		logstream->realTimeLog(id, meshName, f);
	}
}
