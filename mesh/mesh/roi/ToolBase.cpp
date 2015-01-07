#include "ToolBase.h"
#include "GraphicsBase.h"

namespace hj
{

  ToolBase::ToolBase()
  {

  }

  bool ToolBase::AddNewGraphics(GraphicsRenderer* gren,
    GraphicsBase* obj)
  {
    gren->UnselectAll();
    obj->SetSelection(true);
    gren->GetGraphics().push_back(obj);
    return true;
  }
}