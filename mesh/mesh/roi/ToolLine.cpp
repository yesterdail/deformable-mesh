#include "ToolLine.h"
#include "GraphicsLine.h"

namespace hj
{

  ToolLine::ToolLine()
    : ToolBase()
    , current_obj_(NULL)
  {

  }

  bool ToolLine::OnMouseDown(GraphicsRenderer* gren,
    const glm::dvec2 &point)
  {
    current_obj_ = new GraphicsLine(
      point,
      point + glm::dvec2(1));
    AddNewGraphics(gren, current_obj_);
    return true;
  }

  bool ToolLine::OnMouseMove(GraphicsRenderer* gren,
    const glm::dvec2 &point)
  {
    if (!current_obj_ || current_obj_->IsFinished()) return false;

    int count = (int)gren->GetGraphics().size();
    if (count <= 0) return false;

    gren->GetGraphics()[count - 1]->MoveHandleTo(point, 2);
    return true;
  }

  bool ToolLine::OnMouseUp(GraphicsRenderer* /*gren*/, 
    const glm::dvec2 &/*point*/)
  {
    if (!current_obj_ || current_obj_->IsFinished()) return false;

    current_obj_->Finish();
    current_obj_ = NULL;
    return true;
  }

}