#include "GraphicsLine.h"
#include <GL/glew.h>

namespace hj
{
  

  bool GraphicsLine::PointNearLine(const glm::vec2 &start, 
    const glm::vec2 &end, 
    const glm::vec2 &p)
  {
    double a, b, c;
    a = glm::length(start - end);
    b = glm::length(start - p);
    c = glm::length(end - p);

    // the line is a point
    if (a <= 0.00001)
      return false;

    // point is on the line
    if (c + b - a < 0.5)
      return true;

    return false;
  }



  GraphicsLine::GraphicsLine()
    : GraphicsLine(glm::vec2(0), glm::vec2(0))
  {
  }

  GraphicsLine::GraphicsLine(
    const glm::vec2 &start,
    const glm::vec2 &end)
    : GraphicsBase()
    , start_(start)
    , end_(end)
  {
  }

  GraphicsLine::~GraphicsLine()
  {
  }

  void GraphicsLine::Draw()
  {
    if (selected_)
      glColor3f(0, 0.47f, 0.79f);
    else
      glColor3f(0.47f, 0, 0.79f);
    glBegin(GL_LINES);
    glVertex2d(start_.x, start_.y);
    glVertex2d(end_.x, end_.y);
    glEnd();

    if (selected_) {
      DrawTracker();
    }

    glColor3f(1, 1, 1);
  }

  void GraphicsLine::MoveHandleTo(const glm::vec2& point,
    int handle_number)
  {
    if (handle_number == 1) {
      start_ = point;
    }
    else {
      end_ = point;
    }
  }

  glm::vec2 GraphicsLine::GetHandle(int handleNumber)
  {
    if (handleNumber == 1)
      return start_;
    else
      return end_;
  }

  void GraphicsLine::Move(const glm::vec2& delta)
  {
    start_ += delta;
    end_ += delta;
  }

  bool GraphicsLine::Contains(const glm::vec2& point)
  {
    return PointNearLine(start_, end_, point);
  }
}