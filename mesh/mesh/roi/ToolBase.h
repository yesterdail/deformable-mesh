#ifndef HJ_ToolBase_h__
#define HJ_ToolBase_h__

#include "common/glmext.h"
#include "GraphicsRenderer.h"

namespace hj
{
  class ToolBase
  {
  public:

    ToolBase();

    /**
    * Create New graphics when mouse down.
    * @param point: mouse position.
    */
    virtual bool OnMouseDown(GraphicsRenderer* gren,
      const glm::dvec2 &point) = 0;

    /**
    * resize new graphics when mouse move.
    * @param point: mouse position.
    */
    virtual bool OnMouseMove(GraphicsRenderer* gren,
      const glm::dvec2 &point) = 0;

    /**
    * Release mouse.
    * @param point: mouse position.
    */
    virtual bool OnMouseUp(GraphicsRenderer* gren,
      const glm::dvec2 &point) = 0;

  protected:
    /**
    * Add a graphics object.
    * @param type: graphics type.
    * @param start: start point of rectangle.
    * @param end: end point of rectangle.
    * @return: True if data are valid, false otherwise.
    */
    bool AddNewGraphics(GraphicsRenderer* gren,
      GraphicsBase* o);
  };
}
#endif // HJ_ToolBase_h__