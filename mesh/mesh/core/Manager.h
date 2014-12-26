#ifndef HJ_Manager_h__
#define HJ_Manager_h__

#include <string>
#include "common/config.h"

namespace hj
{
  class GLOffScreenRender;
  class MeshRenderer;

  class Manager
  {
  public:
    /**
    * Default constructor.
    */
    HJ_EXPORT Manager();

    /**
    * Destructor.
    */
    HJ_EXPORT ~Manager();

    /**
    * Loads a series of volume files and creates corresponding MPR and projection views.
    * @param viewWidth: Width of Rendering View.
    * @param viewHeight: Height of Rendering View.
    */
    HJ_EXPORT bool CreateView(int viewWidth, int viewHeight);

    /**
    * Retrieves pixel data.
    * @param output_buffer: Pointer to the buffer receiving pixel data.
    * @return: Size of encoded data, -1 if any parameters are invalid.
    */
    HJ_EXPORT const void GetView(uint8_t* output_buffer, int buffer_len);

    /**
    * Resizes output image.
    * @param new_width: Width of Rendering View.
    * @param new_height: Height of Rendering View.
    * @return: True if data are valid, false otherwise.
    */
    HJ_EXPORT bool Resize(int new_width, int new_height);

  private:
    GLOffScreenRender* offscreen_render_ptr_;

    MeshRenderer* renderer_ptr_;
  };
}
#endif // HJ_Manager_h__