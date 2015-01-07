#ifndef HJ_ToolLine_h__
#define HJ_ToolLine_h__

#include "ToolBase.h"

namespace hj
{
  class GraphicsLine;

  class ToolLine : public ToolBase
  {
  public:

    ToolLine();

    /**
    * Create New graphics when mouse down.
    * @param point: mouse position.
    */
    virtual bool OnMouseDown(GraphicsRenderer* gren,
      const glm::dvec2 &point);

    /**
    * resize new graphics when mouse move.
    * @param point: mouse position.
    */
    virtual bool OnMouseMove(GraphicsRenderer* gren,
      const glm::dvec2 &point);

    /**
    * Release mouse.
    * @param point: mouse position.
    */
    virtual bool OnMouseUp(GraphicsRenderer* gren,
      const glm::dvec2 &point);

  private:
    GraphicsLine* current_obj_;
  };
}
#endif // HJ_ToolLine_h__