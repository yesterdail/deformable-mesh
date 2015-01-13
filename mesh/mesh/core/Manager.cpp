#include "Manager.h"
#include "common/GLOffScreenRender.h"
#include "common/GLFramebuffer.h"
#include "common/GLTexture.h"
#include "roi/GraphicsRenderer.h"
#include "MeshRenderer.h"

namespace hj
{


  Manager::Manager()
    : out_fbo_ptr_(NULL)
  {
    offscreen_render_ptr_ = new GLOffScreenRender();
    renderer_ptr_ = new MeshRenderer();
    graphics_renderer_ptr_ = new GraphicsRenderer();
    renderer_ptr_->SetAssociate(graphics_renderer_ptr_);
  }

  Manager::~Manager()
  {
    delete renderer_ptr_;
    delete offscreen_render_ptr_;
    delete graphics_renderer_ptr_;
    DEL_PTR(out_fbo_ptr_);
  }

  bool Manager::CreateView(int viewWidth, int viewHeight)
  {
    return Resize(viewWidth, viewHeight)
     && renderer_ptr_->ResetCamera();
  }

  const void Manager::GetView(uint8_t* output_buffer, int buffer_len)
  {
    if (!output_buffer) {
      assert(0);
      return;
    }
    if (!renderer_ptr_->Run()) {
      assert(0);
      return;
    }
    if (!graphics_renderer_ptr_->Run()) {
      assert(0);
      return;
    }
    GLTexture* texture_ptr = out_fbo_ptr_->GetTexture2D(GL_COLOR_ATTACHMENT0);
    texture_ptr->DownloadTexture(output_buffer, buffer_len);
    return;
  }

  bool Manager::Resize(int new_width, int new_height)
  {
    DEL_PTR(out_fbo_ptr_);
    out_fbo_ptr_ = new GLFramebuffer(new_width, new_height);
    out_fbo_ptr_->CreateColorTexture(GL_COLOR_ATTACHMENT0,
      GL_RGB8,
      GL_RGB,
      GL_UNSIGNED_BYTE);
    //out_fbo_ptr_->CreateColorTexture(GL_COLOR_ATTACHMENT1,
    //  GL_RGB8,
    //  GL_RGB,
    //  GL_UNSIGNED_BYTE);
    //out_fbo_ptr_->CreateDepthBuffer();
    out_fbo_ptr_->CreateDepthStencilBuffer();
    if (!out_fbo_ptr_->IsOk()) {
      assert(0);
      return false;
    }
    renderer_ptr_->SetFBO(out_fbo_ptr_);
    graphics_renderer_ptr_->SetFBO(out_fbo_ptr_);

    return true;
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

  void Manager::SetAnchorPoints(float *polyx, float *polyy, int count)
  {
    std::vector<glm::vec2> polygon;
    for (int i = 0; i < count; ++i)
      polygon.push_back(glm::vec2(polyx[i], polyy[i]));
    renderer_ptr_->SetAnchorPoints(polygon);
  }

  void Manager::SetControlPoints(float *polyx,
    float *polyy,
    int count)
  {
    std::vector<glm::vec2> polygon;
    for (int i = 0; i < count; ++i)
      polygon.push_back(glm::vec2(polyx[i], polyy[i]));
    renderer_ptr_->SetControlPoints(polygon);
  }

  bool Manager::PostSelection(float mouseX, float mouseY)
  {
    return renderer_ptr_->PostSelection(glm::vec2(mouseX, mouseY));
  }

  bool Manager::Deformation(float mouseX, float mouseY)
  {
    return renderer_ptr_->Deformation(glm::vec2(mouseX, mouseY));
  }

  void Manager::CancelDeform()
  {
    return renderer_ptr_->CancelDeform();
  }

  void  Manager::RestoreMesh()
  {
    return renderer_ptr_->RestoreMesh();
  }

  void Manager::CutMesh()
  {
    renderer_ptr_->CutMesh();
  }

  bool Manager::OnMouseDown_GraphicsOverlay(float x, float y)
  {
    return graphics_renderer_ptr_->OnMouseDown(glm::vec2(x, y));
  }

  bool Manager::OnMouseMove_GraphicsOverlay(float x, float y)
  {
    return graphics_renderer_ptr_->OnMouseMove(glm::vec2(x, y));
  }

  bool Manager::OnMouseUp_GraphicsOverlay(float x, float y)
  {
    return graphics_renderer_ptr_->OnMouseUp(glm::vec2(x, y));
  }

  void Manager::SetToolType(int type)
  {
    graphics_renderer_ptr_->SetToolType(
      GraphicsRenderer::ToolType(type));
  }

  int Manager::GetToolType()
  {
    return graphics_renderer_ptr_->GetToolType();
  }

  void Manager::RemoveAllGraphics()
  {
    graphics_renderer_ptr_->RemoveAll();
  }

  void Manager::RemoveSelection()
  {
    graphics_renderer_ptr_->RemoveSelection();
  }

  void Manager::AddCylinder(float mouseX, float mouseY)
  {
    renderer_ptr_->AddCylinder(mouseX, mouseY);
  }

  bool Manager::CheckSelection(float mouseX, float mouseY)
  {
    return renderer_ptr_->CheckSelection(mouseX, mouseY);
  }

  bool Manager::GetSelectionProperty(float properties[3])
  {
    return renderer_ptr_->GetSelectionProperty(properties);
  }

  void Manager::SetInnerRadius(float r)
  {
    return renderer_ptr_->SetInnerRadius(r);
  }

  void Manager::SetOuterRadius(float r)
  {
    return renderer_ptr_->SetOuterRadius(r);
  }

  void Manager::SetHeight(float h)
  {
    return renderer_ptr_->SetHeight(h);
  }
}