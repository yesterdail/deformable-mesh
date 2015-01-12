#include "GLFramebuffer.h"
#include "GLTexture.h"
#include <cassert>
#include <tchar.h>

namespace hj
{

  GLRendertarget::GLRendertarget(int width, int height)
    : width_(width)
    , height_(height)
    , count_(0)
  {

  }

  GLRendertarget::~GLRendertarget()
  {

  }

  void GLRendertarget::AttachToBoundFBO(int /*attachment*/)
  {
    ++count_;
  }

  void GLRendertarget::DetachFromBoundFBO(int attachment)
  {
    // It doesn't matter which render buffer target we use here
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, 
      attachment, GL_RENDERBUFFER_EXT, 0);

    --count_;
  }

  GLRenderbuffer::GLRenderbuffer(int width, int height, int format)
    : GLRendertarget(width, height)
    , id_(0)
    , format_(format)
  {
    glGenRenderbuffersEXT(1, &id_);
    Allocate();
  }

  GLRenderbuffer::~GLRenderbuffer()
  {
    glDeleteRenderbuffersEXT(1, &id_);
  }

  void GLRenderbuffer::AttachToBoundFBO(int attachment)
  {
    GLRendertarget::AttachToBoundFBO(attachment);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, 
      attachment, GL_RENDERBUFFER_EXT, id_);
  }

  void GLRenderbuffer::Allocate()
  {
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, id_);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, 
      format_, width_, height_);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
  }

  bool GLRenderbuffer::Resize(int width, int height)
  {
    width_ = width;
    height_ = height;
    Allocate();
    return GL_NO_ERROR == glGetError();
  }

  GLRenderTexture2D::GLRenderTexture2D(int width, int height, int internalformat, int format, int type)
    : GLRendertarget(width, height)
    , texture_(0)
  {
    texture_ = new GLTexture(glm::ivec3(width_, height_, 1), internalformat, format, type);
  }

  GLRenderTexture2D::~GLRenderTexture2D()
  {
    DEL_PTR(texture_);
  }

  bool GLRenderTexture2D::Resize(int width, int height)
  {
    int internalformat = texture_->GetInternalFormat();
    int format = texture_->GetFormat();
    int datatype = texture_->GetDataType();
    // Delete the existing texture
    DEL_PTR(texture_);
    width_ = width;
    height_ = height;
    texture_ = new GLTexture(glm::ivec3(width_, height_, 1), internalformat, format, datatype);
    return true;
  }

  void GLRenderTexture2D::AttachToBoundFBO(int attachment)
  {
    GLRendertarget::AttachToBoundFBO(attachment);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
      attachment, texture_->GetTextureTarget(), texture_->GetId(), 0);
  }

  GLFramebuffer::GLFramebuffer(int width, int height)
    : id_(0)
    , width_(width)
    , height_(height)
    , bound_(false)
  {
    assert(GLEW_EXT_framebuffer_object);
    glGenFramebuffersEXT(1, &id_);
  }

  GLFramebuffer::~GLFramebuffer(void)
  {
    glDeleteFramebuffersEXT(1, &id_);
  }

  void GLFramebuffer::Bind()
  {
    //assert(!bound_);
    // Store old view port
    //glPushAttrib(GL_VIEWPORT_BIT);
    // Bind FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER, id_);
    // Set view port
    glViewport(0, 0, width_, height_);
    bound_ = true;
  }

  void GLFramebuffer::Unbind()
  {
    //assert(bound_);
    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    // Restore view port
    //glPopAttrib();
    bound_ = false;
  }

  bool GLFramebuffer::IsOk()
  {
    if (!bound_) Bind();
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) 
    {
    case GL_FRAMEBUFFER_COMPLETE:
      return true;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      _tprintf_s(_T("Framebuffer incomplete, incomplete attachment\n"));
      return false;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      _tprintf_s(_T("Unsupported framebuffer format\n"));
      return false;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      _tprintf_s(_T("Framebuffer incomplete, missing attachment\n"));
      return false;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      _tprintf_s(_T("Framebuffer incomplete, attached images must have same dimensions\n"));
      return false;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      _tprintf_s(_T("Framebuffer incomplete, attached images must have same format\n"));
      return false;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      _tprintf_s(_T("Framebuffer incomplete, missing draw buffer\n"));
      return false;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      _tprintf_s(_T("Framebuffer incomplete, missing read buffer\n"));
      return false;
    default:
      _tprintf_s(_T("Warning:Other Errors!\n"));
      return false;
    }
  }

  bool GLFramebuffer::CreateColorBuffer(
    int attachment /*= GL_COLOR_ATTACHMENT0_EXT*/, 
    int format /*= GL_RGBA*/)
  {
    GLRendertarget* buffer = new GLRenderbuffer(width_, height_, format);
    GLRendertarget* oldtarget = Attach(attachment, buffer);
    if(oldtarget) {
      if(oldtarget->GetCount() == 0) delete oldtarget;
    }
    return true;
  }

  bool GLFramebuffer::CreateColorTexture(
    int attachment /*= GL_COLOR_ATTACHMENT0_EXT*/, 
    int internalformat /*= GL_RGBA8*/, 
    int format /*= GL_RGBA*/, 
    int type /*= GL_UNSIGNED_BYTE*/)
  {
    GLRendertarget* rendertexture = new GLRenderTexture2D(width_, height_, internalformat, format, type);
    GLRendertarget* oldtarget = Attach(attachment, rendertexture);
    if(oldtarget) {
      if(oldtarget->GetCount() == 0) delete oldtarget;
    }
    return true;
  }

  bool GLFramebuffer::CreateDepthBuffer(int format /*= GL_DEPTH_COMPONENT*/)
  {
    return CreateColorBuffer(GL_DEPTH_ATTACHMENT, format);
  }

  bool GLFramebuffer::CreateDepthTexture(int format /*= GL_DEPTH_COMPONENT*/)
  {
    return CreateColorTexture(GL_DEPTH_ATTACHMENT, format, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE);
  }

  bool GLFramebuffer::CreateDepthStencilBuffer(int format /*= GL_DEPTH24_STENCIL8*/)
  {
    GLRendertarget* buffer = new GLRenderbuffer(width_, height_, format);
    GLRendertarget* oldtarget = NULL;
    oldtarget = Attach(GL_DEPTH_ATTACHMENT, buffer);
    if (oldtarget) {
      if (oldtarget->GetCount() == 0) delete oldtarget;
    }
    oldtarget = Attach(GL_STENCIL_ATTACHMENT, buffer);
    if (oldtarget) {
      if (oldtarget->GetCount() == 0) delete oldtarget;
    }
    return true;
  }

  GLRendertarget* GLFramebuffer::Attach(int attachment, GLRendertarget* target)
  {
    if(!bound_) Bind();

    GLRendertarget* oldtarget = Detach(attachment);
    target->AttachToBoundFBO(attachment);
    attachments_[attachment] = target;
    return oldtarget;
  }

  GLRendertarget* GLFramebuffer::Detach(int attachment)
  {
    if(!bound_) Bind();

    GLRendertarget* target = nullptr;
    std::map<int, GLRendertarget*>::iterator iter = attachments_.find(attachment);
    if(iter != attachments_.end()) {
      target = iter->second;
      target->DetachFromBoundFBO(attachment);
    }
    attachments_.erase(attachment);
    return target;
  }

  GLTexture * GLFramebuffer::GetTexture2D(int attachment /*= GL_COLOR_ATTACHMENT0_EXT*/)
  {
    std::map<int, GLRendertarget*>::iterator iter = attachments_.find(attachment);
    if(iter != attachments_.end()) {
      GLRenderTexture2D* rendertexter = dynamic_cast<GLRenderTexture2D*>(iter->second);
      if(rendertexter) {
        return rendertexter->GetTexture();
      }
    }
    return nullptr;
  }

  bool GLFramebuffer::Resize(int width, int height)
  {
    if (width_ == width && height_ == height) return true;

    bool ok = true;

    // Resize all attachments
    std::map<int, GLRendertarget*>::iterator iter = attachments_.begin();
    for (; iter != attachments_.end(); iter++) 
    {
      int attachment = iter->first;
      // We need to detach, because the internal render target might change
      GLRendertarget *target = Detach(attachment);
      if (!target->Resize(width, height))
      {
        assert(0);
        // Try to continue without this attachment (caller's decision)
        delete target;
        target = 0;
        ok = false;
      }
      // Re-attach the render target
      if (target) Attach(attachment, target);
    }

    // Update frame buffer size
    width_ = width;
    height_ = height;

    // Unbind the frame buffer if bound
    if (bound_) Unbind();

    return ok;
  }


}
