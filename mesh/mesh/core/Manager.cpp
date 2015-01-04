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

  bool Manager::LoadMesh(const std::string& filename)
  {
    return renderer_ptr_->LoadMesh(filename);
  }

  bool Manager::LoadTexture(const std::string& filename)
  {
    return renderer_ptr_->LoadTexture(filename);
  }

  bool Manager::ResetCamera()
  {
    return renderer_ptr_->ResetCamera();
  }

  void Manager::Rotate(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    renderer_ptr_->Rotate(newMouseX,
      newMouseY,
      lastMouseX,
      lastMouseY);
  }

  void Manager::Zoom(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    renderer_ptr_->Zoom(newMouseX,
      newMouseY,
      lastMouseX,
      lastMouseY);
  }

  void Manager::Move(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    renderer_ptr_->Move(newMouseX,
      newMouseY,
      lastMouseX,
      lastMouseY);
  }

  void Manager::SetSmooth()
  {
    renderer_ptr_->SetSmooth();
  }

  void Manager::SetFlat()
  {
    renderer_ptr_->SetFlat();
  }

  void Manager::SetWireframe(bool w)
  {
    renderer_ptr_->SetWireframe(w);
  }

  void Manager::SetSolid(bool s)
  {
    renderer_ptr_->SetSolid(s);
  }

  void Manager::SetTexture(bool t)
  {
    renderer_ptr_->SetTexture(t);
  }
}