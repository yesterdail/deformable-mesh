#include "Manager.h"
#include "common/GLOffScreenRender.h"
#include "MeshRenderer.h"

namespace hj
{


  Manager::Manager()
  {
    offscreen_render_ptr_ = new GLOffScreenRender();
    renderer_ptr_ = new MeshRenderer();
  }

  Manager::~Manager()
  {
    delete renderer_ptr_;
    delete offscreen_render_ptr_;
  }

  bool Manager::CreateView(int viewWidth, int viewHeight)
  {
    return renderer_ptr_->Initialize(glm::ivec2(viewWidth, viewHeight));
  }

  const void Manager::GetView(uint8_t* output_buffer, int buffer_len)
  {
    return renderer_ptr_->GetPixel(output_buffer, buffer_len, false);
  }

  bool Manager::Resize(int new_width, int new_height)
  {
    return renderer_ptr_->ResizeOutput(glm::ivec2(new_width, new_height));
  }

}