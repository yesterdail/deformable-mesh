#include "GraphicsBase.h"

namespace hj
{
  int GetUniqueId()
  {
    static int n = 0;
    return n++;
  }

  GraphicsBase::GraphicsBase()
    : selected_(false)
    , finished_(false)
    , handle_size_(8.0f)
  {
    object_id_ = GetUniqueId();
  }

  GraphicsBase::~GraphicsBase()
  {

  }

  void GraphicsBase::DrawTracker()
  {
    for (int i = 1; i <= HandleCount(); ++i) {
      GetHandleRect(i).Fill();
    }
  }

  Rect GraphicsBase::GetHandleRect(int handle_number)
  {
    glm::vec2 point = GetHandle(handle_number);

    return Rect(point.x - handle_size_ / 2, 
      point.y - handle_size_ / 2,
      handle_size_, 
      handle_size_);
  }

  int GraphicsBase::MakeHitTest(const glm::vec2& point)
  {
    if (selected_) {
      for (int i = 1; i <= HandleCount(); ++i)
      {
        if (GetHandleRect(i).Contains(point))
          return i;
      }
    }

    if (Contains(point))
      return 0;

    return -1;
  }
}