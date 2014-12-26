#include "GLUtility.h"
#include <cassert>
#include <iostream>
#include <tchar.h>
#include <GL/glew.h>

#pragma warning(disable:4996)

namespace hj
{
  GLUtility::GLUtility(void)
  {
  }

  GLUtility::~GLUtility(void)
  {
  }

  bool GLUtility::CheckOpenGLError(const std::string &task /*= ""*/)
  {
    // Clear the previous error flag
    bool SuccessFlag = true;

    GLenum err = glGetError();
    // Make sure we get all error flags before we continue, 
    // but don't loop more than 10 times
    const int MAX_REPEAT_ERROR = 10;
    int i = 0;
    while (err != GL_NO_ERROR && i < MAX_REPEAT_ERROR)
    {
      SuccessFlag = false;
      const char *errstring = 
        reinterpret_cast<const char*>(gluErrorString(err));
      const GLubyte *errString2 = gluErrorString(err); errString2;
      std::cerr << task << " FAILED (err[" << (i+1) << "]=" << err << 
        ", \"" << (errstring ? errstring : "???") << "\")" << std::endl;
      assert(0);

      err = glGetError();
      ++i;
      if (i == MAX_REPEAT_ERROR)
      {
        // This should not happen, but apparently it does
        // glGetError should return 0 if it generates an error itself
        std::cerr << "GLUtility: 10 consecutive errors reached, "
          << "are you missing a glEnd()?" << std::endl;
        assert(0);
      }
    }

    return SuccessFlag;
  }

  std::string GLUtility::GetShaderSource(const std::string &filename)
  {
    if(!filename.size()) {
      assert(0);
    }
    FILE* fp = fopen(filename.c_str(), "rt");
    if (!fp) {
      assert(0);
    }
    // Gets length of the file.
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    //  Allocates a buffer for the source code.
    std::string src_string = "";
    if (length > 0) {
      char* source = new char[length + 1];
      length = static_cast<int>(fread(source,sizeof(char), 
        static_cast<size_t>(length), fp));
      source[length] = '\0';
      src_string = std::string(source);
      delete []source;
    } 
    else {  // Empty file.
      assert(0);
    }
    fclose(fp);
    return src_string;
  }

}
