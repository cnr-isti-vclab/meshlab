/*
** pyramid_point_renderer.h Pyramid Point Based Rendering header.
**
**
**   history:	created  02-Jul-07
*/


#ifndef __PYRAMID_POINT_RENDERER_H__
#define __PYRAMID_POINT_RENDERER_H__

#include <cmath>
#include <cassert>

#include "pyramid_point_renderer_base.h"

class PyramidPointRenderer : public PyramidPointRendererBase
{
 private:
  void createShaders ( void );

 public:

  PyramidPointRenderer(int w, int h);
 
};

#endif
