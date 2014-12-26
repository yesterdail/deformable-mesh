#ifndef HJ_GLUtility_h__
#define HJ_GLUtility_h__

#include <string>

namespace hj
{
  class GLUtility
  {
  public:
    ~GLUtility(void);

    // Error handling
    static bool CheckOpenGLError(const std::string &task = "");

    static std::string GetShaderSource(const std::string &filename);

  private:
    GLUtility(); // Class can not be instantiated
  };

}

#endif // HJ_GLUtility_h__
