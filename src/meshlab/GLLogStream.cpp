
#include <qfont.h>
#include <stdio.h>

#include "GLLogStream.h"

using namespace std;
void GLLogStream::Log(int Level, const char * f, ... )
{
  char buf[4096];
  va_list marker;
  va_start( marker, f );     

  vsprintf(buf,f,marker);
  va_end( marker );              

  S.push_back(make_pair<int,string>(Level,buf));
}

void GLLogStream::Save(int Level, const char * filename )
{
  FILE *fp=fopen(filename,"wb");
  list<pair <int,string> > ::iterator li;
  for(li=S.begin();li!=S.end();++li)
    fprintf(fp,(*li).second.c_str());
}


void  GLLogStream::glDraw(QGLWidget *qgl, int Level, int nlines)
{
  static QFont qf("Helvetica",8);
  const int LineWidth=15;
  list<pair <int,string> > ::iterator li;
  li=S.end();
  advance(li,-nlines); 
  if(li==S.end()) li=S.begin();
  int StartLine=qgl->height() - (nlines+1) * LineWidth;
  for(;li!=S.end();++li)
      qgl->renderText(20,StartLine+=LineWidth,(*li).second.c_str(),qf);
}
