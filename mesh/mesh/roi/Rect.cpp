#include "Rect.h"
#include <GL/glew.h>

namespace hj
{

  Rect::Rect()
  {
    this->l = 0;
    this->t = 0;
    this->w = 0;
    this->h = 0;
  }

  Rect::Rect(double l, double t, double w, double h)
  {
    this->l = l;
    this->t = t;
    this->w = w;
    this->h = h;
  }

  void Rect::Draw()
  {
    glBegin(GL_LINE_LOOP);
    glVertex2d(l, t);
    glVertex2d(l + w, t);
    glVertex2d(l + w, t + h);
    glVertex2d(l, t + h);
    glEnd();
  }

  void Rect::Fill()
  {
    glBegin(GL_QUADS);
    glVertex2d(l, t);
    glVertex2d(l + w, t);
    glVertex2d(l + w, t + h);
    glVertex2d(l, t + h);
    glEnd();
  }

  bool Rect::Contains(const glm::dvec2& point)
  {
    return point.x >= l
      && point.y >= t
      && point.x <= l + w
      && point.y <= t + h;
  }

}