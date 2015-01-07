#ifndef HJ_GraphicsRenderer_h__
#define HJ_GraphicsRenderer_h__

#include "common/macro.h"
#include "common/glmext.h"
#include "Rect.h"
#include <vector>

namespace hj
{
  class GraphicsBase;
  class ToolBase;
  class GLFramebuffer;

  class GraphicsRenderer
  {
  public:
    enum ToolType
    {
      Pointer = 0,
      Line,
      //Anchor,
      //Control,
      Max
    };

    /**
    * Default constructor
    */
    GraphicsRenderer();

    /**
    * Destructor.
    */
    ~GraphicsRenderer();

    /**
    * Set FBO.
    * @return: True if data are valid, false otherwise.
    */
    bool SetFBO(GLFramebuffer* fbo);

    /**
    * Runs the engine, drawing graphics.
    * @return: True if index is valid and execution succeeds, false otherwise.
    */
    bool Run();

    /**
    * Create New graphics when mouse down.
    * @param point: mouse position.
    */
    bool OnMouseDown(const glm::vec2 &point);

    /**
    * resize new graphics when mouse move.
    * @param point: mouse position.
    */
    bool OnMouseMove(const glm::vec2 &point);

    /**
    * Release mouse.
    * @param point: mouse position.
    */
    bool OnMouseUp(const glm::vec2 &point);

    /**
    * Set current drawing graphics type.
    */
    void SetToolType(ToolType type);

    /**
    * Get current drawing graphics type.
    */
    int GetToolType();

    /**
    * Unselect all graphic objects.
    */
    void UnselectAll();

    /**
    * Remove all graphic objects.
    */
    void RemoveAll();

    /**
    * Remove selected graphic object.
    */
    void RemoveSelection();

    /**
    * Get all graphic objects.
    */
    std::vector<GraphicsBase*>& GetGraphics() 
    { return graphics_array_; }

  private:
    /** Output frame buffer. */
    GLFramebuffer* out_fbo_ptr_;

    std::vector<GraphicsBase*> graphics_array_;

    ToolType tool_type_;

    ToolBase* tools_[ToolType::Max];
  };

}
#endif // HJ_GraphicsRenderer_h__
