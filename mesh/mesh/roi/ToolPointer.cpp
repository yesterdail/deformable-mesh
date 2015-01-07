#include "ToolPointer.h"
#include "GraphicsBase.h"

namespace hj
{

  ToolPointer::ToolPointer()
    : ToolBase()
    , select_object_(NULL)
    , select_mode_(SelectionMode::None)
    , select_handle_(-1)
  {

  }

  bool ToolPointer::OnMouseDown(GraphicsRenderer* gren,
    const glm::dvec2 &point)
  {
    GraphicsBase* object;
    int handleNumber;
    select_mode_ = SelectionMode::None;
    bool wasChange = false;

    // Test for resizing (only if control is selected, cursor is on the handle)
    // Test for move (cursor is on the object)
    int count = (int)gren->GetGraphics().size();
    if (count <= 0) return false;

    for (int i = count - 1; i >= 0; --i) {
      object = gren->GetGraphics()[i];

      handleNumber = object->MakeHitTest(point);
      if (handleNumber > 0) {
        select_mode_ = SelectionMode::Size;

        // keep resized object in class member
        select_object_ = object;
        select_handle_ = handleNumber;

        gren->UnselectAll();
        select_object_->SetSelection(true);
        wasChange = true;
        break;
      }
      else if (handleNumber == 0) {
        select_mode_ = SelectionMode::Move;

        // keep moved object in class member
        select_object_ = object;

        gren->UnselectAll();
        select_object_->SetSelection(true);
        wasChange = true;
        break;
      }
    }

    // Click on background
    if (select_mode_ == SelectionMode::None)
    {
      gren->UnselectAll();
      wasChange = false;
    }

    last_point_ = point;
    return wasChange;
  }

  bool ToolPointer::OnMouseMove(GraphicsRenderer* /*gren*/, 
    const glm::dvec2 &point)
  {
    bool wasChange = false;

    // Resize
    if (select_mode_ == SelectionMode::Size) {
      select_object_->MoveHandleTo(point, select_handle_);
      wasChange = true;
    }
    // Move
    else if (select_mode_ == SelectionMode::Move) {
      select_object_->Move(point - last_point_);
      wasChange = true;
    }

    last_point_ = point;
    return wasChange;
  }

  bool ToolPointer::OnMouseUp(GraphicsRenderer* /*gren*/, 
    const glm::dvec2 &/*point*/)
  {
    bool wasChange = select_mode_ != SelectionMode::None;

    select_mode_ = SelectionMode::None;
    select_handle_ = -1;
    select_object_ = NULL;

    return wasChange;
  }



}