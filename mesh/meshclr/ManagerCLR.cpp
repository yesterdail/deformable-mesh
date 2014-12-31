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
}
