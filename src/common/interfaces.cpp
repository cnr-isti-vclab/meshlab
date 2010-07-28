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

bool MeshFilterInterface::isFilterApplicable(QAction *act, const MeshModel& m, QStringList &MissingItems) const
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

  if (preMask & MeshModel::MM_FACENUMBER && (m.cm.fn==0))
      MissingItems.push_back("Non empty Face Set");

  return MissingItems.isEmpty();
}

