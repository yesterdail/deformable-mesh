#ifndef HJ_ToolPointer_h__
#define HJ_ToolPointer_h__

#include "ToolBase.h"

namespace hj
{
  class ToolPointer : public ToolBase
  {
  public:
    enum SelectionMode
    {
      None,
      Move,           // object(s) are moved
      Size,           // object is resized
    };

    ToolPointer();

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

    /** these are for interaction. */
    GraphicsBase* select_object_;
    SelectionMode select_mode_;
    int           select_handle_;

    glm::dvec2 last_point_;
  };
}
#endif // HJ_ToolPointer_h__