#ifndef GLFramebuffer_h__
#define GLFramebuffer_h__

#include "Config.h"
#include <GL/glew.h>
#include <map>

namespace hj
{
  class GLTexture;

  class GLRendertarget
  {
  public:
    GLRendertarget(int width, int height);

    virtual ~GLRendertarget();

    virtual bool Resize(int width, int height) = 0;

    virtual void AttachToBoundFBO(int attachment);

    virtual void DetachFromBoundFBO(int attachment);

    int GetCount() const {return count_;}

  protected:
    int width_;
    int height_;
    int count_;
  };

  class GLRenderbuffer : public GLRendertarget
  {
  public:
    GLRenderbuffer(int width, int height, int format);

    virtual ~GLRenderbuffer();

    virtual bool Resize(int width, int height);

    virtual void AttachToBoundFBO(int attachment);

  private:
    void Allocate();

    GLuint id_;
    int format_;
  };

  class GLRenderTexture2D : public GLRendertarget
  {
  public:
    GLRenderTexture2D(int width, int height, int internalformat, 
      int format, int type);

    virtual ~GLRenderTexture2D();

    virtual bool Resize(int width, int height);

    virtual void AttachToBoundFBO(int attachment);

    GLTexture* GetTexture() { return texture_; }
    const GLTexture* GetTexture() const { return texture_; }

  private:
    GLTexture* texture_;
  };

  class GLFramebuffer
  {
  public:
    GLFramebuffer(int width, int height);

    ~GLFramebuffer(void);

    void Bind();

    void Unbind();

    bool IsOk();

    bool Resize(int width, int height);

    int GetWidth() const { return width_; }
    
    int GetHeight() const { return height_; }

    GLuint GetID() const {return id_; }

    GLTexture *GetTexture2D(int attachment = GL_COLOR_ATTACHMENT0_EXT);

    bool CreateColorBuffer(
      int attachment = GL_COLOR_ATTACHMENT0_EXT, 
      int format = GL_RGBA);

    bool CreateColorTexture(
      int attachment = GL_COLOR_ATTACHMENT0_EXT, 
      int internalformat = GL_RGBA8, 
      int format = GL_RGBA, 
      int type = GL_UNSIGNED_BYTE);

    bool CreateDepthBuffer(int format = GL_DEPTH_COMPONENT);

    bool CreateDepthTexture(int format = GL_DEPTH_COMPONENT);

    bool CreateDepthStencilBuffer(int format = GL_DEPTH24_STENCIL8);

  private:
    GLRendertarget* Attach(int attachment, GLRendertarget* target);

    GLRendertarget* Detach(int attachment);

  private:
    GLuint id_;
    int width_;
    int height_;
    bool bound_;

    std::map<int, GLRendertarget*> attachments_;
  };
}
#endif // GLFramebuffer_h__
