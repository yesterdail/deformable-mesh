#include "MeshRenderer.h"
#include "common/GLFramebuffer.h"
#include "common/GLTexture.h"

namespace hj
{
  MeshRenderer::MeshRenderer()
    : out_fbo_ptr_(NULL)
  {
  }

  MeshRenderer::~MeshRenderer()
  {
    DEL_PTR(out_fbo_ptr_);
  }

  bool MeshRenderer::Initialize(const glm::ivec2& view_size)
  {
    if (!ResizeOutput(view_size)) {
      return false;
    }

    return true;
  }

  void MeshRenderer::GetPixel(uint8_t* pixels,
    uint32_t pixel_length,
    bool swap_channels)
  {
    if (!pixels) {
      assert(0);
      return;
    }
    if (!Run()) {
      assert(0);
      return;
    }
    GLTexture* texture_ptr = out_fbo_ptr_->GetTexture2D(GL_COLOR_ATTACHMENT0);
    texture_ptr->DownloadTexture(pixels, pixel_length);
    glFinish();
    if (swap_channels) {
      int pixel_count = pixel_length / 3;
      uint8_t* ptr = pixels;
      for (int i = 0; i < pixel_count; ++i) {
        uint8_t tmp = ptr[2];
        ptr[2] = ptr[0];
        ptr[0] = tmp;
        ptr += 3;
      }
    }
  }

  bool MeshRenderer::Run()
  {
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3d(1, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(-0.5, -0.5);
    glVertex2d( 0.5, -0.5);
    glVertex2d( 0.5,  0.5);
    glVertex2d(-0.5,  0.5);
    glEnd();

    return (GL_NO_ERROR == glGetError());
  }

  bool MeshRenderer::ResizeOutput(const glm::ivec2 &new_size)
  {
    DEL_PTR(out_fbo_ptr_);
    out_fbo_ptr_ = new GLFramebuffer(new_size.x, new_size.y);
    out_fbo_ptr_->CreateColorTexture(GL_COLOR_ATTACHMENT0,
      GL_RGB8,
      GL_RGB,
      GL_UNSIGNED_BYTE);
    out_fbo_ptr_->CreateDepthTexture();
    if (!out_fbo_ptr_->IsOk()) {
      assert(0);
      return false;
    }

    return true;
  }
}