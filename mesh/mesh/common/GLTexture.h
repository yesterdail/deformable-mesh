#ifndef GLTexture_h__
#define GLTexture_h__

#include <GL/glew.h>
#include "glmext.h"

namespace hj
{
  class GLTexture
  {
  public:
    enum Filter {
      NEAREST,
      LINEAR,
      MIPMAP
    };

    /**
    * Without data and with internalformat argument, type_ is calculated by
    * dimensions and a new chunk of data will be allocated on the heap.
    */
    GLTexture(const glm::ivec3& dimensions, GLint internalformat, GLint format,
      GLenum dataType  = GL_UNSIGNED_BYTE, Filter filter = LINEAR);

    /**
    * With data and internalformat argument, type_ is calculated by
    * dimensions and no new chunk of data will be allocated on the heap.
    */
    GLTexture(GLubyte* data, const glm::ivec3& dimensions, GLint internalformat, GLint format,
            GLenum dataType = GL_UNSIGNED_BYTE, Filter filter = LINEAR);

    ~GLTexture(void);

    /**
    * Bind the texture to the active texture unit and target.
    * Note: This does not enable texturing (use enable()).
    */
    void Bind() const;

    void Unbind() const;

    /**
    * Enable texturing on the active texture unit.
    */
    void Enable() const;

    /**
    * Disable texturing on the active texture unit
    */
    void Disable() const;

    /**
     * Upload Texture to graphics-card. Binds the texture.
     * type_, format_, internalformat_, dimensions, dataType_ and the pixels_ pointer have to
     * be set before calling this method.
     */
    void UploadTexture(const GLubyte* pixels);

    /**
     * Download texture from the GPU to a preallocated buffer. Binds the texture.
     * type_, format_, dimensions, and dataType_ have to be set before
     * calling this method!
     */
     void DownloadTexture(GLubyte* pixels, size_t numBytesAllocated) const;

    /**
    *   Return OpenGL texture ID
    */
    GLuint GetId() const { return id_; }

    /**
    * Returns type of the texture (GL_TEXTURE_1D...3D, GL_TEXTURE_RECTANGLE_ARB)
    */
    GLenum GetTextureTarget() const { return type_; }

    glm::ivec3 GetDimensions() const { return dimensions_;}
    int GetWidth() const { return dimensions_.x; };
    int GetHeight() const { return dimensions_.y; }
    int GetDepth() const { return dimensions_.z; }
    GLint GetFormat() const { return format_; }
    GLint GetInternalFormat() const { return internalformat_; }
    Filter GetFilter() const { return filter_; }
    GLenum GetDataType() const { return dataType_; }
    size_t GetNumChannels() const { return CalcNumChannels(format_); }

    /**
    * Returns the Bytes Per Pixel used,
    * usually 3 or 4 for 24/32 bits per pixel.
    */
    size_t GetBpp() const { return bpp_; }

  private:
    // used internally in the constructors
    void Init();

    /// calculates the type_ (GL_TEXTURE_1D, GL_TEXTURE_2D or GL_TEXTURE_3D) from dimensions_.
    static GLenum CalcType(const glm::ivec3& dimensions);

    /// calculates the bytes per pixel from format dataType and dataType.
    static size_t CalcBpp(GLint format, GLenum dataType);

    /// calculates the bytes per pixel from the internal format.
    static size_t CalcBpp(GLint internalformat);

    /// calculates the number of channels from the passed texture format
    static size_t CalcNumChannels(GLint format);

    /**
    * Generate OpenGL texture ID
    * @return The generated ID
    */
    GLuint GenerateId();

    /**
    * Applies the stored filter once again. Binds the texture.
    */
    void ApplyFilter();

  private:
    glm::ivec3 dimensions_;
    GLint format_;          ///< GL_RGB...
    GLint internalformat_;  ///< GL_RGB...
    GLenum dataType_;       ///< GL_UNSIGNED_BYTE
    Filter filter_;
    GLuint wrapping_;

    GLuint id_;             ///< OpenGL texture id

    GLenum type_;           ///< 1D, 2D, 3D
    size_t bpp_;            ///< bytes per pixel
  };

}
#endif // GLTexture_h__
