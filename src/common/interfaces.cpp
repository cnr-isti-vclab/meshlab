#include "interfaces.h"

void MeshLabInterface::Log(const char * f, ... )
{
  if(log)
  {
    char buf[4096];
    va_list marker;
    va_start( marker, f );
    vsprintf(buf,f,marker);
    va_end( marker );
    log->Log(GLLogStream::FILTER,buf);
  }
}


void MeshLabInterface::Log(int Level, const char * f, ... )
{
  if(log)
  {
    char buf[4096];
    va_list marker;
    va_start( marker, f );
    vsprintf(buf,f,marker);
    va_end( marker );
    log->Log(Level,buf);
  }
}
