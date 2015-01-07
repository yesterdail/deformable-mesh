#ifndef HJ_GraphicsBase_h__
#define HJ_GraphicsBase_h__

#include "common/glmext.h"
#include "Rect.h"
#include <string>

namespace hj
{
  class GraphicsBase
  {
  public:
    /**
    * Default constructor
    */
    GraphicsBase();

    /**
    * Destructor.
    */
    virtual ~GraphicsBase();

    /**
    * Implements actual drawing code.
    */
    virtual void Draw() = 0;

    /**
    * Move handle to new point (resizing)
    */
    virtual void MoveHandleTo(const glm::vec2& point,
      int handle_number) = 0;

    /**
    * Move object
    */
    virtual void Move(const glm::vec2& delta) = 0;

    /**
    * Hit test.
    * @return: -1 - no hit, 
    *           0 - hit anywhere, 
    *          >1 - handle number.
    */
    virtual int MakeHitTest(const glm::vec2& point);

    /**
    * Test whether graphics contains point.
    */
    virtual bool Contains(const glm::vec2& point) = 0;

    /** 
    * return class name for type identification.
    */
    virtual std::string GetClassName() const = 0;

    /**
    * Set selection of current graphics object.
    */
    void SetSelection(bool s) { selected_ = s; }

    /**
    * Get selection of current graphics object.
    */
    bool GetSelection() { return selected_; }

    /**
    * Get graphics object id.
    */
    int GetId() { return object_id_; }

    /**
    * Set flag for finishing drawing graphics
    */
    void Finish() { finished_ = true; }

    /**
    * Get flag for finishing drawing graphics
    */
    bool IsFinished() const { return finished_; }

  
  protected:
  public:
    /**
    * Draw tracker for selected object.
    */
    void DrawTracker();

    /**
    * Number of handles.
    */
    virtual int HandleCount() = 0;

    /**
    * Get handle point by 1-based number.
    */
    virtual glm::vec2 GetHandle(int handleNumber) = 0;

    /**
    * Get handle rectangle by 1-based number.
    */
    Rect GetHandleRect(int handleNumber);

    /**
    * Transform point form view coordiante to model coordinate.
    * @param point: point in view coordinate.
    * @return: point in model coordinate.
    */
    glm::vec3 View2Model(const glm::vec3 &point);

    /**
    * Transform point form world coordiante to view coordinate.
    * @param point: point in world coordinate.
    * @return: point in view coordinate.
    */
    glm::vec3 Model2View(const glm::vec3 &point);

  protected:

    bool selected_;

    float handle_size_;

    int object_id_;

    /** indicates that we are creating graphics, the graphics is not complete yet. */
    bool finished_;
  };
}
#endif // HJ_GraphicsBase_h__