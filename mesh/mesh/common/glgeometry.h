#ifndef HJ_GLGeometyr_h__
#define HJ_GLGeometyr_h__

#include <gl/glew.h>
#include <algorithm>
#include "macro.h"

namespace hj
{

  /*
  * Draws a solid sphere
  */
  void drawSolidSphere(GLdouble radius, GLint slices, GLint stacks);

  /*
  * Draws a solid cylinder
  */
  void drawSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);

  /*
  * Draws a wire cylinder
  */
  void drawWireCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
}

#endif // HJ_GLGeometyr_h__