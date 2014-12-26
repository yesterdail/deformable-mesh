#include "GLTexture.h"

namespace hj
{
  GLTexture::GLTexture(const glm::ivec3& dimensions, GLint internalformat, GLint format,
    GLenum dataType, Filter filter)
    : dimensions_(dimensions)
    , format_(format)
    , internalformat_(internalformat)
    , dataType_(dataType)
    , filter_(filter)
    , wrapping_(GL_REPEAT)
    , id_(0)
  {
    Init();

    Enable();
    UploadTexture(nullptr);
    Disable();
  }

  GLTexture::GLTexture(GLubyte* data, const glm::ivec3& dimensions, GLint internalformat, GLint format, 
    GLenum dataType /*= GL_UNSIGNED_BYTE*/, Filter filter /*= LINEAR*/)
    : dimensions_(dimensions)
    , format_(format)
    , internalformat_(internalformat)
    , dataType_(dataType)
    , filter_(filter)
    , wrapping_(GL_REPEAT)
    , id_(0)
  {
    Init();

    Enable();
    UploadTexture(data);
    Disable();
  }

  GLTexture::~GLTexture(void)
  {
    if (id_)
      glDeleteTextures(1, &id_);
  }

  void GLTexture::Init()
  {
    type_ = CalcType(dimensions_);
    bpp_ = CalcBpp(format_, dataType_);
    GenerateId();
    ApplyFilter();
  }

  GLenum GLTexture::CalcType(const glm::ivec3& dimensions)
  {
    GLenum type;
    if (dimensions.z == 1) {
      if (dimensions.y == 1)
        type = GL_TEXTURE_1D;
      else
        type = GL_TEXTURE_2D;
    }
    else {
      type = GL_TEXTURE_3D;
    }
    return type;
  }

  size_t GLTexture::CalcBpp(GLint format, GLenum dataType)
  {
    size_t numChannels = CalcNumChannels(format);

    size_t typeSize = 0;
    switch (dataType) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
      typeSize = 1;
      break;

    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
      typeSize = 2;
      break;

    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
      typeSize = 4;
      break;

    default:
      assert(0);
    }

    return typeSize * numChannels;
  }

  size_t GLTexture::CalcBpp(GLint internalformat)
  {
    size_t bpp = 0;
    switch (internalformat) { 
    case 1: 
    case GL_COLOR_INDEX: 
    case GL_RED: 
    case GL_GREEN: 
    case GL_BLUE: 
    case GL_ALPHA: 
    case GL_INTENSITY: 
    case GL_LUMINANCE: 
    case GL_DEPTH_COMPONENT: 
      bpp = 1; 
      break; 

    case 2: 
    case GL_LUMINANCE_ALPHA: 
    case GL_INTENSITY16: 
    case GL_DEPTH_COMPONENT16: 
      bpp = 2; 
      break; 

    case GL_RGB: 
    case GL_BGR: 
    case GL_DEPTH_COMPONENT24: 
      bpp = 3; 
      break; 

    case GL_RGBA: 
    case GL_RGBA8: 
    case GL_BGRA: 
    case GL_DEPTH_COMPONENT32: 
      bpp = 4; 
      break; 

    case GL_RGB16: 
    case GL_RGB16F_ARB: 
      bpp = 6; 
      break; 

    case GL_RGBA16: 
    case GL_RGBA16F_ARB: 
      bpp = 8; 
      break; 

    case GL_RGB32F_ARB:
      bpp = 12;
      break;

    case GL_RGBA32F_ARB:
      bpp = 16;
      break;

    default: 
      assert(0);
      break; 
    }

    return bpp;
  }

  size_t GLTexture::CalcNumChannels(GLint format)
  {
    switch (format) {
    case 1:
    case GL_COLOR_INDEX:
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_INTENSITY:
    case GL_LUMINANCE:
    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_COMPONENT24:
    case GL_ALPHA_INTEGER_EXT:
      return 1;
      break;

    case 2:
    case GL_LUMINANCE_ALPHA:
      return 2;
      break;

    case 3:
    case GL_RGB:
    case GL_BGR:
      return 3;
      break;

    case 4:
    case GL_RGBA:
    case GL_BGRA:
    case GL_RGBA16:
    case GL_RGBA16F_ARB:
      return 4;
      break;

    default:
      assert(0);
      return 0;
    }
  }

  GLuint GLTexture::GenerateId()
  {
    id_ = 0; 
    glGenTextures(1, &id_);
    return id_;
  }

  void GLTexture::Bind() const
  {
    glBindTexture(type_ , id_);
  }

  void GLTexture::Unbind() const
  {
    glBindTexture(type_ , 0);
  }

  void GLTexture::Enable() const
  {
    glEnable(type_);
  }

  void GLTexture::Disable() const
  {
    glDisable(type_);
  }

  void GLTexture::ApplyFilter()
  {
    Bind();

    switch(filter_) {
    case NEAREST:
      glTexParameteri(type_,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
      glTexParameteri(type_,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      break;

    case LINEAR:
      glTexParameteri(type_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      break;

    case MIPMAP:
      glTexParameteri(type_,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(type_,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(type_, GL_SGIS_generate_mipmap, GL_TRUE);
      break;
    }
  }

  void GLTexture::UploadTexture(const GLubyte* pixels)
  {
    Bind();

    switch(type_) {
    case GL_TEXTURE_1D:
      glTexImage1D(GL_TEXTURE_1D, 0, internalformat_, dimensions_.x, 0,
        format_, dataType_, pixels);
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // Clamps to edge.
      break;

    case GL_TEXTURE_2D:
      glTexImage2D(GL_TEXTURE_2D, 0, internalformat_, dimensions_.x, dimensions_.y, 0,
        format_, dataType_, pixels);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  // Clamps to edge.
      break;

    case GL_TEXTURE_3D:
      glTexImage3D(GL_TEXTURE_3D, 0, internalformat_,
        dimensions_.x, dimensions_.y, dimensions_.z, 0,
        format_, dataType_, pixels);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);  // Clamps to edge.
      break;
    }
  }

  void GLTexture::DownloadTexture(GLubyte* pixels, size_t numBytesAllocated) const
  {
    Bind();

    size_t arraySize = glm::hmul(dimensions_) * bpp_;
    if(numBytesAllocated < arraySize) {
      assert(0);
    }
    else {
      glGetTexImage(type_, 0, format_, dataType_, pixels);
    }
  }



}
