#ifndef HJ_MeshRenderer_h__
#define HJ_MeshRenderer_h__

#include "common/macro.h"
#include "common/glmext.h"

namespace hj
{
  class GLFramebuffer;
  class GLProgram;
  class Image;

  class MeshRenderer
  {
  public:
    /**
    * Default constructor
    */
    MeshRenderer();

    /**
    * Destructor.
    */
    ~MeshRenderer();

    /**
    * Sets pointer to input volume and view size
    * @param view_size: render view size.
    * @return: True if data are valid, false otherwise.
    */
    bool Initialize(const glm::ivec2& view_size);

    /**
    * Retrieves pixel data of an MPR view.
    * @param pixels: Pointer to the buffer receiving pixel data.
    * @param pixel_length: Size of pixels.
    */
    void GetPixel(uint8_t* pixels,
      uint32_t pixel_length,
      bool swap_channels);

    /**
    * Resizes output image.
    * @param view_size: render view size.
    * @return: True if data are valid, false otherwise.
    */
    bool ResizeOutput(const glm::ivec2 &new_size);

  private:

    /**
    * Paint.
    */
    bool Run();

  private:
    GLFramebuffer* out_fbo_ptr_;
  };
}
#endif // HJ_MeshRenderer_h__