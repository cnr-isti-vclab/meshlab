#include "meshlab_plugin.h"

MeshLabPlugin::MeshLabPlugin() :
    logstream(nullptr)
{
}

void MeshLabPlugin::setLog(GLLogStream* log)
{
	this->logstream = log;
}

void MeshLabPlugin::log(const char* s)
{
	if(logstream != nullptr) {
		logstream->log(GLLogStream::FILTER, s);
	}
}

void MeshLabPlugin::log(const std::string& s)
{
	if(logstream != nullptr) {
		logstream->log(GLLogStream::FILTER, s);
	}
}

void MeshLabPlugin::log(GLLogStream::Levels level, const char* s)
{
	if(logstream != nullptr) {
		logstream->log(level, s);
	}
}

void MeshLabPlugin::log(GLLogStream::Levels level, const std::string& s)
{
	if(logstream != nullptr) {
		logstream->log(level, s);
	}
}

void MeshLabPlugin::realTimeLog(QString id, const QString& meshName, const char* f)
{
	if(logstream != nullptr) {
		logstream->realTimeLog(id, meshName, f);
	}
}
