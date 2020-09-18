#include "plugin_interface.h"

MeshLabInterface::MeshLabInterface() :
	log(nullptr)
{
}

void MeshLabInterface::setLog(GLLogStream* log)
{
	this->log = log;
}

void MeshLabInterface::Log(const char* s)
{
	if(log != nullptr) {
		log->Log(GLLogStream::FILTER, s);
	}
}

void MeshLabInterface::Log(const std::string& s)
{
	if(log != nullptr) {
		log->Log(GLLogStream::FILTER, s);
	}
}

void MeshLabInterface::Log(GLLogStream::Levels level, const char* s)
{
	if(log != nullptr) {
		log->Log(level, s);
	}
}

void MeshLabInterface::Log(GLLogStream::Levels level, const std::string& s)
{
	if(log != nullptr) {
		log->Log(level, s);
	}
}

void MeshLabInterface::RealTimeLog(QString Id, const QString& meshName, const char* f)
{
	if(log != nullptr) {
		log->RealTimeLog(Id, meshName, f);
	}
}
