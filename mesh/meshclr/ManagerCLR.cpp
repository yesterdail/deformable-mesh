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
}
