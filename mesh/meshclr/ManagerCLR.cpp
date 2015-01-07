#include "ManagerCLR.h"


namespace hj
{
  ManagerCLR::ManagerCLR()
  {
    mgr_ptr_ = new Manager();
  }

  ManagerCLR::~ManagerCLR()
  {
    this->!ManagerCLR();
  }

  ManagerCLR::!ManagerCLR()
  {
    delete mgr_ptr_;
    mgr_ptr_ = NULL;
  }

  bool ManagerCLR::CreateView(int viewWidth, int viewHeight)
  {
    return mgr_ptr_->CreateView(viewWidth, viewHeight);
  }

  void ManagerCLR::GetView(array<unsigned char>^ output_buffer)
  {
    pin_ptr<unsigned char> pinned_buffer = &output_buffer[0];
    mgr_ptr_->GetView(pinned_buffer,
      output_buffer->Length);
  }

  bool ManagerCLR::Resize(int new_width, int new_height)
  {
    return mgr_ptr_->Resize(new_width, new_height);
  }

  bool ManagerCLR::LoadMesh(String^ filename)
  {
    std::string native_file_path;
    char char_buffer[8192] = { 0 };
    size_t converted = 0;
    pin_ptr<const wchar_t> pinned_string = PtrToStringChars(filename);
    wcstombs_s(&converted,
      char_buffer,
      sizeof(char_buffer),
      pinned_string,
      filename->Length);
    native_file_path.assign(char_buffer);
    return mgr_ptr_->LoadMesh(native_file_path);
  }

  bool ManagerCLR::LoadTexture(String^ filename)
  {
    std::string native_file_path;
    char char_buffer[8192] = { 0 };
    size_t converted = 0;
    pin_ptr<const wchar_t> pinned_string = PtrToStringChars(filename);
    wcstombs_s(&converted,
      char_buffer,
      sizeof(char_buffer),
      pinned_string,
      filename->Length);
    native_file_path.assign(char_buffer);
    return mgr_ptr_->LoadTexture(native_file_path);
  }

  bool ManagerCLR::ResetCamera()
  {
    return mgr_ptr_->ResetCamera();
  }

  void ManagerCLR::Rotate(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    mgr_ptr_->Rotate(newMouseX,
      newMouseY,
      lastMouseX,
      lastMouseY);
  }

  void ManagerCLR::Zoom(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    mgr_ptr_->Zoom(newMouseX,
      newMouseY,
      lastMouseX,
      lastMouseY);
  }

  void ManagerCLR::Move(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    mgr_ptr_->Move(newMouseX,
      newMouseY,
      lastMouseX,
      lastMouseY);
  }

  void ManagerCLR::SetSmooth()
  {
    mgr_ptr_->SetSmooth();
  }

  void ManagerCLR::SetFlat()
  {
    mgr_ptr_->SetFlat();
  }

  void ManagerCLR::SetWireframe(bool w)
  {
    mgr_ptr_->SetWireframe(w);
  }

  void ManagerCLR::SetSolid(bool s)
  {
    mgr_ptr_->SetSolid(s);
  }

  void ManagerCLR::SetTexture(bool t)
  {
    mgr_ptr_->SetTexture(t);
  }

  void ManagerCLR::SetAnchorPoints(array<float>^ polyx,
    array<float>^ polyy,
    int count)
  {
    pin_ptr<float> pinned_polyx = &polyx[0];
    pin_ptr<float> pinned_polyy = &polyy[0];
    return mgr_ptr_->SetAnchorPoints(pinned_polyx, pinned_polyy, count);
  }

  void ManagerCLR::SetControlPoints(array<float>^ polyx,
    array<float>^ polyy,
    int count)
  {
    pin_ptr<float> pinned_polyx = &polyx[0];
    pin_ptr<float> pinned_polyy = &polyy[0];
    return mgr_ptr_->SetControlPoints(pinned_polyx, pinned_polyy, count);
  }

  bool ManagerCLR::PostSelection(float mouseX, float mouseY)
  {
    return mgr_ptr_->PostSelection(mouseX, mouseY);
  }

  bool ManagerCLR::Deformation(float mouseX, float mouseY)
  {
    return mgr_ptr_->Deformation(mouseX, mouseY);
  }

  void ManagerCLR::CancelDeform()
  {
    return mgr_ptr_->CancelDeform();
  }

  void ManagerCLR::RestoreMesh()
  {
    return mgr_ptr_->RestoreMesh();
  }

  void ManagerCLR::SetLine(float startX,
    float startY,
    float endX,
    float endY)
  {
    mgr_ptr_->SetLine(startX, startY, endX, endY);
  }

  void ManagerCLR::SetLineDepth(float d)
  {
    mgr_ptr_->SetLineDepth(d);
  }

  float ManagerCLR::GetLineDepth()
  {
    return mgr_ptr_->GetLineDepth();
  }

  bool ManagerCLR::OnMouseDown_GraphicsOverlay(float x, float y)
  {
    return mgr_ptr_->OnMouseDown_GraphicsOverlay(x, y);
  }

  bool ManagerCLR::OnMouseMove_GraphicsOverlay(float x, float y)
  {
    return mgr_ptr_->OnMouseMove_GraphicsOverlay(x, y);
  }

  bool ManagerCLR::OnMouseUp_GraphicsOverlay(float x, float y)
  {
    return mgr_ptr_->OnMouseUp_GraphicsOverlay(x, y);
  }

  void ManagerCLR::SetToolType(int type)
  {
    return mgr_ptr_->SetToolType(type);
  }

  int ManagerCLR::GetToolType()
  {
    return mgr_ptr_->GetToolType();
  }

  void ManagerCLR::RemoveAllGraphics()
  {
    return mgr_ptr_->RemoveAllGraphics();
  }

  void ManagerCLR::RemoveSelection()
  {
    return mgr_ptr_->RemoveSelection();
  }
}
