#ifndef HJ_GraphicsLine_h__
#define HJ_GraphicsLine_h__

#include "GraphicsBase.h"


namespace hj
{

  class GraphicsLine : public GraphicsBase
  {
  public:
    /**
    * Judge whether a point is near a line.
    * @start: start point of line.
    * @end: end point of line.
    * @p: test point.
    * @return: true if near, false otherwise.
    */
    static bool PointNearLine(const glm::vec2 &start, 
      const glm::vec2 &end, 
      const glm::vec2 &p);

  public:
    /**
    * Default constructor
    */
    GraphicsLine();

    /**
    * Constructor with parameters.
    */
    GraphicsLine(const glm::vec2 &start,
      const glm::vec2 &end);

    /**
    * Destructor.
    */
    virtual ~GraphicsLine();

    /**
    * Implements actual drawing code.
    * Call GraphicsBase::Draw in the end of every derived class Draw
    * function to draw tracker if necessary.
    */
    virtual void Draw();

    /**
    * Move handle to new point (resizing)
    */
    virtual void MoveHandleTo(const glm::vec2& point,
      int handle_number);

    /**
    * Move object
    */
    virtual void Move(const glm::vec2& delta);

    /**
    * Test whether line contains point.
    */
    virtual bool Contains(const glm::vec2& point);

    /**
    * return class name for type identification.
    */
    virtual std::string GetClassName() const { return "GraphicsLine"; }

    glm::vec2 GetStart() { return start_; }

    glm::vec2 GetEnd() { return end_; }

  protected:
    /**
    * Number of handles.
    */
    virtual int HandleCount() { return 2; }

    /**
    * Get handle point by 1-based number.
    */
    virtual glm::vec2 GetHandle(int handleNumber);

  protected:
    glm::vec2 start_;
    glm::vec2 end_;
  };
}
#endif // HJ_GraphicsLine_h__